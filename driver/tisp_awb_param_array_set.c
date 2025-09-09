#include "include/main.h"


  int32_t tisp_awb_param_array_set(int32_t arg1, int32_t arg2, int32_t* arg3)

{
        int32_t var_10_1 = arg1;
    if (arg1 - (uintptr_t)0x23 >= 0x19)
    {
        isp_printf(); // Fixed: macro call, removed arguments;
        return 0xffffffff;
    }
    
    int32_t $v0_2;
    
    switch (arg1)
    {
        case 0x23:
        {
            memcpy(&_awb_parameter, arg2, 0xb4);
            $v0_2 = 0xb4;
            break;
        }
        case 0x24:
        {
            memcpy(&_pixel_cnt_th);
            $v0_2 = 4;
            break;
        }
        case 0x25:
        {
            memcpy(&_awb_lowlight_rg_th);
            $v0_2 = 8;
            break;
        }
        case 0x26:
        {
            memcpy(&_AwbPointPos);
            $v0_2 = 8;
            break;
        }
        case 0x27:
        {
            memcpy(&_awb_cof);
            $v0_2 = 8;
            break;
        }
        case 0x28:
        {
            $v0_2 = 0x18;
            break;
        }
        case 0x29:
        {
            memcpy(&_awb_mode);
            $v0_2 = 0xc;
            break;
        }
        case 0x2a:
        case 0x2b:
        {
            $v0_2 = 4;
            break;
        }
        case 0x2c:
        {
            memcpy(&_wb_static);
            $v0_2 = 8;
            break;
        }
        case 0x2d:
        {
            memcpy(&_light_src, arg2, 0x50);
            $v0_2 = 0x50;
            break;
        }
        case 0x2e:
        {
            memcpy(&_light_src_num);
            $v0_2 = 4;
            break;
        }
        case 0x2f:
        {
            memcpy(&_rg_pos);
            $v0_2 = 0x3c;
            break;
        }
        case 0x30:
        {
            memcpy(&_bg_pos);
            $v0_2 = 0x3c;
            break;
        }
        case 0x31:
        {
            memcpy(&_awb_ct_th_ot_luxhigh);
            $v0_2 = 0x10;
            break;
        }
        case 0x32:
        {
            memcpy(&_awb_ct_th_ot_luxlow);
            $v0_2 = 0x10;
            break;
        }
        case 0x33:
        {
            memcpy(&_awb_ct_th_in);
            $v0_2 = 0x10;
            break;
        }
        case 0x34:
        {
            memcpy(&_awb_ct_para_ot);
            $v0_2 = 8;
            break;
        }
        case 0x35:
        {
            memcpy(&_awb_ct_para_in);
            $v0_2 = 8;
            break;
        }
        case 0x36:
        {
            memcpy(&_awb_dis_tw);
            $v0_2 = 0xc;
            break;
        }
        case 0x37:
        {
            memcpy(&_rgbg_weight);
            $v0_2 = 0x384;
            break;
        }
        case 0x38:
        {
            memcpy(&_color_temp_mesh);
            $v0_2 = 0x384;
            break;
        }
        case 0x39:
        {
            memcpy(&_awb_wght);
            $v0_2 = 0x384;
            break;
        }
        case 0x3a:
        {
            memcpy(&_rgbg_weight_ot);
            $v0_2 = 0x384;
            break;
        }
        case 0x3b:
        {
            memcpy(&_ls_w_lut, arg2, 0x808);
            $v0_2 = 0x808;
            break;
        }
    }
    
    *arg3 = $v0_2;
    tiziano_awb_set_hardware_param();
    return 0;
}

