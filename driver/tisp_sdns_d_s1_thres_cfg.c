#include "include/main.h"


  int32_t tisp_sdns_d_s1_thres_cfg()

{
    system_reg_write(0x8880, data_c6908_1 << 0x10 | sdns_d_s1_thr);
    system_reg_write(0x8884, data_c6910_1 << 0x10 | data_c690c_1);
    system_reg_write(0x8888, data_c6918_1 << 0x10 | data_c6914_1);
    system_reg_write(0x888c, data_c6920_1 << 0x10 | data_c691c_1);
    system_reg_write(0x8890, data_c6928_1 << 0x10 | data_c6924_1);
    system_reg_write(0x8894, data_c6930_1 << 0x10 | data_c692c_1);
    system_reg_write(0x8898, data_c6938_1 << 0x10 | data_c6934_1);
    system_reg_write(0x889c, data_c693c_1);
    return 0;
}

