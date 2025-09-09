#include "include/main.h"


  int32_t tisp_ae_param_array_get(int32_t arg1, int32_t arg2, int32_t* arg3)

{
        int32_t var_18_1 = arg1;
        return 0xffffffff;
    if (arg1 - (uintptr_t)1 >= 0x22)
    {
        isp_printf(); // Fixed: macro with no parameters, removed 3 arguments;
    }
    
    uint64_t* $a1_1;
    int32_t $s1_1;
    
    switch (arg1)
    {
        case 1:
        {
            $a1_1 = &_ae_parameter;
            $s1_1 = 0xa8;
            break;
        }
        case 2:
        {
            $a1_1 = &ae_exp_th;
            $s1_1 = 0x50;
            break;
        }
        case 3:
        {
            $a1_1 = &_AePointPos;
            $s1_1 = 8;
            break;
        }
        case 4:
        {
            $a1_1 = &_exp_parameter;
            $s1_1 = 0x2c;
            break;
        }
        case 5:
        {
            $a1_1 = &ae_ev_step;
            $s1_1 = 0x14;
            break;
        }
        case 6:
        {
            $a1_1 = &ae_stable_tol;
            $s1_1 = 0x10;
            break;
        }
        case 7:
        {
            $a1_1 = &ae0_ev_list;
            $s1_1 = 0x28;
            break;
        }
        case 8:
        {
            $a1_1 = &_lum_list;
            $s1_1 = 0x28;
            break;
        }
        case 9:
        {
            $a1_1 = U"KA7-(";
            $s1_1 = 0x28;
            break;
        }
        case 0xa:
        {
            $a1_1 = &_deflicker_para;
            $s1_1 = 0xc;
            break;
        }
        case 0xb:
        {
            $a1_1 = &_flicker_t;
            $s1_1 = 0x18;
            break;
        }
        case 0xc:
        {
            $a1_1 = &_scene_para;
            $s1_1 = 0x2c;
            break;
        }
        case 0xd:
        {
            $a1_1 = &ae_scene_mode_th;
            $s1_1 = 0x10;
            break;
        }
        case 0xe:
        {
            $a1_1 = &_log2_lut;
            $s1_1 = 0x50;
            break;
        }
        case 0xf:
        {
            $a1_1 = &_weight_lut;
            $s1_1 = 0x50;
            break;
        }
        case 0x10:
        {
            $a1_1 = &_ae_zone_weight;
            $s1_1 = 0x384;
            break;
        }
        case 0x11:
        {
            $a1_1 = &_scene_roui_weight;
            $s1_1 = 0x384;
            break;
        }
        case 0x12:
        {
            $a1_1 = &_scene_roi_weight;
            $s1_1 = 0x384;
            break;
        }
        case 0x13:
        {
            $a1_1 = &_ae_result;
            $s1_1 = 0x18;
            break;
        }
        case 0x14:
        {
            $a1_1 = &_ae_stat;
            $s1_1 = 0x14;
            break;
        }
        case 0x15:
        {
            $a1_1 = &_ae_wm_q;
            $s1_1 = 0x3c;
            break;
        }
        case 0x16:
        {
            $a1_1 = &ae_comp_param;
            $s1_1 = 0x18;
            break;
        }
        case 0x17:
        {
            $a1_1 = &ae_comp_ev_list;
            $s1_1 = 0x28;
            break;
        }
        case 0x18:
        {
            $a1_1 = U"KA7-(";
            $s1_1 = 0x28;
            break;
        }
        case 0x19:
        {
            $a1_1 = &ae_extra_at_list;
            $s1_1 = 0x28;
            break;
        }
        case 0x1a:
        {
            $a1_1 = &ae1_ev_list;
            $s1_1 = 0x28;
            break;
        }
        case 0x1b:
        {
            $a1_1 = &ae0_ev_list_wdr;
            $s1_1 = 0x28;
            break;
        }
        case 0x1c:
        {
            $a1_1 = &_lum_list_wdr;
            $s1_1 = 0x28;
            break;
        }
        case 0x1d:
        {
            $a1_1 = U"KA7-(";
            $s1_1 = 0x28;
            break;
        }
        case 0x1e:
        {
            $a1_1 = &_scene_para_wdr;
            $s1_1 = 0x2c;
            break;
        }
        case 0x1f:
        {
            $a1_1 = &ae_scene_mode_th_wdr;
            $s1_1 = 0x10;
            break;
        }
        case 0x20:
        {
            $a1_1 = &ae_comp_param_wdr;
            $s1_1 = 0x18;
            break;
        }
        case 0x21:
        {
            $a1_1 = &ae_extra_at_list_wdr;
            $s1_1 = 0x28;
            break;
        }
        case 0x22:
        {
            $a1_1 = &ae1_comp_ev_list;
            $s1_1 = 0x28;
            break;
        }
    }
    
    memcpy(arg2, $a1_1, $s1_1);
    *arg3 = $s1_1;
    return 0;
}

