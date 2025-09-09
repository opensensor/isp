#include "include/main.h"


  int32_t tisp_dmsc_d_ud_ns_par_cfg()

{
    return 0;
    system_reg_write(0x49a0, *dmsc_sp_d_ud_ns_opt << 0x10 | dmsc_sp_d_ns_thres_intp);
    system_reg_write(0x49a4, *(dmsc_sp_d_ud_ns_opt + 4) << 0x10 | dmsc_sp_ud_ns_thres_intp);
}

