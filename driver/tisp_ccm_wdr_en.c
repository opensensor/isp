#include "include/main.h"


  int32_t tisp_ccm_wdr_en(uint32_t arg1)

{
    void* $v0;
    ccm_wdr_en = arg1;
    
    if (arg1 != 1)
    {
        tiziano_ccm_a_now = &tiziano_ccm_a_linear;
        tiziano_ccm_t_now = &tiziano_ccm_t_linear;
        tiziano_ccm_d_now = &tiziano_ccm_d_linear;
        cm_ev_list_now = &cm_ev_list;
        $v0 = &cm_sat_list;
    }
    else
    {
        tiziano_ccm_a_now = &tiziano_ccm_a_wdr;
        tiziano_ccm_t_now = &tiziano_ccm_t_wdr;
        tiziano_ccm_d_now = &tiziano_ccm_d_wdr;
        cm_ev_list_now = &cm_ev_list_wdr;
        $v0 = &cm_sat_list_wdr;
    }
    
    cm_sat_list_now = $v0;
    return 0;
}

