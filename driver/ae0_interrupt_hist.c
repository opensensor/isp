#include "include/main.h"


  int32_t ae0_interrupt_hist()

{
    int32_t $s0 = (system_reg_read(0xa050) & 3) << 0xb;
    private_dma_cache_sync(0, $s0 + data_b2f48_2, 0x800, 0);
    int32_t $a0_1;
    int32_t $a2;
    
    if (data_b0e10_1 != 1)
    {
        $a0_1 = data_b2f48_3;
        $a2 = 1;
    }
    else
    {
        $a0_1 = data_b2f48_4;
        $a2 = 0;
    }
    
    tisp_ae0_get_hist($s0 + $a0_1, 1, $a2);
    int32_t var_38_41 = 1;
    void var_40_42;
    tisp_event_push(&var_40_43);
    return 2;
}

