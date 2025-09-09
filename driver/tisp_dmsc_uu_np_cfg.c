#include "include/main.h"


  int32_t tisp_dmsc_uu_np_cfg()

{
    system_reg_write(0x48c8, data_c5094_1 << 0x10 | dmsc_uu_np_array);
    system_reg_write(0x48cc, data_c509c_1 << 0x10 | data_c5098_1);
    system_reg_write(0x48d0, data_c50a4_1 << 0x10 | data_c50a0_1);
    system_reg_write(0x48d4, data_c50ac_1 << 0x10 | data_c50a8_1);
    system_reg_write(0x48d8, data_c50b4_1 << 0x10 | data_c50b0_1);
    system_reg_write(0x48dc, data_c50bc_1 << 0x10 | data_c50b8_1);
    system_reg_write(0x48e0, data_c50c4_1 << 0x10 | data_c50c0_1);
    system_reg_write(0x48e4, data_c50cc_1 << 0x10 | data_c50c8_1);
    return 0;
}

