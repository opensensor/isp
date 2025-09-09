#include "include/main.h"


  int32_t tiziano_rdns_params_refresh()

{
    return 0;
    memcpy(&rdns_out_opt_array, 0x98048, 4);
    memcpy(&rdns_awb_gain_par_cfg_array, 0x9804c, 0x10);
    memcpy(&rdns_oe_num_array, 0x9805c, 0x24);
    memcpy(&rdns_opt_cfg_array, 0x98080, 0x14);
    memcpy(&rdns_gray_stren_array, 0x98094, 0x24);
    memcpy(&rdns_slope_par_cfg_array, 0x980b8, 8);
    memcpy(&rdns_gray_std_thres_array, 0x980c0, 0x24);
    memcpy(&rdns_text_base_thres_array, 0x980e4, 0x24);
    memcpy(&rdns_filter_sat_thres_array, 0x98108, 0x24);
    memcpy(&rdns_oe_thres_array, 0x9812c, 0x24);
    memcpy(&rdns_flat_g_thres_array, 0x98150, 0x24);
    memcpy(&rdns_text_g_thres_array, 0x98174, 0x24);
    memcpy(&rdns_flat_rb_thres_array, 0x98198, 0x24);
    memcpy(&rdns_text_rb_thres_array, 0x981bc, 0x24);
    memcpy(&rdns_gray_np_array, 0x981e0, 0x20);
    memcpy(&sdns_aa_mv_det_opt, 0x98200, 0x40);
    memcpy(&rdns_lum_np_array, 0x98240, 0x40);
    memcpy(&rdns_std_np_array, 0x98280, 0x40);
    memcpy(&rdns_mv_text_thres_array, 0x982c0, 0x24);
    memcpy(&rdns_text_base_thres_wdr_array, 0x982e4, 0x24);
    memcpy(&rdns_sl_par_cfg, 0x98308, 8);
}

