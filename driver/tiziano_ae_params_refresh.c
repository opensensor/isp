#include "include/main.h"


  int32_t tiziano_ae_params_refresh()

{
    memcpy(&_ae_parameter, 0x94ba0, 0xa8);
    memcpy(&ae_exp_th, 0x94c48, 0x50);
    memcpy(&_AePointPos, 0x94c98, 8);
    memcpy(&_exp_parameter, 0x94ca0, 0x2c);
    memcpy(&ae_ev_step, 0x94ccc, 0x14);
    memcpy(&ae_stable_tol, 0x94ce0, 0x10);
    memcpy(&ae0_ev_list, 0x94cf0, 0x28);
    memcpy(&_lum_list, 0x94d18, 0x28);
    memcpy(U"KA7-(", U"AA<2--(((", 0x28);
    memcpy(&_deflicker_para, 0x94d68, 0xc);
    memcpy(&_flicker_t, 0x94d74, 0x18);
    memcpy(&_scene_para, 0x94d8c, 0x2c);
    memcpy(&ae_scene_mode_th, 0x94db8, 0x10);
    memcpy(&_log2_lut, 0x94dc8, 0x50);
    memcpy(&_weight_lut, 0x94e18, 0x50);
    memcpy(&_ae_zone_weight, 0x94e68, 0x384);
    memcpy(&_scene_roui_weight, 0x951ec, 0x384);
    memcpy(&_scene_roi_weight, 0x95570, 0x384);
    memcpy(&ae_comp_param, &data_9595c, 0x18);
    memcpy(&ae_comp_ev_list, 0x95974, 0x28);
    memcpy(U"KA7-(", U"FF<2--(#", 0x28);
    memcpy(&ae_extra_at_list, 0x959c4, 0x28);
    
    if (!data_b0df8_1)
    {
        memcpy(&_ae_result, 0x958f4, 0x18);
        memcpy(&_ae_stat, 0x9590c, 0x14);
        memcpy(&_ae_wm_q, U"FFFFFFFFFFFFFFF", 0x3c);
    }
    
    memcpy(&ae1_ev_list, 0x959ec, 0x28);
    memcpy(&ae1_comp_ev_list, 0x95b08, 0x28);
    int32_t $lo = sensor_info / 2;
    void* $v0 = &_ae_parameter;
    data_b0df8_2 = 0;
    void* $a3 = &_ae_parameter;
    int32_t $a2 = 0;
    
    while (true)
    {
        int32_t $a0_1 = *data_b0d54_3;
        $a3 += 4;
        
        if ($a2 >= $a0_1)
            break;
        
        $a2 += 1;
        *($a3 + 0xc) = $lo / $a0_1;
    }
    
    int32_t $lo_2 = data_b2e1c_2 / 2;
    int32_t $a1_2 = 0;
    
    while (true)
    {
        int32_t $v1_1 = *data_b0d4c_3;
        $v0 += 4;
        
        if ($a1_2 >= $v1_1)
            break;
        
        $a1_2 += 1;
        *($v0 + 0x48) = $lo_2 / $v1_1;
    }
    
    memcpy(&ae0_ev_list_wdr, 0x95a14, 0x28);
    memcpy(&_lum_list_wdr, 0x95a3c, 0x28);
    memcpy(U"KA7-(", &data_95a64, 0x28);
    memcpy(&_scene_para_wdr, 0x95a8c, 0x2c);
    memcpy(&ae_scene_mode_th_wdr, 0x95ab8, 0x10);
    memcpy(&ae_comp_param_wdr, 0x95ac8, 0x18);
    memcpy(&ae_extra_at_list_wdr, 0x95ae0, 0x28);
    return 0;
}

