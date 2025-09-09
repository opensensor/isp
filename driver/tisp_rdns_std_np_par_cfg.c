#include "include/main.h"


  int32_t tisp_rdns_std_np_par_cfg()

{
    system_reg_write(0x3078, data_d6f5c_1 << 0x10 | rdns_std_np_array);
    system_reg_write(0x307c, data_d6f64_1 << 0x10 | data_d6f60_1);
    system_reg_write(0x3080, data_d6f6c_1 << 0x10 | data_d6f68_1);
    system_reg_write(0x3084, data_d6f74_1 << 0x10 | data_d6f70_1);
    system_reg_write(0x3088, data_d6f7c_1 << 0x10 | data_d6f78_1);
    system_reg_write(0x308c, data_d6f84_1 << 0x10 | data_d6f80_1);
    system_reg_write(0x3090, data_d6f8c_1 << 0x10 | data_d6f88_1);
    system_reg_write(0x3094, data_d6f94_1 << 0x10 | data_d6f90_1);
    return 0;
}

