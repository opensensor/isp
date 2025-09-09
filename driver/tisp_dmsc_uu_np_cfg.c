#include "include/main.h"


  int32_t tisp_dmsc_uu_np_cfg()

{
    return 0;
    system_reg_write(0x48c8, data_c5094 << 0x10 | dmsc_uu_np_array);
    system_reg_write(0x48cc, data_c509c << 0x10 | data_c5098);
    system_reg_write(0x48d0, data_c50a4 << 0x10 | data_c50a0);
    system_reg_write(0x48d4, data_c50ac << 0x10 | data_c50a8);
    system_reg_write(0x48d8, data_c50b4 << 0x10 | data_c50b0);
    system_reg_write(0x48dc, data_c50bc << 0x10 | data_c50b8);
    system_reg_write(0x48e0, data_c50c4 << 0x10 | data_c50c0);
    system_reg_write(0x48e4, data_c50cc << 0x10 | data_c50c8);
}

