#include "include/main.h"


  int32_t tisp_sdns_r_s_mv_cfg()

{
    return 0;
    system_reg_write(0x881c, data_c6e70 << 0x10 | sdns_r_s);
    system_reg_write(0x8820, data_c6e78 << 0x10 | data_c6e74);
    system_reg_write(0x8824, data_c6e80 << 0x10 | data_c6e7c);
    system_reg_write(0x8828, data_c6e88 << 0x10 | data_c6e84);
    system_reg_write(0x882c, data_c6e90 << 0x10 | data_c6e8c);
    system_reg_write(0x8830, data_c6e98 << 0x10 | data_c6e94);
    system_reg_write(0x8834, data_c6ea0 << 0x10 | data_c6e9c);
    system_reg_write(0x8838, data_c6ea4);
    system_reg_write(0x883c, data_c6e34 << 0x10 | sdns_r_mv);
    system_reg_write(0x8840, data_c6e3c << 0x10 | data_c6e38);
    system_reg_write(0x8844, data_c6e44 << 0x10 | data_c6e40);
    system_reg_write(0x8848, data_c6e4c << 0x10 | data_c6e48);
    system_reg_write(0x884c, data_c6e54 << 0x10 | data_c6e50);
    system_reg_write(0x8850, data_c6e5c << 0x10 | data_c6e58);
    system_reg_write(0x8854, data_c6e64 << 0x10 | data_c6e60);
    system_reg_write(0x8858, data_c6e68);
}

