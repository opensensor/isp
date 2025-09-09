#include "include/main.h"


  int32_t tisp_rdns_std_np_par_cfg()

{
    system_reg_write(0x3078, data_d6f5c << 0x10 | rdns_std_np_array);
    system_reg_write(0x307c, data_d6f64 << 0x10 | data_d6f60);
    system_reg_write(0x3080, data_d6f6c << 0x10 | data_d6f68);
    system_reg_write(0x3084, data_d6f74 << 0x10 | data_d6f70);
    system_reg_write(0x3088, data_d6f7c << 0x10 | data_d6f78);
    system_reg_write(0x308c, data_d6f84 << 0x10 | data_d6f80);
    system_reg_write(0x3090, data_d6f8c << 0x10 | data_d6f88);
    system_reg_write(0x3094, data_d6f94 << 0x10 | data_d6f90);
    return 0;
}

