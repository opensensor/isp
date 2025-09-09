#include "include/main.h"


  int32_t tisp_dmsc_sp_d_w_wei_np_cfg()

{
    system_reg_write(0x48f4, 
        data_c4fa8 << 0x18 | data_c4fa4 << 0x12 | dmsc_sp_d_w_wei_np_array | data_c4fa0 << 0xc
            | data_c4f9c << 6);
    system_reg_write(0x48f8, 
        data_c4fbc << 0x18 | data_c4fb8 << 0x12 | data_c4fac | data_c4fb4 << 0xc | data_c4fb0 << 6);
    system_reg_write(0x48fc, 
        data_c4fd0 << 0x18 | data_c4fcc << 0x12 | data_c4fc0 | data_c4fc8 << 0xc | data_c4fc4 << 6);
    system_reg_write(0x4900, 
        data_c4fe4 << 0x18 | data_c4fe0 << 0x12 | data_c4fd4 | data_c4fdc << 0xc | data_c4fd8 << 6);
    system_reg_write(0x4904, data_c4fec << 6 | data_c4fe8);
    return 0;
}

