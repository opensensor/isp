#include "include/main.h"


  int32_t tiziano_sharpen_params_refresh()

{
    memcpy(&y_sp_out_opt_array, 0x9e878, 4);
    memcpy(&y_sp_sl_exp_thres_array, 0x9e87c, 0x24);
    memcpy(&y_sp_sl_exp_num_array, 0x9e8a0, 0x24);
    memcpy(&y_sp_std_cfg_array, 0x9e8c4, 8);
    memcpy(&y_sp_uu_min_stren_array, 0x9e8cc, 0x24);
    memcpy(&y_sp_uu_min_thres_array, 0x9e8f0, 0x24);
    memcpy(&y_sp_uu_thres_array, 0x9e914, 0x24);
    memcpy(&y_sp_mv_uu_thres_array, &data_9e938, 0x24);
    memcpy(&y_sp_mv_uu_stren_array, U"?????????ddddddddd", 0x24);
    memcpy(&y_sp_uu_stren_array, &data_9e95c[9], 0x24);
    memcpy(&y_sp_uu_par_cfg_array, &data_9e95c[0x12], 0x10);
    memcpy(&y_sp_fl_std_thres_array, 0x9e9b4, 0x24);
    memcpy(&y_sp_mv_fl_std_thres_array, U"\n\n\n\n\n\n\n\n\n", 0x24);
    memcpy(&y_sp_fl_thres_array, &data_9e9fc, 0x24);
    memcpy(&y_sp_fl_min_thres_array, U"\n\n\n\n\n\n\n\n\n", 0x24);
    memcpy(&y_sp_mv_fl_thres_array, &data_9ea44, 0x24);
    memcpy(&y_sp_mv_fl_min_thres_array, U"\n\n\n\n\n\n\n\n\n?", 0x24);
    memcpy(&y_sp_fl_par_cfg_array, &data_9ea68[9], 8);
    memcpy(&y_sp_v2_win5_thres_array, 0x9ea94, 0x24);
    memcpy(&y_sp_v1_v2_coef_par_cfg_array, 0x9eab8, 0x30);
    memcpy(&y_sp_w_b_ll_par_cfg_array, 0x9eae8, 0x24);
    memcpy(&y_sp_uu_np_array, 0x9eb0c, 0x40);
    memcpy(&y_sp_w_wei_np_array, U"                        ", 0x40);
    memcpy(&y_sp_b_wei_np_array, &data_9eb4c[0x10], 0x40);
    memcpy(&y_sp_w_sl_stren_0_array, &data_9ebcc, 0x24);
    memcpy(&y_sp_w_sl_stren_1_array, &data_9ebf0, 0x24);
    memcpy(&y_sp_w_sl_stren_2_array, 0x9ec14, 0x24);
    memcpy(&y_sp_w_sl_stren_3_array, &data_9ec38, 0x24);
    memcpy(&y_sp_b_sl_stren_0_array, &data_9ec5c, 0x24);
    memcpy(&y_sp_b_sl_stren_1_array, &data_9ec80, 0x24);
    memcpy(&y_sp_b_sl_stren_2_array, &data_9eca4, 0x24);
    memcpy(&y_sp_b_sl_stren_3_array, &data_9ecc8, 0x24);
    memcpy(&awb_dn_refresh_flag, &data_9ecec, 0x24);
    memcpy(&DumpNum.32174, 0x9ed10, 0x24);
    memcpy(&y_sp_uu_sl_2_array, 0x9ed34, 0x24);
    memcpy(&y_sp_uu_sl_3_array, 0x9ed58, 0x24);
    memcpy(&IspAwbCtDetectParam, 0x9ed7c, 0x24);
    memcpy(&y_sp_fl_sl_1_array, 0x9eda0, 0x24);
    memcpy(&y_sp_fl_sl_2_array, 0x9edc4, 0x24);
    memcpy(&y_sp_uu_thres_wdr_array, 0x9ede8, 0x24);
    memcpy(&y_sp_w_sl_stren_0_wdr_array, 0x9ee0c, 0x24);
    memcpy(&y_sp_w_sl_stren_1_wdr_array, 0x9ee30, 0x24);
    memcpy(&y_sp_w_sl_stren_2_wdr_array, 0x9ee54, 0x24);
    memcpy(&y_sp_w_sl_stren_3_wdr_array, 0x9ee78, 0x24);
    memcpy(&y_sp_b_sl_stren_0_wdr_array, 0x9ee9c, 0x24);
    memcpy(&y_sp_b_sl_stren_1_wdr_array, 0x9eec0, 0x24);
    memcpy(&y_sp_b_sl_stren_2_wdr_array, 0x9eee4, 0x24);
    memcpy(&y_sp_b_sl_stren_3_wdr_array, 0x9ef08, 0x24);
    memcpy(&y_sp_fl_sl_3_array, 0x9ef2c, 0x24);
    return 0;
}

