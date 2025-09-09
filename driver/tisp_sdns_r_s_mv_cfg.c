#include "include/main.h"


  int32_t tisp_sdns_r_s_mv_cfg()

{
    system_reg_write(0x881c, data_c6e70_1 << 0x10 | sdns_r_s);
    system_reg_write(0x8820, data_c6e78_1 << 0x10 | data_c6e74_1);
    system_reg_write(0x8824, data_c6e80_1 << 0x10 | data_c6e7c_1);
    system_reg_write(0x8828, data_c6e88_1 << 0x10 | data_c6e84_1);
    system_reg_write(0x882c, data_c6e90_1 << 0x10 | data_c6e8c_1);
    system_reg_write(0x8830, data_c6e98_1 << 0x10 | data_c6e94_1);
    system_reg_write(0x8834, data_c6ea0_1 << 0x10 | data_c6e9c_1);
    system_reg_write(0x8838, data_c6ea4_1);
    system_reg_write(0x883c, data_c6e34_1 << 0x10 | sdns_r_mv);
    system_reg_write(0x8840, data_c6e3c_1 << 0x10 | data_c6e38_1);
    system_reg_write(0x8844, data_c6e44_1 << 0x10 | data_c6e40_1);
    system_reg_write(0x8848, data_c6e4c_1 << 0x10 | data_c6e48_1);
    system_reg_write(0x884c, data_c6e54_1 << 0x10 | data_c6e50_1);
    system_reg_write(0x8850, data_c6e5c_1 << 0x10 | data_c6e58_1);
    system_reg_write(0x8854, data_c6e64_1 << 0x10 | data_c6e60_1);
    system_reg_write(0x8858, data_c6e68_1);
    return 0;
}

