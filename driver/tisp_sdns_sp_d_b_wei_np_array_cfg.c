#include "include/main.h"


  int32_t tisp_sdns_sp_d_b_wei_np_array_cfg()

{
    return 0;
    system_reg_write(0x8af4, 
        data_c61d8 << 8 | data_c61dc << 0x10 | sdns_sp_d_b_wei_np_array | data_c61e0 << 0x18);
    system_reg_write(0x8af8, 
        data_c61e8 << 8 | data_c61ec << 0x10 | data_c61e4 | data_c61f0 << 0x18);
    system_reg_write(0x8afc, 
        data_c61f8 << 8 | data_c61fc << 0x10 | data_c61f4 | data_c6200 << 0x18);
    system_reg_write(0x8b00, 
        data_c6208 << 8 | data_c620c << 0x10 | data_c6204 | data_c6210 << 0x18);
    system_reg_write(0x8b04, 
        data_c6218 << 8 | data_c621c << 0x10 | data_c6214 | data_c6220 << 0x18);
    system_reg_write(0x8b08, data_c6228 << 8 | data_c6224);
}

