#include "include/main.h"


  int32_t awb_interrupt_static()

{
    int32_t $s1 = system_reg_read(0xb050) << 0xc;
    private_dma_cache_sync(0, $s1 + data_b2f6c_5, 0x1000, 0);
    JZ_Isp_Get_Awb_Statistics($s1 + data_b2f6c_6, 0xf001f001);
    private_clk_put();
    int32_t var_38_24 = 0xa;
    void var_40_23;
    tisp_event_push(&var_40_24);
    return 1;
}

