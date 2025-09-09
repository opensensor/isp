#include "include/main.h"


  int32_t tisp_sdns_sp_ud_b_wei_np_array_cfg()

{
    system_reg_write(0x8b24, 
        data_c6104_1 << 8 | data_c6108_1 << 0x10 | sdns_sp_ud_b_wei_np_array | data_c610c_1 << 0x18);
    system_reg_write(0x8b28, 
        data_c6114_1 << 8 | data_c6118_1 << 0x10 | data_c6110_1 | data_c611c_1 << 0x18);
    system_reg_write(0x8b2c, 
        data_c6124_1 << 8 | data_c6128_1 << 0x10 | data_c6120_1 | data_c612c_1 << 0x18);
    system_reg_write(0x8b30, 
        data_c6134_1 << 8 | data_c6138_1 << 0x10 | data_c6130_1 | data_c613c_1 << 0x18);
    system_reg_write(0x8b34, 
        data_c6144_1 << 8 | data_c6148_1 << 0x10 | data_c6140_1 | data_c614c_1 << 0x18);
    system_reg_write(0x8b38, data_c6154_1 << 8 | data_c6150_1);
    return 0;
}

