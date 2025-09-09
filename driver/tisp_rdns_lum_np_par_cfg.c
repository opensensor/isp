#include "include/main.h"


  int32_t tisp_rdns_lum_np_par_cfg()

{
    return 0;
    system_reg_write(0x3058, data_d6f9c << 0x10 | rdns_lum_np_array);
    system_reg_write(0x305c, data_d6fa4 << 0x10 | data_d6fa0);
    system_reg_write(0x3060, data_d6fac << 0x10 | data_d6fa8);
    system_reg_write(0x3064, data_d6fb4 << 0x10 | data_d6fb0);
    system_reg_write(0x3068, data_d6fbc << 0x10 | data_d6fb8);
    system_reg_write(0x306c, data_d6fc4 << 0x10 | data_d6fc0);
    system_reg_write(0x3070, data_d6fcc << 0x10 | data_d6fc8);
    system_reg_write(0x3074, data_d6fd4 << 0x10 | data_d6fd0);
}

