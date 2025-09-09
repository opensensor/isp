#include "include/main.h"


  int32_t tisp_dpc_cor_par_cfg()

{
    system_reg_write(0x2810, 
        data_cb7cc_1 << 8 | data_cb7d0_1 << 0x10 | dpc_d_cor_par_array | data_cb7d4_1 << 0x14
            | data_cb7d8_1 << 0x18);
    system_reg_write(0x2804, 
        data_cb7dc_1 << 8 | data_cb7e0_1 << 0x10 | data_cba44_1 | data_cb7e4_1 << 0x18);
    system_reg_write(0x2884, data_cb7ec_1 << 0xc | data_cb7f0_1 << 0x18 | data_cb7e8_1);
    return 0;
}

