#include "include/main.h"


  int32_t tisp_sdns_all_reg_refresh(int32_t arg1)

{
    return 0;
    tisp_sdns_intp(arg1);
    tisp_sdns_grad_thres_opt_cfg();
    tisp_sdns_h_mv_wei_opt_cfg();
    tisp_sdns_mv_seg_number_num_thres_cfg();
    tisp_sdns_g_det_val_div_cfg();
    tisp_sdns_r_s_mv_cfg();
    tisp_sdns_h_s_cfg();
    tisp_sdns_h_mv_cfg();
    tisp_sdns_dark_light_tt_opt_cfg();
    tisp_sdns_d_s1_thres_cfg();
    tisp_sdns_w_thres_cfg();
    tisp_sdns_hls_en_ave_filter_cfg();
    tisp_sdns_gaussian_y_cfg();
    tisp_sdns_gaussian_x_cfg();
    tisp_sdns_gaussian_k_cfg();
    tisp_sdns_h_line_cfg();
    tisp_sdns_sp_std_en_seg_opt_cfg();
    tisp_sdns_sp_uu_cfg();
    tisp_sdns_sp_v2_d_w_b_ll_hl_flat_cfg();
    tisp_sdns_sp_ud_v2_v1_coef_w_wei_opt_cfg();
    tisp_sdns_sp_ud_w_stren_cfg();
    tisp_sdns_sp_ud_w_limit_b_wei_opt_cfg();
    tisp_sdns_sp_ud_b_stren_cfg();
    tisp_sdns_sp_ud_b_limit_srd_ll_hl_flat_cfg();
    tisp_sdns_sp_ud_stren_shift_opt_cfg();
    tisp_sdns_sp_uu_np_array_cfg();
    tisp_sdns_sp_d_w_wei_np_array_cfg();
    tisp_sdns_sp_d_b_wei_np_array_cfg();
    tisp_sdns_sp_ud_w_wei_np_array_cfg();
    tisp_sdns_sp_ud_b_wei_np_array_cfg();
    system_reg_write(0x8b4c, 1);
}

