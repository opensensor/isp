#include "include/main.h"


  int32_t tisp_sdns_sp_ud_w_wei_np_array_cfg()

{
    system_reg_write(0x8b0c, 
        data_c6180 << 8 | data_c6184 << 0x10 | sdns_sp_ud_w_wei_np_array | data_c6188 << 0x18);
    system_reg_write(0x8b10, 
        data_c6190 << 8 | data_c6194 << 0x10 | data_c618c | data_c6198 << 0x18);
    system_reg_write(0x8b14, 
        data_c61a0 << 8 | data_c61a4 << 0x10 | data_c619c | data_c61a8 << 0x18);
    system_reg_write(0x8b18, 
        data_c61b0 << 8 | data_c61b4 << 0x10 | data_c61ac | data_c61b8 << 0x18);
    system_reg_write(0x8b1c, 
        data_c61c0 << 8 | data_c61c4 << 0x10 | data_c61bc | data_c61c8 << 0x18);
    system_reg_write(0x8b20, data_c61d0 << 8 | data_c61cc);
    return 0;
}

