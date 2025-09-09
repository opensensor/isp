#include "include/main.h"


  int32_t tisp_sdns_intp(int32_t arg1)

{
    int32_t $s2 = arg1 >> 0x10;
    int32_t $s0 = arg1 & 0xffff;
    sdns_grad_zx_thres_intp = tisp_simple_intp($s2, $s0, sdns_grad_zx_thres_array_now);
    sdns_grad_zy_thres_intp = tisp_simple_intp($s2, $s0, sdns_grad_zy_thres_array_now);
    sdns_std_thr1_intp = tisp_simple_intp($s2, $s0, sdns_std_thr1_array_now);
    sdns_std_thr2_intp = tisp_simple_intp($s2, $s0, sdns_std_thr2_array_now);
    sdns_mv_num_thr_5x5_intp = tisp_simple_intp($s2, $s0, &sdns_mv_num_thr_5x5_array);
    sdns_mv_num_thr_7x7_intp = tisp_simple_intp($s2, $s0, &sdns_mv_num_thr_7x7_array);
    sdns_mv_num_thr_9x9_intp = tisp_simple_intp($s2, $s0, &sdns_mv_num_thr_9x9_array);
    sdns_mv_num_thr_11x11_intp = tisp_simple_intp($s2, $s0, &sdns_mv_num_thr_11x11_array);
    sdns_h_s_1_intp = tisp_simple_intp($s2, $s0, sdns_h_s_1_array_now);
    sdns_h_s_2_intp = tisp_simple_intp($s2, $s0, sdns_h_s_2_array_now);
    sdns_h_s_3_intp = tisp_simple_intp($s2, $s0, sdns_h_s_3_array_now);
    sdns_h_s_4_intp = tisp_simple_intp($s2, $s0, sdns_h_s_4_array_now);
    sdns_h_s_5_intp = tisp_simple_intp($s2, $s0, sdns_h_s_5_array_now);
    sdns_h_s_6_intp = tisp_simple_intp($s2, $s0, sdns_h_s_6_array_now);
    sdns_h_s_7_intp = tisp_simple_intp($s2, $s0, sdns_h_s_7_array_now);
    sdns_h_s_8_intp = tisp_simple_intp($s2, $s0, sdns_h_s_8_array_now);
    sdns_h_s_9_intp = tisp_simple_intp($s2, $s0, sdns_h_s_9_array_now);
    sdns_h_s_10_intp = tisp_simple_intp($s2, $s0, sdns_h_s_10_array_now);
    sdns_h_s_11_intp = tisp_simple_intp($s2, $s0, sdns_h_s_11_array_now);
    sdns_h_s_12_intp = tisp_simple_intp($s2, $s0, sdns_h_s_12_array_now);
    sdns_h_s_13_intp = tisp_simple_intp($s2, $s0, sdns_h_s_13_array_now);
    sdns_h_s_14_intp = tisp_simple_intp($s2, $s0, sdns_h_s_14_array_now);
    sdns_h_s_15_intp = tisp_simple_intp($s2, $s0, sdns_h_s_15_array_now);
    sdns_h_s_16_intp = tisp_simple_intp($s2, $s0, sdns_h_s_16_array_now);
    sdns_h_mv_1_intp = tisp_simple_intp($s2, $s0, &sdns_h_mv_1_array);
    sdns_h_mv_2_intp = tisp_simple_intp($s2, $s0, &sdns_h_mv_2_array);
    sdns_h_mv_3_intp = tisp_simple_intp($s2, $s0, &sdns_h_mv_3_array);
    sdns_h_mv_4_intp = tisp_simple_intp($s2, $s0, &sdns_h_mv_4_array);
    sdns_h_mv_5_intp = tisp_simple_intp($s2, $s0, &sdns_h_mv_5_array);
    sdns_h_mv_6_intp = tisp_simple_intp($s2, $s0, &sdns_h_mv_6_array);
    sdns_h_mv_7_intp = tisp_simple_intp($s2, $s0, &sdns_h_mv_7_array);
    sdns_h_mv_8_intp = tisp_simple_intp($s2, $s0, &sdns_h_mv_8_array);
    sdns_h_mv_9_intp = tisp_simple_intp($s2, $s0, &sdns_h_mv_9_array);
    sdns_h_mv_10_intp = tisp_simple_intp($s2, $s0, &sdns_h_mv_10_array);
    sdns_h_mv_11_intp = tisp_simple_intp($s2, $s0, &sdns_h_mv_11_array);
    sdns_h_mv_12_intp = tisp_simple_intp($s2, $s0, &sdns_h_mv_12_array);
    sdns_h_mv_13_intp = tisp_simple_intp($s2, $s0, &sdns_h_mv_13_array);
    sdns_h_mv_14_intp = tisp_simple_intp($s2, $s0, &sdns_h_mv_14_array);
    sdns_h_mv_15_intp = tisp_simple_intp($s2, $s0, &sdns_h_mv_15_array);
    sdns_h_mv_16_intp = tisp_simple_intp($s2, $s0, &sdns_h_mv_16_array);
    sdns_dark_thres_intp = tisp_simple_intp($s2, $s0, &sdns_dark_thres_array);
    sdns_light_thres_intp = tisp_simple_intp($s2, $s0, &sdns_light_thres_array);
    sdns_sharpen_tt_opt_intp = tisp_simple_intp($s2, $s0, sdns_sharpen_tt_opt_array_now);
    sdns_sp_uu_thres_intp = tisp_simple_intp($s2, $s0, sdns_sp_uu_thres_array_now);
    sdns_sp_uu_stren_intp = tisp_simple_intp($s2, $s0, sdns_sp_uu_stren_array_now);
    sdns_sp_mv_uu_thres_intp = tisp_simple_intp($s2, $s0, sdns_sp_mv_uu_thres_array_now);
    sdns_sp_d_v2_win5_thres_intp = tisp_simple_intp($s2, $s0, &sdns_sp_d_v2_win5_thres_array);
    sdns_sp_mv_uu_stren_intp = tisp_simple_intp($s2, $s0, sdns_sp_mv_uu_stren_array_now);
    sdns_sp_d_w_sp_stren_0_intp = tisp_simple_intp($s2, $s0, &sdns_sp_d_w_sp_stren_0_array);
    sdns_sp_d_w_sp_stren_1_intp = tisp_simple_intp($s2, $s0, &sdns_sp_d_w_sp_stren_1_array);
    sdns_sp_d_w_sp_stren_2_intp = tisp_simple_intp($s2, $s0, &sdns_sp_d_w_sp_stren_2_array);
    sdns_sp_d_w_sp_stren_3_intp = tisp_simple_intp($s2, $s0, &sdns_sp_d_w_sp_stren_3_array);
    sdns_sp_d_b_sp_stren_0_intp = tisp_simple_intp($s2, $s0, &sdns_sp_d_b_sp_stren_0_array);
    sdns_sp_d_b_sp_stren_1_intp = tisp_simple_intp($s2, $s0, &sdns_sp_d_b_sp_stren_1_array);
    sdns_sp_d_b_sp_stren_2_intp = tisp_simple_intp($s2, $s0, &sdns_sp_d_b_sp_stren_2_array);
    sdns_sp_d_b_sp_stren_3_intp = tisp_simple_intp($s2, $s0, &sdns_sp_d_b_sp_stren_3_array);
    sdns_sp_d_flat_thres_intp = tisp_simple_intp($s2, $s0, &sdns_sp_d_flat_thres_array);
    sdns_sp_d_flat_stren_intp = tisp_simple_intp($s2, $s0, &sdns_sp_d_flat_stren_array);
    sdns_sp_ud_w_sp_stren_0_intp = tisp_simple_intp($s2, $s0, &sdns_sp_ud_w_sp_stren_0_array);
    sdns_sp_ud_w_sp_stren_1_intp = tisp_simple_intp($s2, $s0, &sdns_sp_ud_w_sp_stren_1_array);
    sdns_sp_ud_w_sp_stren_2_intp = tisp_simple_intp($s2, $s0, &sdns_sp_ud_w_sp_stren_2_array);
    sdns_sp_ud_w_sp_stren_3_intp = tisp_simple_intp($s2, $s0, &sdns_sp_ud_w_sp_stren_3_array);
    sdns_sp_ud_b_sp_stren_0_intp = tisp_simple_intp($s2, $s0, &sdns_sp_ud_b_sp_stren_0_array);
    sdns_sp_ud_b_sp_stren_1_intp = tisp_simple_intp($s2, $s0, &sdns_sp_ud_b_sp_stren_1_array);
    sdns_sp_ud_b_sp_stren_2_intp = tisp_simple_intp($s2, $s0, &sdns_sp_ud_b_sp_stren_2_array);
    sdns_sp_ud_b_sp_stren_3_intp = tisp_simple_intp($s2, $s0, &sdns_sp_ud_b_sp_stren_3_array);
    sdns_sp_ud_std_thres_intp = tisp_simple_intp($s2, $s0, &sdns_sp_ud_std_thres_array);
    sdns_sp_ud_std_stren_intp = tisp_simple_intp($s2, $s0, &sdns_sp_ud_std_stren_array);
    sdns_sp_ud_flat_thres_intp = tisp_simple_intp($s2, $s0, &sdns_sp_ud_flat_thres_array);
    sdns_ave_thres_intp = tisp_simple_intp($s2, $s0, sdns_ave_thres_array_now);
    sdns_sp_ud_flat_stren_intp = tisp_simple_intp($s2, $s0, &sdns_sp_ud_flat_stren_array);
    return 0;
}

