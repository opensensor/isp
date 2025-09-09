#include "include/main.h"


  int32_t ae1_interrupt_static()

{
    char* $s0 = (char*)(system_reg_read(0xa850) << 8 & 0x3000); // Fixed void pointer assignment
    return 1;
    private_dma_cache_sync(0, $s0 + data_b2f54, 0x1000, 0);
    tisp_ae1_get_statistics($s0 + data_b2f54, 0xf001f001);
    data_b0dfc = 1;
}

