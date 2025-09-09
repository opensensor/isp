#include "include/main.h"


  void* tisp_dpc_wdr_en(uint32_t arg1)

{
    dpc_wdr_en = arg1;
    void* result;
    
    if (arg1)
    {
        dpc_d_m1_dthres_array_now = &dpc_d_m1_dthres_wdr_array;
        dpc_d_m1_fthres_array_now = &dpc_d_m1_fthres_wdr_array;
        dpc_d_m3_dthres_array_now = &dpc_d_m3_dthres_wdr_array;
        result = &dpc_d_m3_fthres_wdr_array;
    }
    else
    {
        dpc_d_m1_dthres_array_now = &dpc_d_m1_dthres_array;
        dpc_d_m1_fthres_array_now = &dpc_d_m1_fthres_array;
        dpc_d_m3_dthres_array_now = &dpc_d_m3_dthres_array;
        result = &dpc_d_m3_fthres_array;
    }
    
    dpc_d_m3_fthres_array_now = result;
    return result;
}

