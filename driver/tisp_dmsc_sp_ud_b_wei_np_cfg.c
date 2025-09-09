#include "include/main.h"


  int32_t tisp_dmsc_sp_ud_b_wei_np_cfg()

{
    system_reg_write(0x4930, 
        data_c4ea0_1 << 0x18 | data_c4e9c_1 << 0x12 | dmsc_sp_ud_b_wei_np_array | data_c4e98_1 << 0xc
            | data_c4e94_1 << 6);
    system_reg_write(0x4934, 
        data_c4eb4_1 << 0x18 | data_c4eb0_1 << 0x12 | data_c4ea4_1 | data_c4eac_1 << 0xc | data_c4ea8_1 << 6);
    system_reg_write(0x4938, 
        data_c4ec8_1 << 0x18 | data_c4ec4_1 << 0x12 | data_c4eb8_1 | data_c4ec0_1 << 0xc | data_c4ebc_1 << 6);
    system_reg_write(0x493c, 
        data_c4edc_1 << 0x18 | data_c4ed8_1 << 0x12 | data_c4ecc_1 | data_c4ed4_1 << 0xc | data_c4ed0_1 << 6);
    system_reg_write(0x4940, data_c4ee4_1 << 6 | data_c4ee0_1);
    return 0;
}

