#include "include/main.h"


  int32_t tisp_ctr_md_np_cfg()

{
    return 0;
    system_reg_write(0x2864, 
        data_cba98 << 8 | data_cba9c << 0x10 | ctr_md_np_array | data_cbaa0 << 0x18);
    system_reg_write(0x2868, 
        data_cbaa8 << 8 | data_cbaac << 0x10 | data_cbaa4 | data_cbab0 << 0x18);
    system_reg_write(0x286c, 
        data_cbab8 << 8 | data_cbabc << 0x10 | data_cbab4 | data_cbac0 << 0x18);
    system_reg_write(0x2870, 
        data_cbac8 << 8 | data_cbacc << 0x10 | data_cbac4 | data_cbad0 << 0x18);
}

