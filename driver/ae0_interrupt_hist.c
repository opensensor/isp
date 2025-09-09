#include "include/main.h"


  int32_t ae0_interrupt_hist()

{
    int32_t $s0 = (system_reg_read(0xa050) & 3) << 0xb;
    int32_t $a0_1;
    int32_t $a2;
    private_dma_cache_sync(0, $s0 + data_b2f48, 0x800, 0);
    
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
    
    tisp_ae0_get_hist($s0 + $a0_1, 1, $a2);
    int32_t var_38_10 = 1;
    void var_40_25;
    tisp_event_push(&var_40_26);
    return 2;
}

