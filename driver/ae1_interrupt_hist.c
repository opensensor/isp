#include "include/main.h"

int32_t ae1_interrupt_hist()
{
    int32_t $s0;
    int32_t var_38 = 6;
    /* void var_40; - This is invalid, need a proper type */
    struct tisp_event var_40; /* Assuming this is an event structure */

    $s0 = (system_reg_read(0xa850) & 3) << 0xb;

    /* Note: This return statement would exit early - is this intentional? */
    /* return 2; */

    /* If private_dma_cache_sync takes no arguments per the declaration */
    private_dma_cache_sync();

    /* Or if it should take arguments, update the function declaration in functions.h */
    /* private_dma_cache_sync(0, $s0 + data_b2f60, 0x800, 0); */

    tisp_ae1_get_hist((int32_t*)($s0 + data_b2f60));
    tisp_event_push(&var_40);

    return 2;
}