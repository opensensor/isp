#include "include/main.h"


  int32_t tisp_ctr_md_np_cfg()

{
    system_reg_write(0x2864, 
        data_cba98_1 << 8 | data_cba9c_1 << 0x10 | ctr_md_np_array | data_cbaa0_1 << 0x18);
    system_reg_write(0x2868, 
        data_cbaa8_1 << 8 | data_cbaac_1 << 0x10 | data_cbaa4_1 | data_cbab0_1 << 0x18);
    system_reg_write(0x286c, 
        data_cbab8_1 << 8 | data_cbabc_1 << 0x10 | data_cbab4_1 | data_cbac0_1 << 0x18);
    system_reg_write(0x2870, 
        data_cbac8_1 << 8 | data_cbacc_1 << 0x10 | data_cbac4_1 | data_cbad0_1 << 0x18);
    return 0;
}

