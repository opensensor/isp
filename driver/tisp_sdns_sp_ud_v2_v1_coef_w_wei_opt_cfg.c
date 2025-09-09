#include "include/main.h"


  int32_t tisp_sdns_sp_ud_v2_v1_coef_w_wei_opt_cfg()

{
    system_reg_write(0x8a9c, 
        data_c64a4 << 4 | data_c64a8 << 8 | sdns_sp_ud_v2_1_coef | data_c64ac << 0xc
            | data_c64b0 << 0x10);
    system_reg_write(0x8aa0, 
        data_c64b8 << 8 | data_c64bc << 0xc | data_c64b4 | sdns_sp_ud_wbhl_flat << 0x10);
    return 0;
}

