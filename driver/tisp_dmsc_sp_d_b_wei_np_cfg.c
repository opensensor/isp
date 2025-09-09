#include "include/main.h"


  int32_t tisp_dmsc_sp_d_b_wei_np_cfg()

{
    system_reg_write(0x4908, 
        data_c4f50 << 0x18 | data_c4f4c << 0x12 | dmsc_sp_d_b_wei_np_array | data_c4f48 << 0xc
            | data_c4f44 << 6);
    system_reg_write(0x490c, 
        data_c4f64 << 0x18 | data_c4f60 << 0x12 | data_c4f54 | data_c4f5c << 0xc | data_c4f58 << 6);
    system_reg_write(0x4910, 
        data_c4f78 << 0x18 | data_c4f74 << 0x12 | data_c4f68 | data_c4f70 << 0xc | data_c4f6c << 6);
    system_reg_write(0x4914, 
        data_c4f8c << 0x18 | data_c4f88 << 0x12 | data_c4f7c | data_c4f84 << 0xc | data_c4f80 << 6);
    system_reg_write(0x4918, data_c4f94 << 6 | data_c4f90);
    return 0;
}

