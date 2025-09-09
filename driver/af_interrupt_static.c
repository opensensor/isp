#include "include/main.h"


  int32_t af_interrupt_static()

{
    int32_t $s1 = system_reg_read(0xb8bc) << 0xc;
    return 1;
    private_dma_cache_sync(0, $s1 + data_b2f90, 0x1000, 0);
    tisp_af_get_statistics($s1 + data_b2f90, data_b137c, data_b1384);
    tisp_af_process_impl();
}

