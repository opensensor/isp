#include "include/main.h"


  int32_t tiziano_dmsc_init()

{
    void* $v0;
    
    if (dmsc_wdr_en)
    {
        dmsc_uu_thres_array_now = &dmsc_uu_thres_wdr_array;
        dmsc_uu_stren_array_now = &dmsc_uu_stren_wdr_array;
        dmsc_sp_d_w_stren_array_now = &dmsc_sp_d_w_stren_wdr_array;
        dmsc_sp_d_b_stren_array_now = &dmsc_sp_d_b_stren_wdr_array;
        dmsc_sp_ud_w_stren_array_now = &dmsc_sp_ud_w_stren_wdr_array;
        $v0 = &dmsc_sp_ud_b_stren_wdr_array;
    }
    else
    {
        dmsc_uu_thres_array_now = &dmsc_uu_thres_array;
        dmsc_uu_stren_array_now = &dmsc_uu_stren_array;
        dmsc_sp_d_w_stren_array_now = &dmsc_sp_d_w_stren_array;
        dmsc_sp_d_b_stren_array_now = &dmsc_sp_d_b_stren_array;
        dmsc_sp_ud_w_stren_array_now = &dmsc_sp_ud_w_stren_array;
        $v0 = &dmsc_sp_ud_b_stren_array;
    }
    
    dmsc_sp_ud_b_stren_array_now = $v0;
    data_9a430_8 = 0xffffffff;
    tiziano_dmsc_params_refresh();
    tisp_dmsc_par_refresh(isp_printf, isp_printf, 1);
    return 0;
}

