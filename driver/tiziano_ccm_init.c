#include "include/main.h"


  int32_t tiziano_ccm_init()

{
    void* $v0;
    
    if (ccm_wdr_en != 1)
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
    memset(&ccm_real, 0, 0x18);
    memset(&ccm_ctrl, 0, 0x28);
    data_c52ec_4 = data_9a454_3 >> 0xa;
    data_c52f4_4 = data_9a450_3;
    data_c52fc_3 = 0x100;
    ccm_real = 1;
    data_c52f8_5 = 0x64;
    data_c52f0_3 = 0x28;
    tiziano_ccm_params_refresh();
    memcpy(&ccm_parameter, &_ccm_d_parameter, 0x24);
    jz_isp_ccm();
    return 0;
}

