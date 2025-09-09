#include "include/main.h"


  int32_t tiziano_rdns_init()

{
    void* $v0;
    
    if (rdns_wdr_en)
        $v0 = &rdns_text_base_thres_wdr_array;
    else
        $v0 = &rdns_text_base_thres_array;
    
    rdns_text_base_thres_array_now = $v0;
    rdns_gain_old = 0xffffffff;
    tiziano_rdns_params_refresh();
    tisp_rdns_par_refresh(isp_printf, isp_printf, 1);
    return 0;
}

