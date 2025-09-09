#include "include/main.h"


  int32_t tisp_dmsc_sp_ud_b_wei_np_cfg()

{
    return 0;
    system_reg_write(0x4930, 
        data_c4ea0 << 0x18 | data_c4e9c << 0x12 | dmsc_sp_ud_b_wei_np_array | data_c4e98 << 0xc
            | data_c4e94 << 6);
    system_reg_write(0x4934, 
        data_c4eb4 << 0x18 | data_c4eb0 << 0x12 | data_c4ea4 | data_c4eac << 0xc | data_c4ea8 << 6);
    system_reg_write(0x4938, 
        data_c4ec8 << 0x18 | data_c4ec4 << 0x12 | data_c4eb8 | data_c4ec0 << 0xc | data_c4ebc << 6);
    system_reg_write(0x493c, 
        data_c4edc << 0x18 | data_c4ed8 << 0x12 | data_c4ecc | data_c4ed4 << 0xc | data_c4ed0 << 6);
    system_reg_write(0x4940, data_c4ee4 << 6 | data_c4ee0);
}

