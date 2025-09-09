#include "include/main.h"


  int32_t tiziano_ydns_params_refresh()

{
    memcpy(&sdns_mv_num_thr_7x7_array, 0xa7120, 4);
    memcpy(&sdns_mv_num_thr_9x9_array, 0xa7124, 0x24);
    memcpy(&sdns_mv_num_thr_11x11_array, 0xa7148, 0x24);
    memcpy(&ydns_mv_thres2_array, 0xa716c, 0x24);
    memcpy(&ydns_fus_level_array, 0xa7190, 0x24);
    memcpy(&ydns_fus_min_thres_array, U"dddddddddddddddddd", 0x24);
    memcpy(&ydns_fus_max_thres_array, &data_a71b4_1[9], 0x24);
    memcpy(&ydns_fus_sswei_array, &data_a71b4_2[0x12], 0x24);
    memcpy(&ydns_fus_sewei_array, 0xa7220, 0x24);
    memcpy(&ydns_fus_mswei_array, 0xa7244, 0x24);
    memcpy(&ydns_fus_mewei_array, 0xa7268, 0x24);
    memcpy(&ydns_fus_uvwei_array, 0xa728c, 0x24);
    memcpy(&ydns_edge_wei_array, 0xa72b0, 0x24);
    memcpy(&ydns_edge_div_array, 0xa72d4, 0x24);
    memcpy(&ydns_edge_thres_array, 0xa72f8, 0x24);
    return 0;
}

