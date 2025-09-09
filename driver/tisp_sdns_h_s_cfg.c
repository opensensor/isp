#include "include/main.h"


  int32_t tisp_sdns_h_s_cfg()

{
    system_reg_write(0x885c, 
        sdns_h_s_2_intp << 8 | sdns_h_s_3_intp << 0x10 | sdns_h_s_1_intp | sdns_h_s_4_intp << 0x18);
    system_reg_write(0x8860, 
        sdns_h_s_6_intp << 8 | sdns_h_s_7_intp << 0x10 | sdns_h_s_5_intp | sdns_h_s_8_intp << 0x18);
    system_reg_write(0x8864, 
        sdns_h_s_10_intp << 8 | sdns_h_s_11_intp << 0x10 | sdns_h_s_9_intp
            | sdns_h_s_12_intp << 0x18);
    system_reg_write(0x8868, 
        sdns_h_s_14_intp << 8 | sdns_h_s_15_intp << 0x10 | sdns_h_s_13_intp
            | sdns_h_s_16_intp << 0x18);
    return 0;
}

