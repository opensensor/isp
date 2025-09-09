#include "include/main.h"


  int32_t tisp_sdns_sp_d_b_wei_np_array_cfg()

{
    system_reg_write(0x8af4, 
        data_c61d8_1 << 8 | data_c61dc_1 << 0x10 | sdns_sp_d_b_wei_np_array | data_c61e0_1 << 0x18);
    system_reg_write(0x8af8, 
        data_c61e8_1 << 8 | data_c61ec_1 << 0x10 | data_c61e4_1 | data_c61f0_1 << 0x18);
    system_reg_write(0x8afc, 
        data_c61f8_1 << 8 | data_c61fc_1 << 0x10 | data_c61f4_1 | data_c6200_1 << 0x18);
    system_reg_write(0x8b00, 
        data_c6208_1 << 8 | data_c620c_1 << 0x10 | data_c6204_1 | data_c6210_1 << 0x18);
    system_reg_write(0x8b04, 
        data_c6218_1 << 8 | data_c621c_1 << 0x10 | data_c6214_1 | data_c6220_1 << 0x18);
    system_reg_write(0x8b08, data_c6228_1 << 8 | data_c6224_1);
    return 0;
}

