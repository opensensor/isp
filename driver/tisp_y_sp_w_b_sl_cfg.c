#include "include/main.h"


  int32_t tisp_y_sp_w_b_sl_cfg()

{
    system_reg_write(0x7068, y_sp_w_sl_stren_1_intp << 0x10 | y_sp_w_sl_stren_0_intp);
    system_reg_write(0x706c, y_sp_w_sl_stren_3_intp << 0x10 | y_sp_w_sl_stren_2_intp);
    system_reg_write(0x7070, y_sp_b_sl_stren_1_intp << 0x10 | y_sp_b_sl_stren_0_intp);
    system_reg_write(0x7074, y_sp_b_sl_stren_3_intp << 0x10 | y_sp_b_sl_stren_2_intp);
    return 0;
}

