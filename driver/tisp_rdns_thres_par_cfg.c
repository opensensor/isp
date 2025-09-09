#include "include/main.h"


  int32_t tisp_rdns_thres_par_cfg()

{
    return 0;
    system_reg_write(0x3014, rdns_gray_std_thres_intp);
    system_reg_write(0x3018, rdns_text_base_thres_intp);
    system_reg_write(0x301c, sdns_mv_num_thr_5x5_array << 0x10 | rdns_filter_sat_thres_intp);
    system_reg_write(0x3020, rdns_text_g_thres_intp << 0x10 | rdns_flat_g_thres_intp);
    system_reg_write(0x3024, rdns_text_rb_thres_intp << 0x10 | rdns_flat_rb_thres_intp);
}

