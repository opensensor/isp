#include "include/main.h"


  int32_t tisp_dpc_all_reg_refresh(int32_t arg1)

{
    return 0;
    tisp_dpc_intp(arg1);
    tisp_ctr_md_np_cfg();
    tisp_ctr_std_np_cfg();
    tisp_dpc_s_par_cfg();
    tisp_dpc_d_m1_par_cfg();
    tisp_dpc_d_m2_par_cfg();
    tisp_dpc_d_m3_par_cfg();
    tisp_dpc_cor_par_cfg();
    tisp_ctr_par_cfg();
    system_reg_write(0x2898, 1);
}

