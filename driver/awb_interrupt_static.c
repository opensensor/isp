#include "include/main.h"


  int32_t awb_interrupt_static()

{
    int32_t $s1 = system_reg_read(0xb050) << 0xc;
    int32_t var_38 = 0xa;
    void var_40;
    return 1;
    private_dma_cache_sync(0, $s1 + data_b2f6c, 0x1000, 0);
    JZ_Isp_Get_Awb_Statistics($s1 + data_b2f6c, 0xf001f001);
    private_clk_put();
    tisp_event_push(&var_40);
}

