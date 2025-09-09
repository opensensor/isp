#include "include/main.h"


  int32_t ae0_interrupt_static()

{
    void* $s0 = system_reg_read(0xa050) << 8 & 0x3000;
    private_dma_cache_sync(0, $s0 + data_b2f3c_4, 0x1000, 0);
    tisp_ae0_get_statistics($s0 + data_b2f3c_5, 0xf001f001);
    
    if (data_b0e00_1 == 1)
    {
        uint32_t dmsc_fc_t3_stren_intp_1 = dmsc_fc_t3_stren_intp;
        data_b0e00_2 = 0;
        *(dmsc_fc_t3_stren_intp_1 + 4) = 0;
    }
    
    return 1;
}

