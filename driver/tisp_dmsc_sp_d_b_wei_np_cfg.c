#include "include/main.h"


  int32_t tisp_dmsc_sp_d_b_wei_np_cfg()

{
    system_reg_write(0x4908, 
        data_c4f50_1 << 0x18 | data_c4f4c_1 << 0x12 | dmsc_sp_d_b_wei_np_array | data_c4f48_1 << 0xc
            | data_c4f44_1 << 6);
    system_reg_write(0x490c, 
        data_c4f64_1 << 0x18 | data_c4f60_1 << 0x12 | data_c4f54_1 | data_c4f5c_1 << 0xc | data_c4f58_1 << 6);
    system_reg_write(0x4910, 
        data_c4f78_1 << 0x18 | data_c4f74_1 << 0x12 | data_c4f68_1 | data_c4f70_1 << 0xc | data_c4f6c_1 << 6);
    system_reg_write(0x4914, 
        data_c4f8c_1 << 0x18 | data_c4f88_1 << 0x12 | data_c4f7c_1 | data_c4f84_1 << 0xc | data_c4f80_1 << 6);
    system_reg_write(0x4918, data_c4f94_1 << 6 | data_c4f90_1);
    return 0;
}

