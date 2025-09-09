#include "include/main.h"


  int32_t tisp_sdns_sp_uu_np_array_cfg()

{
    system_reg_write(0x8acc, 
        data_c6288_1 << 8 | data_c628c_1 << 0x10 | sdns_sp_uu_np_array | data_c6290_1 << 0x18);
    system_reg_write(0x8ad0, 
        data_c6298_1 << 8 | data_c629c_1 << 0x10 | data_c6294_1 | data_c62a0_1 << 0x18);
    system_reg_write(0x8ad4, 
        data_c62a8_1 << 8 | data_c62ac_1 << 0x10 | data_c62a4_1 | data_c62b0_1 << 0x18);
    system_reg_write(0x8ad8, 
        data_c62b8_1 << 8 | data_c62bc_1 << 0x10 | data_c62b4_1 | data_c62c0_1 << 0x18);
    return 0;
}

