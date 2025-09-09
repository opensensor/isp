#include "include/main.h"


  int32_t tisp_rdns_slope_cfg()

{
    system_reg_write(0x300c, rdns_out_opt_array << 0x10 | rdns_gray_stren_intp);
    system_reg_write(0x3010, *(rdns_slope_par_cfg_array + 4) << 0x10 | *rdns_slope_par_cfg_array);
    return 0;
}

