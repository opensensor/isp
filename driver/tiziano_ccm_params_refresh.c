#include "include/main.h"


  int32_t tiziano_ccm_params_refresh()

{
    if (!ccm_ctrl)
    {
        memcpy(&tiziano_ccm_a_linear, 0x9e708, 0x24);
        memcpy(&tiziano_ccm_t_linear, 0x9e72c, 0x24);
        memcpy(&cm_sat_list, 0x9e798, 0x24);
        memcpy(&tiziano_ccm_a_wdr, 0x9e7bc, 0x24);
        memcpy(&tiziano_ccm_t_wdr, 0x9e7e0, 0x24);
        memcpy(&tiziano_ccm_d_wdr, 0x9e804, 0x24);
        memcpy(&tiziano_ccm_d_linear, 0x9e750, 0x24);
        memcpy(&cm_sat_list_wdr, 0x9e84c, 0x24);
    }
    
    memcpy(&tiziano_ccm_dp_cfg, 0x9e6f4, 0x14);
    memcpy(&cm_ev_list, 0x9e774, 0x24);
    memcpy(&cm_ev_list_wdr, 0x9e828, 0x24);
    memcpy(&cm_awb_list, 0x9e870, 8);
    return 0;
}

