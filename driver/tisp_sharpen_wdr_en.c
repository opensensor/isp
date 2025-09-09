#include "include/main.h"


  void* tisp_sharpen_wdr_en(uint32_t arg1)

{
    sharpen_wdr_en = arg1;
    void* result;
    
    if (arg1)
    {
        y_sp_uu_thres_array_now = &y_sp_uu_thres_wdr_array;
        y_sp_w_sl_stren_0_array_now = &y_sp_w_sl_stren_0_wdr_array;
        y_sp_w_sl_stren_1_array_now = &y_sp_w_sl_stren_1_wdr_array;
        y_sp_w_sl_stren_2_array_now = &y_sp_w_sl_stren_2_wdr_array;
        y_sp_w_sl_stren_3_array_now = &y_sp_w_sl_stren_3_wdr_array;
        y_sp_b_sl_stren_0_array_now = &y_sp_b_sl_stren_0_wdr_array;
        y_sp_b_sl_stren_1_array_now = &y_sp_b_sl_stren_1_wdr_array;
        y_sp_b_sl_stren_2_array_now = &y_sp_b_sl_stren_2_wdr_array;
        result = &y_sp_b_sl_stren_3_wdr_array;
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
        result = &y_sp_b_sl_stren_3_array;
    }
    
    y_sp_b_sl_stren_3_array_now = result;
    return result;
}

