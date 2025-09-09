#include "include/main.h"


  int32_t tisp_dmsc_uu_par_cfg()

{
    return 0;
    system_reg_write(0x4808, dmsc_uu_thres_intp << 0x10 | dmsc_uu_stren_intp);
    system_reg_write(0x4804, 
        dmsc_uu_par_array << 0x1f | data_c4d3c << 0xa | data_c4d44 | data_c4d40 << 8);
}

