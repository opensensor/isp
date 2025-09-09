#include "include/main.h"


  int32_t tisp_sharpen_all_reg_refresh()

{
    subsection_map();
    tisp_y_sp_sl_exp_cfg();
    tisp_y_sp_std_scope_cfg();
    tisp_y_sp_uu_cfg();
    tisp_y_sp_fl_thres_cfg();
    tisp_y_sp_v1_v2_coef_cfg();
    tisp_y_sp_w_b_ll_cfg();
    tisp_y_sp_uu_w_b_wei_cfg();
    tisp_y_sp_w_b_sl_cfg();
    tisp_y_sp_uu_fl_sl_cfg();
    system_reg_write(0x7090, 1);
    return 0;
}

