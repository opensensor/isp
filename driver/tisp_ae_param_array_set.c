#include "include/main.h"


  int32_t tisp_ae_param_array_set(int32_t arg1, int32_t arg2, int32_t* arg3)

{
        int32_t var_18_1 = arg1;
        return 0xffffffff;
    if (arg1 - (uintptr_t)1 >= 0x22)
    {

    }
    
    int32_t $v0_2;
    
    switch (arg1)
    {
        case 1:
        {
            memcpy(&_ae_parameter, arg2, 0xa8);
            $v0_2 = 0xa8;
            break;
        }
        case 2:
        {
            memcpy(&ae_exp_th);
            $v0_2 = 0x50;
            break;
        }
        case 3:
        {
            memcpy(&_AePointPos, arg2, 8);
            $v0_2 = 8;
            break;
        }
        case 4:
        {
            memcpy(&_exp_parameter);
            $v0_2 = 0x2c;
            break;
        }
        case 5:
        {
            memcpy(&ae_ev_step, arg2, 0x14);
            $v0_2 = 0x14;
            break;
        }
        case 6:
        {
            memcpy(&ae_stable_tol);
            $v0_2 = 0x10;
            break;
        }
        case 7:
        {
            memcpy(&ae0_ev_list);
            $v0_2 = 0x28;
            break;
        }
        case 8:
        {
            memcpy(&_lum_list);
            $v0_2 = 0x28;
            break;
        }
        case 9:
        {
            memcpy(U"KA7-(");
            $v0_2 = 0x28;
            break;
        }
        case 0xa:
        {
            memcpy(&_deflicker_para, arg2, 0xc);
            $v0_2 = 0xc;
            break;
        }
        case 0xb:
        {
            memcpy(&_flicker_t, arg2, 0x18);
            tiziano_deflicker_expt(_flicker_t, data_b0b28, data_b0b2c, data_b0b30, &_deflick_lut, 
                &_nodes_num);
            $v0_2 = 0x18;
            break;
        }
        case 0xc:
        {
            memcpy(&_scene_para);
            $v0_2 = 0x2c;
            break;
        }
        case 0xd:
        {
            memcpy(&ae_scene_mode_th);
            $v0_2 = 0x10;
            break;
        }
        case 0xe:
        {
            memcpy(&_log2_lut);
            $v0_2 = 0x50;
            break;
        }
        case 0xf:
        {
            memcpy(&_weight_lut);
            $v0_2 = 0x50;
            break;
        }
        case 0x10:
        {
            memcpy(&_ae_zone_weight);
            $v0_2 = 0x384;
            break;
        }
        case 0x11:
        {
            memcpy(&_scene_roui_weight);
            $v0_2 = 0x384;
            break;
        }
        case 0x12:
        {
            memcpy(&_scene_roi_weight);
            $v0_2 = 0x384;
            break;
        }
        case 0x13:
        {
            $v0_2 = 0x18;
            break;
        }
        case 0x14:
        {
            $v0_2 = 0x14;
            break;
        }
        case 0x15:
        {
            $v0_2 = 0x3c;
            break;
        }
        case 0x16:
        {
            memcpy(&ae_comp_param);
            $v0_2 = 0x18;
            break;
        }
        case 0x17:
        case 0x18:
        {
            $v0_2 = 0x28;
            break;
        }
        case 0x19:
        {
            memcpy(&ae_extra_at_list);
            $v0_2 = 0x28;
            break;
        }
        case 0x1a:
        {
            memcpy(&ae1_ev_list);
            $v0_2 = 0x28;
            break;
        }
        case 0x1b:
        {
            memcpy(&ae0_ev_list_wdr);
            $v0_2 = 0x28;
            break;
        }
        case 0x1c:
        {
            memcpy(&_lum_list_wdr);
            $v0_2 = 0x28;
            break;
        }
        case 0x1d:
        {
            memcpy(U"KA7-(");
            $v0_2 = 0x28;
            break;
        }
        case 0x1e:
        {
            memcpy(&_scene_para_wdr);
            $v0_2 = 0x2c;
            break;
        }
        case 0x1f:
        {
            memcpy(&ae_scene_mode_th_wdr);
            $v0_2 = 0x10;
            break;
        }
        case 0x20:
        {
            memcpy(&ae_comp_param_wdr);
            $v0_2 = 0x18;
            break;
        }
        case 0x21:
        {
            memcpy(&ae_extra_at_list_wdr);
            $v0_2 = 0x28;
            break;
        }
        case 0x22:
        {
            memcpy(&ae1_comp_ev_list);
            $v0_2 = 0x28;
            break;
        }
    }
    
    *arg3 = $v0_2;
    data_b0e00_2 = 1;
    data_b0e04_3 = 1;
    data_b0e0c_7 = 0;
    tiziano_ae_set_hardware_param(0, &_ae_parameter, 1);
    tiziano_ae_set_hardware_param(1, &_ae_parameter, 1);
    return 0;
}

