#include "include/main.h"


  int32_t af_interrupt_static()

{
    int32_t $s1 = system_reg_read(0xb8bc) << 0xc;
    private_dma_cache_sync(0, $s1 + data_b2f90_5, 0x1000, 0);
    tisp_af_get_statistics($s1 + data_b2f90_6, data_b137c_1, data_b1384_1);
    tisp_af_process_impl();
    return 1;
}

