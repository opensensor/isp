#include "include/main.h"


  int32_t tisp_sdns_d_s1_thres_cfg()

{
    system_reg_write(0x8880, data_c6908 << 0x10 | sdns_d_s1_thr);
    system_reg_write(0x8884, data_c6910 << 0x10 | data_c690c);
    system_reg_write(0x8888, data_c6918 << 0x10 | data_c6914);
    system_reg_write(0x888c, data_c6920 << 0x10 | data_c691c);
    system_reg_write(0x8890, data_c6928 << 0x10 | data_c6924);
    system_reg_write(0x8894, data_c6930 << 0x10 | data_c692c);
    system_reg_write(0x8898, data_c6938 << 0x10 | data_c6934);
    system_reg_write(0x889c, data_c693c);
    return 0;
}

