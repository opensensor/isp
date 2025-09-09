#include "include/main.h"


  int32_t tisp_sdns_sp_ud_v2_v1_coef_w_wei_opt_cfg()

{
    system_reg_write(0x8a9c, 
        data_c64a4_1 << 4 | data_c64a8_1 << 8 | sdns_sp_ud_v2_1_coef | data_c64ac_1 << 0xc
            | data_c64b0_1 << 0x10);
    system_reg_write(0x8aa0, 
        data_c64b8_1 << 8 | data_c64bc_1 << 0xc | data_c64b4_1 | sdns_sp_ud_wbhl_flat << 0x10);
    return 0;
}

