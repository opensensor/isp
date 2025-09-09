#include "include/main.h"


  int32_t tisp_dmsc_sp_d_sigma_3_np_cfg()

{
    system_reg_write(0x48e8, 
        data_c5004_1 << 0x19 | data_c5000_1 << 0x14 | dmsc_sp_d_sigma_3_np_array | data_c4ffc_1 << 0xf
            | data_c4ff8_1 << 0xa | data_c4ff4_1 << 5);
    system_reg_write(0x48ec, 
        data_c501c_1 << 0x19 | data_c5018_1 << 0x14 | data_c5008_1 | data_c5014_1 << 0xf | data_c5010_1 << 0xa
            | data_c500c_1 << 5);
    system_reg_write(0x48f0, data_c502c_1 << 0xf | data_c5028_1 << 0xa | data_c5020_1 | data_c5024_1 << 5);
    return 0;
}

