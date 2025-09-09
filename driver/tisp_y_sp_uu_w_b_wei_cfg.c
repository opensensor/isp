#include "include/main.h"


  int32_t tisp_y_sp_uu_w_b_wei_cfg()

{
    system_reg_write(0x7038, 
        data_c5944 << 8 | data_c5948 << 0x10 | y_sp_uu_np_array | data_c594c << 0x18);
    system_reg_write(0x703c, 
        data_c5954 << 8 | data_c5958 << 0x10 | data_c5950 | data_c595c << 0x18);
    system_reg_write(0x7040, 
        data_c5964 << 8 | data_c5968 << 0x10 | data_c5960 | data_c596c << 0x18);
    system_reg_write(0x7044, 
        data_c5974 << 8 | data_c5978 << 0x10 | data_c5970 | data_c597c << 0x18);
    system_reg_write(0x7048, 
        data_c5904 << 8 | data_c5908 << 0x10 | y_sp_w_wei_np_array | data_c590c << 0x18);
    system_reg_write(0x704c, 
        data_c5914 << 8 | data_c5918 << 0x10 | data_c5910 | data_c591c << 0x18);
    system_reg_write(0x7050, 
        data_c5924 << 8 | data_c5928 << 0x10 | data_c5920 | data_c592c << 0x18);
    system_reg_write(0x7054, 
        data_c5934 << 8 | data_c5938 << 0x10 | data_c5930 | data_c593c << 0x18);
    system_reg_write(0x7058, 
        data_c58c4 << 8 | data_c58c8 << 0x10 | y_sp_b_wei_np_array | data_c58cc << 0x18);
    system_reg_write(0x705c, 
        data_c58d4 << 8 | data_c58d8 << 0x10 | data_c58d0 | data_c58dc << 0x18);
    system_reg_write(0x7060, 
        data_c58e4 << 8 | data_c58e8 << 0x10 | data_c58e0 | data_c58ec << 0x18);
    system_reg_write(0x7064, 
        data_c58f4 << 8 | data_c58f8 << 0x10 | data_c58f0 | data_c58fc << 0x18);
    return 0;
}

