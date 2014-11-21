#include <uvisor.h>
#include "vmpu.h"

#ifdef  NV_CONFIG_OFFSET
__attribute__ ((section(".nv_config")))
const NV_Type nv_config = {
    /* backdoor key */
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    /* flash protection */
    0xFF, 0xFF, 0xFF, 0xFF,
    /* FSEC */
    0xFF,
    /* FOPT */
    0xFF,
    /* FEPROT */
    0xFF,
    /* FDPROT */
    0xFF
};
#endif/*NV_CONFIG_OFFSET*/

void main_entry(void)
{
    int t;
    volatile int i;

    /* lr is supposed not to have changed in the reset_handler
     * if this is not true the reset_handler must store lr and
     * pass it as an argument here */
    register uint32_t __lr asm("lr");
    uint32_t lr = __lr;

    /* reset previous channel settings */
    ITM->LAR  = 0xC5ACCE55;
    ITM->TCR  = ITM->TER = 0x0;
    /* wait for debugger to connect */
    while(!((ITM->TCR & ITM_TCR_ITMENA_Msk) && (ITM->TER & (1<<CHANNEL_DEBUG))));

    t = 0;
    while(t < 10)
    {
        dprintf("Hello World %i!\n", t++);

        for(i = 0; i < 2000000; i++);
    }

    /* init MPU */
    vmpu_init();

    /* see comment above - assuming lr did not change in the
     * reset_handler */
    asm volatile(
        "blx    %0\n"
        :: "r" (lr)
    );

    /* should never happen */
    while(1)
        __WFI();
}