#include "include/main.h"


  int32_t tisp_y_sp_sl_exp_cfg()

{
    system_reg_write(0x7000, y_sp_sl_exp_num_intp << 0x10 | y_sp_sl_exp_thres_intp);
    return 0;
}

