#include "include/main.h"


  int32_t tisp_rdns_intp_reg_refresh(int32_t arg1)

{
    tisp_rdns_intp(arg1);
    tisp_rdns_opt_cfg();
    tisp_rdns_slope_cfg();
    tisp_rdns_thres_par_cfg();
    tisp_rdns_sl_par_cfg();
    return 0;
}

