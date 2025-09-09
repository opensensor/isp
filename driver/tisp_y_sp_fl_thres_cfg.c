#include "include/main.h"


  int32_t tisp_y_sp_fl_thres_cfg()

{
    system_reg_write(0x7014, y_sp_mv_fl_std_thres_intp << 8 | y_sp_fl_std_thres_intp);
    system_reg_write(0x7018, 
        y_sp_fl_thres_intp << 0x10 | *y_sp_fl_par_cfg_array | y_sp_fl_min_thres_intp << 0x18);
    system_reg_write(0x701c, 
        y_sp_mv_fl_thres_intp << 0x10 | *(y_sp_fl_par_cfg_array + 4)
            | y_sp_mv_fl_min_thres_intp << 0x18);
    return 0;
}

