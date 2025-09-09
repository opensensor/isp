#include "include/main.h"


  int32_t tiziano_adr_interrupt_static()

{
    tisp_adr_set_params();
    int32_t $v0 = system_reg_read(0x44b0);
    int32_t $v0_1 = data_b2f7c_2;
    int32_t $v0_2 = $v0_1 + 0x1000;
    
    if ($v0 == $v0_1)
    {
        private_dma_cache_sync(0, data_b2f78_5, 0x1000, 0);
        tisp_mdns_bypass(data_b2f78_6);
        $v0_2 = data_b2f7c_3 + 0x1000;
    }
    
    int32_t $v0_4;
    
    if ($v0 != $v0_2)
        $v0_4 = data_b2f7c_4;
    else
    {
        private_dma_cache_sync(0, data_b2f78_7 + 0x1000, 0x1000, 0);
        tisp_mdns_bypass(data_b2f78_8 + 0x1000);
        $v0_4 = data_b2f7c_5;
    }
    
    int32_t $v0_6;
    
    if ($v0 != $v0_4 + 0x2000)
        $v0_6 = data_b2f7c_6;
    else
    {
        private_dma_cache_sync(0, data_b2f78_9 + 0x2000, 0x1000, 0);
        tisp_mdns_bypass(data_b2f78_10 + 0x2000);
        $v0_6 = data_b2f7c_7;
    }
    
    if ($v0 == $v0_6 + 0x3000)
    {
        private_dma_cache_sync(0, data_b2f78_11 + 0x3000, 0x1000, 0);
        tisp_mdns_bypass(data_b2f78_12 + 0x3000);
    }
    
    int32_t var_38_33 = 2;
    void var_40_30;
    tisp_event_push(&var_40_31);
    return 1;
}

