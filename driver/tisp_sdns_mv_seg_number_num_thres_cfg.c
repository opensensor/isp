#include "include/main.h"


  int32_t tisp_sdns_mv_seg_number_num_thres_cfg()

{
    system_reg_write(0x8810, data_c6fe8_1 << 8 | data_c6fe4_1);
    system_reg_write(0x8814, 
        sdns_mv_num_thr_7x7_intp << 8 | sdns_mv_num_thr_9x9_intp << 0x10 | sdns_mv_num_thr_5x5_intp
            | sdns_mv_num_thr_11x11_intp << 0x18);
    return 0;
}

