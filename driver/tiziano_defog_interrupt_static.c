#include "include/main.h"


  int32_t tiziano_defog_interrupt_static()

{
    tiziano_defog_set_reg_params();
    int32_t $v0 = system_reg_read(0x5ba4);
    int32_t $v0_1 = data_b2f88_2;
    int32_t $v0_2 = $v0_1 + 0x1000;
    
    if ($v0 == $v0_1)
    {
        private_dma_cache_sync(0, data_b2f84_5, 0x1000, 0);
        tiziano_defog_get_data(data_b2f84_6);
        $v0_2 = data_b2f88_3 + 0x1000;
    }
    
    int32_t $v0_4;
    
    if ($v0 != $v0_2)
        $v0_4 = data_b2f88_4;
    else
    {
        private_dma_cache_sync(0, data_b2f84_7 + 0x1000, 0x1000, 0);
        tiziano_defog_get_data(data_b2f84_8 + 0x1000);
        $v0_4 = data_b2f88_5;
    }
    
    int32_t $v0_6;
    
    if ($v0 != $v0_4 + 0x2000)
        $v0_6 = data_b2f88_6;
    else
    {
        private_dma_cache_sync(0, data_b2f84_9 + 0x2000, 0x1000, 0);
        tiziano_defog_get_data(data_b2f84_10 + 0x2000);
        $v0_6 = data_b2f88_7;
    }
    
    if ($v0 == $v0_6 + 0x3000)
    {
        private_dma_cache_sync(0, data_b2f84_11 + 0x3000, 0x1000, 0);
        tiziano_defog_get_data(data_b2f84_12 + 0x3000);
    }
    
    int32_t var_38_32 = 3;
    void var_40_28;
    tisp_event_push(&var_40_29);
    return 1;
}

