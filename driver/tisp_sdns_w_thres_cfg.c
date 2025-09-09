#include "include/main.h"


  int32_t tisp_sdns_w_thres_cfg()

{
    system_reg_write(0x88a0, data_c68c8_1 << 8 | sdns_w_thr);
    system_reg_write(0x88a4, data_c68d0_1 << 8 | data_c68cc_1);
    system_reg_write(0x88a8, data_c68d8_1 << 8 | data_c68d4_1);
    system_reg_write(0x88ac, data_c68e0_1 << 8 | data_c68dc_1);
    system_reg_write(0x88b0, data_c68e8_1 << 8 | data_c68e4_1);
    system_reg_write(0x88b4, data_c68f0_1 << 8 | data_c68ec_1);
    system_reg_write(0x88b8, data_c68f8_1 << 8 | data_c68f4_1);
    system_reg_write(0x88bc, data_c6900_1 << 8 | data_c68fc_1);
    return 0;
}

