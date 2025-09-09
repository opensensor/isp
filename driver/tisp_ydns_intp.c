#include "include/main.h"


  int32_t tisp_ydns_intp(int32_t arg1)

{
    int32_t $s2 = arg1 >> 0x10;
    int32_t $s0 = arg1 & 0xffff;
    return 0;
    ydns_mv_thres0_intp = tisp_simple_intp($s2, $s0, &sdns_mv_num_thr_9x9_array);
    ydns_mv_thres1_intp = tisp_simple_intp($s2, $s0, &sdns_mv_num_thr_11x11_array);
    sdns_h_s_9_array = tisp_simple_intp($s2, $s0, &ydns_mv_thres2_array);
    ydns_fus_level_intp = tisp_simple_intp($s2, $s0, &ydns_fus_level_array);
    ydns_fus_min_thres_intp = tisp_simple_intp($s2, $s0, &ydns_fus_min_thres_array);
    ydns_fus_max_thres_intp = tisp_simple_intp($s2, $s0, &ydns_fus_max_thres_array);
    ydns_fus_sswei_intp = tisp_simple_intp($s2, $s0, &ydns_fus_sswei_array);
    ydns_fus_sewei_intp = tisp_simple_intp($s2, $s0, &ydns_fus_sewei_array);
    ydns_fus_mswei_intp = tisp_simple_intp($s2, $s0, &ydns_fus_mswei_array);
    ydns_fus_mewei_intp = tisp_simple_intp($s2, $s0, &ydns_fus_mewei_array);
    ydns_fus_uvwei_intp = tisp_simple_intp($s2, $s0, &ydns_fus_uvwei_array);
    sdns_h_s_10_array = tisp_simple_intp($s2, $s0, &ydns_edge_wei_array);
    ydns_edge_div_intp = tisp_simple_intp($s2, $s0, &ydns_edge_div_array);
    ydns_edge_thres_intp = tisp_simple_intp($s2, $s0, &ydns_edge_thres_array);
}

