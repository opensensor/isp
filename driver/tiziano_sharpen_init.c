#include "include/main.h"


  int32_t tiziano_sharpen_init()

{
    void* $v0;
    
    if (sharpen_wdr_en)
    {
        y_sp_uu_thres_array_now = &y_sp_uu_thres_wdr_array;
        y_sp_w_sl_stren_0_array_now = &y_sp_w_sl_stren_0_wdr_array;
        y_sp_w_sl_stren_1_array_now = &y_sp_w_sl_stren_1_wdr_array;
        y_sp_w_sl_stren_2_array_now = &y_sp_w_sl_stren_2_wdr_array;
        y_sp_w_sl_stren_3_array_now = &y_sp_w_sl_stren_3_wdr_array;
        y_sp_b_sl_stren_0_array_now = &y_sp_b_sl_stren_0_wdr_array;
        y_sp_b_sl_stren_1_array_now = &y_sp_b_sl_stren_1_wdr_array;
        y_sp_b_sl_stren_2_array_now = &y_sp_b_sl_stren_2_wdr_array;
        $v0 = &y_sp_b_sl_stren_3_wdr_array;
    }
    else
    {
        y_sp_uu_thres_array_now = &y_sp_uu_thres_array;
        y_sp_w_sl_stren_0_array_now = &y_sp_w_sl_stren_0_array;
        y_sp_w_sl_stren_1_array_now = &y_sp_w_sl_stren_1_array;
        y_sp_w_sl_stren_2_array_now = &y_sp_w_sl_stren_2_array;
        y_sp_w_sl_stren_3_array_now = &y_sp_w_sl_stren_3_array;
        y_sp_b_sl_stren_0_array_now = &y_sp_b_sl_stren_0_array;
        y_sp_b_sl_stren_1_array_now = &y_sp_b_sl_stren_1_array;
        y_sp_b_sl_stren_2_array_now = &y_sp_b_sl_stren_2_array;
        $v0 = &y_sp_b_sl_stren_3_array;
    }
    
    y_sp_b_sl_stren_3_array_now = $v0;
    data_9a920_1 = 0xffffffff;
    tiziano_sharpen_params_refresh();
    tisp_sharpen_par_refresh(isp_printf, isp_printf, 1);
    return 0;
}

