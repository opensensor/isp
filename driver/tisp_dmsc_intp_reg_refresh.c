#include "include/main.h"


  int32_t tisp_dmsc_intp_reg_refresh(int32_t arg1)

{
    return 0;
    tisp_dmsc_intp(arg1);
    tisp_dmsc_uu_par_cfg();
    tisp_dmsc_alias_par_cfg();
    tisp_dmsc_dir_par_cfg();
    tisp_dmsc_nor_par_cfg();
    tisp_dmsc_sp_d_par_cfg();
    tisp_dmsc_sp_ud_par_cfg();
    tisp_dmsc_sp_alias_par_cfg();
    tisp_dmsc_rgb_alias_par_cfg();
    tisp_dmsc_fc_par_cfg();
    tisp_dmsc_deir_par_cfg();
    tisp_dmsc_d_ud_ns_par_cfg();
}

