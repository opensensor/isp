#include "include/main.h"


  int32_t tisp_dmsc_sp_ud_w_wei_np_cfg()

{
    system_reg_write(0x491c, 
        data_c4ef8 << 0x18 | data_c4ef4 << 0x12 | dmsc_sp_ud_w_wei_np_array | data_c4ef0 << 0xc
            | data_c4eec << 6);
    system_reg_write(0x4920, 
        data_c4f0c << 0x18 | data_c4f08 << 0x12 | data_c4efc | data_c4f04 << 0xc | data_c4f00 << 6);
    system_reg_write(0x4924, 
        data_c4f20 << 0x18 | data_c4f1c << 0x12 | data_c4f10 | data_c4f18 << 0xc | data_c4f14 << 6);
    system_reg_write(0x4928, 
        data_c4f34 << 0x18 | data_c4f30 << 0x12 | data_c4f24 | data_c4f2c << 0xc | data_c4f28 << 6);
    system_reg_write(0x492c, data_c4f3c << 6 | data_c4f38);
    return 0;
}

