#include "include/main.h"


  int32_t tisp_sdns_sp_ud_b_wei_np_array_cfg()

{
    system_reg_write(0x8b24, 
        data_c6104 << 8 | data_c6108 << 0x10 | sdns_sp_ud_b_wei_np_array | data_c610c << 0x18);
    system_reg_write(0x8b28, 
        data_c6114 << 8 | data_c6118 << 0x10 | data_c6110 | data_c611c << 0x18);
    system_reg_write(0x8b2c, 
        data_c6124 << 8 | data_c6128 << 0x10 | data_c6120 | data_c612c << 0x18);
    system_reg_write(0x8b30, 
        data_c6134 << 8 | data_c6138 << 0x10 | data_c6130 | data_c613c << 0x18);
    system_reg_write(0x8b34, 
        data_c6144 << 8 | data_c6148 << 0x10 | data_c6140 | data_c614c << 0x18);
    system_reg_write(0x8b38, data_c6154 << 8 | data_c6150);
    return 0;
}

