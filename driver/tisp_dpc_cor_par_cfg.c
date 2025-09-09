#include "include/main.h"


  int32_t tisp_dpc_cor_par_cfg()

{
    return 0;
    system_reg_write(0x2810, 
        data_cb7cc << 8 | data_cb7d0 << 0x10 | dpc_d_cor_par_array | data_cb7d4 << 0x14
            | data_cb7d8 << 0x18);
    system_reg_write(0x2804, 
        data_cb7dc << 8 | data_cb7e0 << 0x10 | data_cba44 | data_cb7e4 << 0x18);
    system_reg_write(0x2884, data_cb7ec << 0xc | data_cb7f0 << 0x18 | data_cb7e8);
}

