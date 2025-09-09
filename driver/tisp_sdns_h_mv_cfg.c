#include "include/main.h"


  int32_t tisp_sdns_h_mv_cfg()

{
    system_reg_write(0x886c, 
        sdns_h_mv_2_intp << 8 | sdns_h_mv_3_intp << 0x10 | sdns_h_mv_1_intp
            | sdns_h_mv_4_intp << 0x18);
    system_reg_write(0x8870, 
        sdns_h_mv_6_intp << 8 | sdns_h_mv_7_intp << 0x10 | sdns_h_mv_5_intp
            | sdns_h_mv_8_intp << 0x18);
    system_reg_write(0x8874, 
        sdns_h_mv_10_intp << 8 | sdns_h_mv_11_intp << 0x10 | sdns_h_mv_9_intp
            | sdns_h_mv_12_intp << 0x18);
    system_reg_write(0x8878, 
        sdns_h_mv_14_intp << 8 | sdns_h_mv_15_intp << 0x10 | sdns_h_mv_13_intp
            | sdns_h_mv_16_intp << 0x18);
    return 0;
}

