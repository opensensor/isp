#include "include/main.h"


  void* tisp_dmsc_wdr_en(uint32_t arg1)

{
    void* result;
    dmsc_wdr_en = arg1;
    
    if (arg1)
    {
        dmsc_uu_thres_array_now = &dmsc_uu_thres_wdr_array;
        dmsc_uu_stren_array_now = &dmsc_uu_stren_wdr_array;
        dmsc_sp_d_w_stren_array_now = &dmsc_sp_d_w_stren_wdr_array;
        dmsc_sp_d_b_stren_array_now = &dmsc_sp_d_b_stren_wdr_array;
        dmsc_sp_ud_w_stren_array_now = &dmsc_sp_ud_w_stren_wdr_array;
        result = &dmsc_sp_ud_b_stren_wdr_array;
    }
    else
    {
        dmsc_uu_thres_array_now = &dmsc_uu_thres_array;
        dmsc_uu_stren_array_now = &dmsc_uu_stren_array;
        dmsc_sp_d_w_stren_array_now = &dmsc_sp_d_w_stren_array;
        dmsc_sp_d_b_stren_array_now = &dmsc_sp_d_b_stren_array;
        dmsc_sp_ud_w_stren_array_now = &dmsc_sp_ud_w_stren_array;
        result = &dmsc_sp_ud_b_stren_array;
    }
    
    dmsc_sp_ud_b_stren_array_now = result;
    return result;
}

