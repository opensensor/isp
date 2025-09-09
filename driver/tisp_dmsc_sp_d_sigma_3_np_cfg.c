#include "include/main.h"


  int32_t tisp_dmsc_sp_d_sigma_3_np_cfg()

{
    system_reg_write(0x48e8, 
        data_c5004 << 0x19 | data_c5000 << 0x14 | dmsc_sp_d_sigma_3_np_array | data_c4ffc << 0xf
            | data_c4ff8 << 0xa | data_c4ff4 << 5);
    system_reg_write(0x48ec, 
        data_c501c << 0x19 | data_c5018 << 0x14 | data_c5008 | data_c5014 << 0xf | data_c5010 << 0xa
            | data_c500c << 5);
    system_reg_write(0x48f0, data_c502c << 0xf | data_c5028 << 0xa | data_c5020 | data_c5024 << 5);
    return 0;
}

