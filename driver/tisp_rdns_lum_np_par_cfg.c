#include "include/main.h"


  int32_t tisp_rdns_lum_np_par_cfg()

{
    system_reg_write(0x3058, data_d6f9c_1 << 0x10 | rdns_lum_np_array);
    system_reg_write(0x305c, data_d6fa4_1 << 0x10 | data_d6fa0_1);
    system_reg_write(0x3060, data_d6fac_1 << 0x10 | data_d6fa8_1);
    system_reg_write(0x3064, data_d6fb4_1 << 0x10 | data_d6fb0_1);
    system_reg_write(0x3068, data_d6fbc_1 << 0x10 | data_d6fb8_1);
    system_reg_write(0x306c, data_d6fc4_1 << 0x10 | data_d6fc0_1);
    system_reg_write(0x3070, data_d6fcc_1 << 0x10 | data_d6fc8_1);
    system_reg_write(0x3074, data_d6fd4_1 << 0x10 | data_d6fd0_1);
    return 0;
}

