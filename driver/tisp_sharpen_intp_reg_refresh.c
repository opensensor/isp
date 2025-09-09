#include "include/main.h"


  int32_t tisp_sharpen_intp_reg_refresh()

{
    subsection_map();
    tisp_y_sp_sl_exp_cfg();
    tisp_y_sp_uu_cfg();
    tisp_y_sp_fl_thres_cfg();
    tisp_y_sp_v1_v2_coef_cfg();
    tisp_y_sp_w_b_sl_cfg();
    return 0;
}

