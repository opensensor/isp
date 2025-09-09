#include "include/main.h"


  int32_t tisp_awb_param_array_get(int32_t arg1, int32_t arg2, int32_t* arg3)

{
    if (arg1 - 0x23 >= 0x19)
    {
        int32_t var_18_1_3 = arg1;
        isp_printf(2, "VIC_CTRL : %08x\\n", "tisp_awb_param_array_get");
        return 0xffffffff;
    }
    
    uint64_t* $a1_1;
    int32_t $s1_1;
    
    switch (arg1)
    {
        case 0x23:
        {
            $a1_1 = &_awb_parameter;
            $s1_1 = 0xb4;
            break;
        }
        case 0x24:
        {
            $a1_1 = &_pixel_cnt_th;
            $s1_1 = 4;
            break;
        }
        case 0x25:
        {
            $a1_1 = &_awb_lowlight_rg_th;
            $s1_1 = 8;
            break;
        }
        case 0x26:
        {
            $a1_1 = &_AwbPointPos;
            $s1_1 = 8;
            break;
        }
        case 0x27:
        {
            $a1_1 = &_awb_cof;
            $s1_1 = 8;
            break;
        }
        case 0x28:
        {
            $a1_1 = &_awb_mf_para;
            $s1_1 = 0x18;
            break;
        }
        case 0x29:
        {
            $a1_1 = &_awb_mode;
            $s1_1 = 0xc;
            break;
        }
        case 0x2a:
        {
            $a1_1 = &_awb_ct;
            $s1_1 = 4;
            break;
        }
        case 0x2b:
        {
            $a1_1 = &_awb_ct_last;
            $s1_1 = 4;
            break;
        }
        case 0x2c:
        {
            $a1_1 = &_wb_static;
            $s1_1 = 8;
            break;
        }
        case 0x2d:
        {
            $a1_1 = &_light_src;
            $s1_1 = 0x50;
            break;
        }
        case 0x2e:
        {
            $a1_1 = &_light_src_num;
            $s1_1 = 4;
            break;
        }
        case 0x2f:
        {
            $a1_1 = &_rg_pos;
            $s1_1 = 0x3c;
            break;
        }
        case 0x30:
        {
            $a1_1 = &_bg_pos;
            $s1_1 = 0x3c;
            break;
        }
        case 0x31:
        {
            $a1_1 = &_awb_ct_th_ot_luxhigh;
            $s1_1 = 0x10;
            break;
        }
        case 0x32:
        {
            $a1_1 = &_awb_ct_th_ot_luxlow;
            $s1_1 = 0x10;
            break;
        }
        case 0x33:
        {
            $a1_1 = &_awb_ct_th_in;
            $s1_1 = 0x10;
            break;
        }
        case 0x34:
        {
            $a1_1 = &_awb_ct_para_ot;
            $s1_1 = 8;
            break;
        }
        case 0x35:
        {
            $a1_1 = &_awb_ct_para_in;
            $s1_1 = 8;
            break;
        }
        case 0x36:
        {
            $a1_1 = &_awb_dis_tw;
            $s1_1 = 0xc;
            break;
        }
        case 0x37:
        {
            $a1_1 = &_rgbg_weight;
            $s1_1 = 0x384;
            break;
        }
        case 0x38:
        {
            $a1_1 = &_color_temp_mesh;
            $s1_1 = 0x384;
            break;
        }
        case 0x39:
        {
            $a1_1 = &_awb_wght;
            $s1_1 = 0x384;
            break;
        }
        case 0x3a:
        {
            $a1_1 = &_rgbg_weight_ot;
            $s1_1 = 0x384;
            break;
        }
        case 0x3b:
        {
            $a1_1 = &_ls_w_lut;
            $s1_1 = 0x808;
            break;
        }
    }
    
    memcpy(arg2, $a1_1, $s1_1);
    *arg3 = $s1_1;
    return 0;
}

