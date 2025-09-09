#include "include/main.h"


  int32_t tisp_dmsc_sp_ud_w_wei_np_cfg()

{
    system_reg_write(0x491c, 
        data_c4ef8_1 << 0x18 | data_c4ef4_1 << 0x12 | dmsc_sp_ud_w_wei_np_array | data_c4ef0_1 << 0xc
            | data_c4eec_1 << 6);
    system_reg_write(0x4920, 
        data_c4f0c_1 << 0x18 | data_c4f08_1 << 0x12 | data_c4efc_1 | data_c4f04_1 << 0xc | data_c4f00_1 << 6);
    system_reg_write(0x4924, 
        data_c4f20_1 << 0x18 | data_c4f1c_1 << 0x12 | data_c4f10_1 | data_c4f18_1 << 0xc | data_c4f14_1 << 6);
    system_reg_write(0x4928, 
        data_c4f34_1 << 0x18 | data_c4f30_1 << 0x12 | data_c4f24_1 | data_c4f2c_1 << 0xc | data_c4f28_1 << 6);
    system_reg_write(0x492c, data_c4f3c_1 << 6 | data_c4f38_1);
    return 0;
}

