#include "include/main.h"


  int32_t tisp_sdns_intp_reg_refresh(int32_t arg1)

{
    tisp_sdns_intp(arg1);
    tisp_sdns_grad_thres_opt_cfg();
    tisp_sdns_mv_seg_number_num_thres_cfg();
    tisp_sdns_h_s_cfg();
    tisp_sdns_h_mv_cfg();
    tisp_sdns_dark_light_tt_opt_cfg();
    tisp_sdns_sp_uu_cfg();
    tisp_sdns_sp_v2_d_w_b_ll_hl_flat_cfg();
    tisp_sdns_sp_ud_w_stren_cfg();
    tisp_sdns_sp_ud_b_stren_cfg();
    tisp_sdns_sp_ud_b_limit_srd_ll_hl_flat_cfg();
    return 0;
}

