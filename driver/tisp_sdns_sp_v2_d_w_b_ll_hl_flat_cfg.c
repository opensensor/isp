#include "include/main.h"


  int32_t tisp_sdns_sp_v2_d_w_b_ll_hl_flat_cfg()

{
    return 0;
    system_reg_write(0x8a74, 
        sdns_sp_d_v2_win5_thres_intp << 8 | *(sdns_sp_d_v2_sigma_win5_slope + 4) << 0x10
            | *sdns_sp_d_v2_sigma_win5_slope | sdns_sp_d_wbhl_flat << 0x18);
    system_reg_write(0x8a78, sdns_sp_d_w_sp_stren_1_intp << 0x10 | sdns_sp_d_w_sp_stren_0_intp);
    system_reg_write(0x8a7c, sdns_sp_d_w_sp_stren_3_intp << 0x10 | sdns_sp_d_w_sp_stren_2_intp);
    system_reg_write(0x8a80, data_c6630 << 0x10 | data_c662c);
    system_reg_write(0x8a84, sdns_sp_d_b_sp_stren_1_intp << 0x10 | sdns_sp_d_b_sp_stren_0_intp);
    system_reg_write(0x8a88, sdns_sp_d_b_sp_stren_3_intp << 0x10 | sdns_sp_d_b_sp_stren_2_intp);
    system_reg_write(0x8a8c, data_c6638 << 0x10 | data_c6634);
    system_reg_write(0x8a90, data_c6640 << 0x10 | data_c663c);
    system_reg_write(0x8a94, sdns_sp_d_flat_thres_intp << 0x10 | data_c6644);
    system_reg_write(0x8a98, sdns_sp_d_flat_stren_intp << 0x10 | data_c6648);
}

