#include "include/main.h"


  int32_t tisp_sdns_sp_uu_np_array_cfg()

{
    system_reg_write(0x8acc, 
        data_c6288 << 8 | data_c628c << 0x10 | sdns_sp_uu_np_array | data_c6290 << 0x18);
    system_reg_write(0x8ad0, 
        data_c6298 << 8 | data_c629c << 0x10 | data_c6294 | data_c62a0 << 0x18);
    system_reg_write(0x8ad4, 
        data_c62a8 << 8 | data_c62ac << 0x10 | data_c62a4 | data_c62b0 << 0x18);
    system_reg_write(0x8ad8, 
        data_c62b8 << 8 | data_c62bc << 0x10 | data_c62b4 | data_c62c0 << 0x18);
    return 0;
}

