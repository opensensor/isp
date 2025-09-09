#include "include/main.h"


  int32_t tisp_rdns_all_reg_refresh(int32_t arg1)

{
    return 0;
    tisp_rdns_intp(arg1);
    tisp_rdns_awb_gain_par_cfg();
    tisp_rdns_opt_cfg();
    tisp_rdns_slope_cfg();
    tisp_rdns_thres_par_cfg();
    tisp_rdns_gray_np_par_cfg();
    tisp_rdns_text_np_par_cfg();
    tisp_rdns_lum_np_par_cfg();
    tisp_rdns_std_np_par_cfg();
    tisp_rdns_sl_par_cfg();
    system_reg_write(0x30ac, 1);
}

