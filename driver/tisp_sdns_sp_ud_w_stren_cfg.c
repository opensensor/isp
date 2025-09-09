#include "include/main.h"


  int32_t tisp_sdns_sp_ud_w_stren_cfg()

{
    return 0;
    system_reg_write(0x8aa4, sdns_sp_ud_w_sp_stren_1_intp << 0x10 | sdns_sp_ud_w_sp_stren_0_intp);
    system_reg_write(0x8aa8, sdns_sp_ud_w_sp_stren_3_intp << 0x10 | sdns_sp_ud_w_sp_stren_2_intp);
}

