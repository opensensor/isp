#include "include/main.h"


  int32_t tisp_dmsc_sp_d_w_wei_np_cfg()

{
    system_reg_write(0x48f4, 
        data_c4fa8_1 << 0x18 | data_c4fa4_1 << 0x12 | dmsc_sp_d_w_wei_np_array | data_c4fa0_1 << 0xc
            | data_c4f9c_1 << 6);
    system_reg_write(0x48f8, 
        data_c4fbc_1 << 0x18 | data_c4fb8_1 << 0x12 | data_c4fac_1 | data_c4fb4_1 << 0xc | data_c4fb0_1 << 6);
    system_reg_write(0x48fc, 
        data_c4fd0_1 << 0x18 | data_c4fcc_1 << 0x12 | data_c4fc0_1 | data_c4fc8_1 << 0xc | data_c4fc4_1 << 6);
    system_reg_write(0x4900, 
        data_c4fe4_1 << 0x18 | data_c4fe0_1 << 0x12 | data_c4fd4_1 | data_c4fdc_1 << 0xc | data_c4fd8_1 << 6);
    system_reg_write(0x4904, data_c4fec_1 << 6 | data_c4fe8_1);
    return 0;
}

