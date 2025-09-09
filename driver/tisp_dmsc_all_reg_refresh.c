#include "include/main.h"


  int32_t tisp_dmsc_all_reg_refresh(int32_t arg1)

{
    tisp_dmsc_intp(arg1);
    tisp_dmsc_out_opt_cfg();
    tisp_dmsc_uu_par_cfg();
    tisp_dmsc_alias_par_cfg();
    tisp_dmsc_uu_np_cfg();
    tisp_dmsc_sp_d_sigma_3_np_cfg();
    tisp_dmsc_sp_d_w_wei_np_cfg();
    tisp_dmsc_sp_d_b_wei_np_cfg();
    tisp_dmsc_sp_ud_w_wei_np_cfg();
    tisp_dmsc_sp_ud_b_wei_np_cfg();
    tisp_dmsc_dir_par_cfg();
    tisp_dmsc_nor_par_cfg();
    tisp_dmsc_sp_d_par_cfg();
    tisp_dmsc_sp_ud_par_cfg();
    tisp_dmsc_sp_alias_par_cfg();
    tisp_dmsc_rgb_alias_par_cfg();
    tisp_dmsc_fc_par_cfg();
    tisp_dmsc_deir_par_cfg();
    tisp_dmsc_awb_gain_par_cfg();
    tisp_dmsc_deir_rgb_par_cfg();
    tisp_dmsc_d_ud_ns_par_cfg();
    system_reg_write(0x499c, 1);
    return 0;
}

