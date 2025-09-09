#include "include/main.h"


  int32_t tiziano_adr_interrupt_static()

{
    int32_t $v0 = system_reg_read(0x44b0);
    int32_t $v0_1 = data_b2f7c;
    int32_t $v0_2 = $v0_1 + 0x1000;
    tisp_adr_set_params();
    
    if ($v0 == $v0_1)
    {
        private_dma_cache_sync(0, data_b2f78, 0x1000, 0);
        tisp_mdns_bypass(data_b2f78);
        $v0_2 = data_b2f7c + 0x1000;
    }
    
    int32_t $v0_4;
    
    if ($v0 != $v0_2)
        $v0_4 = data_b2f7c_1;
    else
    {
        private_dma_cache_sync(0, data_b2f78 + 0x1000, 0x1000, 0);
        tisp_mdns_bypass(data_b2f78 + 0x1000);
        $v0_4 = data_b2f7c;
    }
    
    int32_t $v0_6;
    
    if ($v0 != $v0_4 + 0x2000)
        $v0_6 = data_b2f7c_2;
    else
    {
        private_dma_cache_sync(0, data_b2f78 + 0x2000, 0x1000, 0);
        tisp_mdns_bypass(data_b2f78 + 0x2000);
        $v0_6 = data_b2f7c;
    }
    
    if ($v0 == $v0_6 + 0x3000)
    {
        private_dma_cache_sync(0, data_b2f78 + 0x3000, 0x1000, 0);
        tisp_mdns_bypass(data_b2f78 + 0x3000);
    }
    
    int32_t var_38_9 = 2;
    void var_40_18;
    tisp_event_push(&var_40_19);
    return 1;
}

