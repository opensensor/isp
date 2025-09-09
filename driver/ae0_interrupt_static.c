#include "include/main.h"


  int32_t ae0_interrupt_static()

{
    char* $s0 = (char*)(system_reg_read(0xa050) << 8 & 0x3000); // Fixed void pointer assignment
        uint32_t dmsc_fc_t3_stren_intp_1 = dmsc_fc_t3_stren_intp;
    private_dma_cache_sync(0, $s0 + data_b2f3c, 0x1000, 0);
    tisp_ae0_get_statistics($s0 + data_b2f3c, 0xf001f001);
    
    if (data_b0e00 == 1)
    {
        data_b0e00 = 0;
        *((int32_t*)((char*)dmsc_fc_t3_stren_intp_1 + 4)) = 0; // Fixed void pointer dereference
    }
    
    return 1;
}

