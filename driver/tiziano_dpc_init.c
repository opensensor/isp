#include "include/main.h"


  int32_t tiziano_dpc_init()

{
    void* $v0;
    
    if (dpc_wdr_en)
    {
        dpc_d_m1_dthres_array_now = &dpc_d_m1_dthres_wdr_array;
        dpc_d_m1_fthres_array_now = &dpc_d_m1_fthres_wdr_array;
        dpc_d_m3_dthres_array_now = &dpc_d_m3_dthres_wdr_array;
        $v0 = &dpc_d_m3_fthres_wdr_array;
    }
    else
    {
        dpc_d_m1_dthres_array_now = &dpc_d_m1_dthres_array;
        dpc_d_m1_fthres_array_now = &dpc_d_m1_fthres_array;
        dpc_d_m3_dthres_array_now = &dpc_d_m3_dthres_array;
        $v0 = &dpc_d_m3_fthres_array;
    }
    
    dpc_d_m3_fthres_array_now = $v0;
    data_9ab10_3 = 0xffffffff;
    tiziano_dpc_params_refresh();
    tisp_dpc_par_refresh(isp_printf, isp_printf, 1);
    return 0;
}

