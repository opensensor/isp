#include "include/main.h"


  int32_t tisp_y_sp_uu_w_b_wei_cfg()

{
    system_reg_write(0x7038, 
        data_c5944_1 << 8 | data_c5948_1 << 0x10 | y_sp_uu_np_array | data_c594c_1 << 0x18);
    system_reg_write(0x703c, 
        data_c5954_1 << 8 | data_c5958_1 << 0x10 | data_c5950_1 | data_c595c_1 << 0x18);
    system_reg_write(0x7040, 
        data_c5964_1 << 8 | data_c5968_1 << 0x10 | data_c5960_1 | data_c596c_1 << 0x18);
    system_reg_write(0x7044, 
        data_c5974_1 << 8 | data_c5978_1 << 0x10 | data_c5970_1 | data_c597c_1 << 0x18);
    system_reg_write(0x7048, 
        data_c5904_1 << 8 | data_c5908_1 << 0x10 | y_sp_w_wei_np_array | data_c590c_1 << 0x18);
    system_reg_write(0x704c, 
        data_c5914_1 << 8 | data_c5918_1 << 0x10 | data_c5910_1 | data_c591c_1 << 0x18);
    system_reg_write(0x7050, 
        data_c5924_1 << 8 | data_c5928_1 << 0x10 | data_c5920_1 | data_c592c_1 << 0x18);
    system_reg_write(0x7054, 
        data_c5934_1 << 8 | data_c5938_1 << 0x10 | data_c5930_1 | data_c593c_1 << 0x18);
    system_reg_write(0x7058, 
        data_c58c4_1 << 8 | data_c58c8_1 << 0x10 | y_sp_b_wei_np_array | data_c58cc_1 << 0x18);
    system_reg_write(0x705c, 
        data_c58d4_1 << 8 | data_c58d8_1 << 0x10 | data_c58d0_1 | data_c58dc_1 << 0x18);
    system_reg_write(0x7060, 
        data_c58e4_1 << 8 | data_c58e8_1 << 0x10 | data_c58e0_1 | data_c58ec_1 << 0x18);
    system_reg_write(0x7064, 
        data_c58f4_1 << 8 | data_c58f8_1 << 0x10 | data_c58f0_1 | data_c58fc_1 << 0x18);
    return 0;
}

