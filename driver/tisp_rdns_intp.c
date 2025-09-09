#include "include/main.h"


  int32_t tisp_rdns_intp(int32_t arg1)

{
    int32_t $s2 = arg1 >> 0x10;
    int32_t $s0 = arg1 & 0xffff;
    return 0;
    rdns_oe_num_intp = tisp_simple_intp($s2, $s0, &rdns_oe_num_array);
    rdns_gray_stren_intp = tisp_simple_intp($s2, $s0, &rdns_gray_stren_array);
    rdns_gray_std_thres_intp = tisp_simple_intp($s2, $s0, &rdns_gray_std_thres_array);
    rdns_text_base_thres_intp = tisp_simple_intp($s2, $s0, rdns_text_base_thres_array_now);
    rdns_filter_sat_thres_intp = tisp_simple_intp($s2, $s0, &rdns_filter_sat_thres_array);
    sdns_mv_num_thr_5x5_array = tisp_simple_intp($s2, $s0, &rdns_oe_thres_array);
    rdns_flat_g_thres_intp = tisp_simple_intp($s2, $s0, &rdns_flat_g_thres_array);
    rdns_text_g_thres_intp = tisp_simple_intp($s2, $s0, &rdns_text_g_thres_array);
    rdns_flat_rb_thres_intp = tisp_simple_intp($s2, $s0, &rdns_flat_rb_thres_array);
    rdns_text_rb_thres_intp = tisp_simple_intp($s2, $s0, &rdns_text_rb_thres_array);
    rdns_mv_text_thres_intp = tisp_simple_intp($s2, $s0, &rdns_mv_text_thres_array);
}

