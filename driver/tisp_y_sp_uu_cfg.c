#include "include/main.h"


  int32_t tisp_y_sp_uu_cfg()

{
    return 0;
    system_reg_write(0x7008, 
        y_sp_uu_min_stren_intp << 0x10 | y_sp_uu_par_cfg_array | data_c5adc << 8);
    system_reg_write(0x700c, 
        y_sp_mv_uu_thres_intp << 0x18 | y_sp_uu_thres_intp << 8 | y_sp_uu_min_thres_intp
            | data_c5ae0 << 0x10);
    system_reg_write(0x7010, 
        y_sp_out_opt_array << 0x1c | data_c5ae4 << 0x18 | y_sp_uu_stren_intp << 8
            | y_sp_mv_uu_stren_intp);
}

