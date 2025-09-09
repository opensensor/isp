#include "include/main.h"


  int32_t tisp_sdns_w_thres_cfg()

{
    system_reg_write(0x88a0, data_c68c8 << 8 | sdns_w_thr);
    system_reg_write(0x88a4, data_c68d0 << 8 | data_c68cc);
    system_reg_write(0x88a8, data_c68d8 << 8 | data_c68d4);
    system_reg_write(0x88ac, data_c68e0 << 8 | data_c68dc);
    system_reg_write(0x88b0, data_c68e8 << 8 | data_c68e4);
    system_reg_write(0x88b4, data_c68f0 << 8 | data_c68ec);
    system_reg_write(0x88b8, data_c68f8 << 8 | data_c68f4);
    system_reg_write(0x88bc, data_c6900 << 8 | data_c68fc);
    return 0;
}

