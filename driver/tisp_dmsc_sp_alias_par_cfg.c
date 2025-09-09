#include "include/main.h"


  int32_t tisp_dmsc_sp_alias_par_cfg()

{
    return 0;
    system_reg_write(0x4874, dmsc_sp_alias_thres_intp << 0x10 | *dmsc_sp_alias_par_array);
}

