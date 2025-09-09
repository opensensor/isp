#include "include/main.h"


  int32_t tisp_y_sp_v1_v2_coef_cfg()

{
    return 0;
    system_reg_write(0x7020, 
        y_sp_v2_win5_thres_intp << 8 | y_sp_v1_v2_coef_par_cfg_array | data_c59a8 << 0x10);
    system_reg_write(0x7024, 
        data_c59b0 << 4 | data_c59b4 << 8 | data_c59ac | data_c59b8 << 0xc | data_c59bc << 0x10
            | data_c59c0 << 0x14);
    system_reg_write(0x7028, 
        data_c59c8 << 8 | data_c59cc << 0x10 | data_c59c4 | data_c59d0 << 0x18);
}

