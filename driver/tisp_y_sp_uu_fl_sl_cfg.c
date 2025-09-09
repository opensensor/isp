#include "include/main.h"


  int32_t tisp_y_sp_uu_fl_sl_cfg()

{
    system_reg_write(0x7078, 
        y_sp_uu_sl_1_array_intp << 8 | y_sp_uu_sl_2_array_intp << 0x10 | y_sp_uu_sl_0_array_intp
            | y_sp_uu_sl_3_array_intp << 0x18);
    system_reg_write(0x707c, 
        y_sp_fl_sl_1_array_intp << 8 | y_sp_fl_sl_2_array_intp << 0x10 | y_sp_fl_sl_0_array_intp
            | y_sp_fl_sl_3_array_intp << 0x18);
    return 0;
}

