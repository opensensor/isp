#include "include/main.h"


  int32_t tisp_ydns_param_cfg()

{
    system_reg_write(0x7af0, 
        ydns_mv_thres1_intp << 8 | sdns_h_s_9_array << 0x10 | ydns_mv_thres0_intp);
    system_reg_write(0x7af4, 
        ydns_fus_max_thres_intp << 8 | ydns_fus_level_intp << 0x10 | ydns_fus_min_thres_intp);
    system_reg_write(0x7af8, 
        ydns_fus_sewei_intp << 4 | ydns_fus_mswei_intp << 8 | ydns_fus_sswei_intp
            | ydns_fus_mewei_intp << 0xc | ydns_fus_uvwei_intp << 0x10);
    system_reg_write(0x7afc, 
        ydns_edge_div_intp << 4 | ydns_edge_thres_intp << 8 | sdns_h_s_10_array
            | sdns_mv_num_thr_7x7_array << 0x10);
    return 0;
}

