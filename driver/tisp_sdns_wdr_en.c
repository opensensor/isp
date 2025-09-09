#include "include/main.h"


  int32_t tisp_sdns_wdr_en(uint32_t arg1)

{
    char* var_30 = (char*)(&data_b0000); // Fixed void pointer assignment
    void* $v0_1;
    sdns_wdr_en = arg1;
    
    if (arg1)
    {
        sdns_h_mv_wei_now = &sdns_h_mv_wei_wdr;
        sdns_std_thr2_array_now = &sdns_std_thr2_wdr_array;
        sdns_grad_zx_thres_array_now = &sdns_grad_zx_thres_wdr_array;
        sdns_grad_zy_thres_array_now = &sdns_grad_zy_thres_wdr_array;
        sdns_std_thr1_array_now = &sdns_std_thr1_wdr_array;
        sdns_h_s_1_array_now = &sdns_h_s_1_wdr_array;
        sdns_h_s_2_array_now = &sdns_h_s_2_wdr_array;
        sdns_h_s_3_array_now = &sdns_h_s_3_wdr_array;
        sdns_h_s_4_array_now = &sdns_h_s_4_wdr_array;
        sdns_h_s_5_array_now = &sdns_h_s_5_wdr_array;
        sdns_h_s_6_array_now = &sdns_h_s_6_wdr_array;
        sdns_h_s_7_array_now = &sdns_h_s_7_wdr_array;
        sdns_h_s_8_array_now = &sdns_h_s_8_wdr_array;
        sdns_h_s_9_array_now = &sdns_h_s_9_wdr_array;
        sdns_h_s_10_array_now = &sdns_h_s_10_wdr_array;
        sdns_h_s_11_array_now = &sdns_h_s_11_wdr_array;
        sdns_h_s_12_array_now = &sdns_h_s_12_wdr_array;
        sdns_h_s_13_array_now = &sdns_h_s_13_wdr_array;
        sdns_h_s_14_array_now = &sdns_h_s_14_wdr_array;
        sdns_h_s_15_array_now = &sdns_h_s_15_wdr_array;
        sdns_h_s_16_array_now = &sdns_h_s_16_wdr_array;
        sdns_sharpen_tt_opt_array_now = &sdns_sharpen_tt_opt_wdr_array;
        sdns_ave_fliter_now = &sdns_ave_fliter_wdr;
        sdns_sp_uu_thres_array_now = &sdns_sp_uu_thres_wdr_array;
        sdns_sp_uu_stren_array_now = &sdns_sp_uu_stren_wdr_array;
        sdns_sp_mv_uu_thres_array_now = &sdns_sp_mv_uu_thres_wdr_array;
        sdns_sp_mv_uu_stren_array_now = &sdns_sp_mv_uu_stren_wdr_array;
        $v0_1 = &sdns_ave_thres_wdr_array;
    }
    else
    {
        sdns_h_mv_wei_now = &sdns_h_mv_wei;
        sdns_std_thr2_array_now = &sdns_std_thr2_array;
        sdns_grad_zx_thres_array_now = &sdns_grad_zx_thres_array;
        sdns_grad_zy_thres_array_now = &sdns_grad_zy_thres_array;
        sdns_std_thr1_array_now = &sdns_std_thr1_array;
        sdns_h_s_1_array_now = &sdns_h_s_1_array;
        sdns_h_s_2_array_now = &sdns_h_s_2_array;
        sdns_h_s_3_array_now = &sdns_h_s_3_array;
        sdns_h_s_4_array_now = &sdns_h_s_4_array;
        sdns_h_s_5_array_now = &sdns_h_s_5_array;
        sdns_h_s_6_array_now = &sdns_h_s_6_array;
        sdns_h_s_7_array_now = &sdns_h_s_7_array;
        sdns_h_s_8_array_now = &sdns_h_s_8_array;
        sdns_h_s_9_array_now = &sdns_h_s_9_array;
        sdns_h_s_10_array_now = &sdns_h_s_10_array;
        sdns_h_s_11_array_now = &sdns_h_s_11_array;
        sdns_h_s_12_array_now = &sdns_h_s_12_array;
        sdns_h_s_13_array_now = &sdns_h_s_13_array;
        sdns_h_s_14_array_now = &sdns_h_s_14_array;
        sdns_h_s_15_array_now = &sdns_h_s_15_array;
        sdns_h_s_16_array_now = &sdns_h_s_16_array;
        sdns_sharpen_tt_opt_array_now = &sdns_sharpen_tt_opt_array;
        sdns_ave_fliter_now = &sdns_ave_fliter;
        sdns_sp_uu_thres_array_now = &sdns_sp_uu_thres_array;
        sdns_sp_uu_stren_array_now = &sdns_sp_uu_stren_array;
        sdns_sp_mv_uu_thres_array_now = &sdns_sp_mv_uu_thres_array;
        sdns_sp_mv_uu_stren_array_now = &sdns_sp_mv_uu_stren_array;
        $v0_1 = &rgbg_dis;
    }
    
    sdns_ave_thres_array_now = $v0_1;
    /* tailcall */
    return tisp_s_sdns_ratio(data_9a9c0_1);
}

