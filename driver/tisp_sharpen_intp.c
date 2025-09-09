#include "include/main.h"


  int32_t tisp_sharpen_intp(int32_t arg1)

{
    int32_t $s2 = arg1 >> 0x10;
    int32_t $s0 = arg1 & 0xffff;
    y_sp_sl_exp_thres_intp = tisp_simple_intp($s2, $s0, &y_sp_sl_exp_thres_array);
    y_sp_sl_exp_num_intp = tisp_simple_intp($s2, $s0, &y_sp_sl_exp_num_array);
    y_sp_uu_min_stren_intp = tisp_simple_intp($s2, $s0, &y_sp_uu_min_stren_array);
    y_sp_uu_min_thres_intp = tisp_simple_intp($s2, $s0, &y_sp_uu_min_thres_array);
    y_sp_uu_thres_intp = tisp_simple_intp($s2, $s0, y_sp_uu_thres_array_now);
    y_sp_mv_uu_thres_intp = tisp_simple_intp($s2, $s0, &y_sp_mv_uu_thres_array);
    y_sp_mv_uu_stren_intp = tisp_simple_intp($s2, $s0, &y_sp_mv_uu_stren_array);
    y_sp_uu_stren_intp = tisp_simple_intp($s2, $s0, &y_sp_uu_stren_array);
    y_sp_fl_std_thres_intp = tisp_simple_intp($s2, $s0, &y_sp_fl_std_thres_array);
    y_sp_mv_fl_std_thres_intp = tisp_simple_intp($s2, $s0, &y_sp_mv_fl_std_thres_array);
    y_sp_fl_thres_intp = tisp_simple_intp($s2, $s0, &y_sp_fl_thres_array);
    y_sp_fl_min_thres_intp = tisp_simple_intp($s2, $s0, &y_sp_fl_min_thres_array);
    y_sp_mv_fl_thres_intp = tisp_simple_intp($s2, $s0, &y_sp_mv_fl_thres_array);
    y_sp_mv_fl_min_thres_intp = tisp_simple_intp($s2, $s0, &y_sp_mv_fl_min_thres_array);
    y_sp_v2_win5_thres_intp = tisp_simple_intp($s2, $s0, &y_sp_v2_win5_thres_array);
    y_sp_w_sl_stren_0_intp = tisp_simple_intp($s2, $s0, y_sp_w_sl_stren_0_array_now);
    y_sp_w_sl_stren_1_intp = tisp_simple_intp($s2, $s0, y_sp_w_sl_stren_1_array_now);
    y_sp_w_sl_stren_2_intp = tisp_simple_intp($s2, $s0, y_sp_w_sl_stren_2_array_now);
    y_sp_w_sl_stren_3_intp = tisp_simple_intp($s2, $s0, y_sp_w_sl_stren_3_array_now);
    y_sp_b_sl_stren_0_intp = tisp_simple_intp($s2, $s0, y_sp_b_sl_stren_0_array_now);
    y_sp_b_sl_stren_1_intp = tisp_simple_intp($s2, $s0, y_sp_b_sl_stren_1_array_now);
    y_sp_b_sl_stren_2_intp = tisp_simple_intp($s2, $s0, y_sp_b_sl_stren_2_array_now);
    y_sp_b_sl_stren_3_intp = tisp_simple_intp($s2, $s0, y_sp_b_sl_stren_3_array_now);
    y_sp_uu_sl_0_array_intp = tisp_simple_intp($s2, $s0, &awb_dn_refresh_flag);
    y_sp_uu_sl_1_array_intp = tisp_simple_intp($s2, $s0, &DumpNum.32174);
    y_sp_uu_sl_2_array_intp = tisp_simple_intp($s2, $s0, &y_sp_uu_sl_2_array);
    y_sp_uu_sl_3_array_intp = tisp_simple_intp($s2, $s0, &y_sp_uu_sl_3_array);
    y_sp_fl_sl_0_array_intp = tisp_simple_intp($s2, $s0, &IspAwbCtDetectParam);
    y_sp_fl_sl_1_array_intp = tisp_simple_intp($s2, $s0, &y_sp_fl_sl_1_array);
    y_sp_fl_sl_2_array_intp = tisp_simple_intp($s2, $s0, &y_sp_fl_sl_2_array);
    y_sp_fl_sl_3_array_intp = tisp_simple_intp($s2, $s0, &y_sp_fl_sl_3_array);
    return 0;
}

