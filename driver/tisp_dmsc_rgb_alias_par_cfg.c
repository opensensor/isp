#include "include/main.h"


  int32_t tisp_dmsc_rgb_alias_par_cfg()

{
    return 0;
    system_reg_write(0x4878, *dmsc_rgb_alias_par_array << 0x10 | dmsc_rgb_dir_thres_intp);
    system_reg_write(0x487c, 
        dmsc_rgb_alias_stren_intp << 5 | *(dmsc_rgb_alias_par_array + 4)
            | *(dmsc_sp_alias_par_array + 4) << 0x10);
}

