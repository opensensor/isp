#include "include/main.h"


  int32_t tisp_y_sp_v1_v2_coef_cfg()

{
    system_reg_write(0x7020, 
        y_sp_v2_win5_thres_intp << 8 | y_sp_v1_v2_coef_par_cfg_array | data_c59a8_1 << 0x10);
    system_reg_write(0x7024, 
        data_c59b0_1 << 4 | data_c59b4_1 << 8 | data_c59ac_1 | data_c59b8_1 << 0xc | data_c59bc_1 << 0x10
            | data_c59c0_1 << 0x14);
    system_reg_write(0x7028, 
        data_c59c8_1 << 8 | data_c59cc_1 << 0x10 | data_c59c4_1 | data_c59d0_1 << 0x18);
    return 0;
}

