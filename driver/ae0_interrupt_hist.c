#include "include/main.h"

int32_t ae0_interrupt_hist()
{
    int32_t $s0;
    int32_t $a0_1;
    int32_t $a2;
    int32_t var_38_10 = 1;
    struct tisp_event var_40_26; /* Proper type for event structure */

    $s0 = (system_reg_read(0xa050) & 3) << 0xb;

    /* Call private_dma_cache_sync - but check function signature first */
    /* If it takes no parameters as declared: */
    private_dma_cache_sync();
    /* If it should take parameters, update the declaration in functions.h */

    if (data_b0e10 != 1)
    {
        $a0_1 = data_b2f48;
        $a2 = 1;
    }
    else
    {
        $a0_1 = data_b2f48;
        $a2 = 0;
    }

    tisp_ae0_get_hist((int32_t*)($s0 + $a0_1), 1, $a2);
    tisp_event_push(&var_40_26);

    return 2;
}