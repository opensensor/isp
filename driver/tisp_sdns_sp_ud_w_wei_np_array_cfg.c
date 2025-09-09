#include "include/main.h"


  int32_t tisp_sdns_sp_ud_w_wei_np_array_cfg()

{
    system_reg_write(0x8b0c, 
        data_c6180_1 << 8 | data_c6184_1 << 0x10 | sdns_sp_ud_w_wei_np_array | data_c6188_1 << 0x18);
    system_reg_write(0x8b10, 
        data_c6190_1 << 8 | data_c6194_1 << 0x10 | data_c618c_1 | data_c6198_1 << 0x18);
    system_reg_write(0x8b14, 
        data_c61a0_1 << 8 | data_c61a4_1 << 0x10 | data_c619c_1 | data_c61a8_1 << 0x18);
    system_reg_write(0x8b18, 
        data_c61b0_1 << 8 | data_c61b4_1 << 0x10 | data_c61ac_1 | data_c61b8_1 << 0x18);
    system_reg_write(0x8b1c, 
        data_c61c0_1 << 8 | data_c61c4_1 << 0x10 | data_c61bc_1 | data_c61c8_1 << 0x18);
    system_reg_write(0x8b20, data_c61d0_1 << 8 | data_c61cc_1);
    return 0;
}

