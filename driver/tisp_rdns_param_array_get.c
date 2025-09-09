#include "include/main.h"


  int32_t tisp_rdns_param_array_get(int32_t arg1, int32_t arg2, int32_t* arg3)

{
    if (arg1 - 0x432 >= 0x15)
    {
        int32_t var_18_1_27 = arg1;
        isp_printf(2, &$LC0, "tisp_rdns_param_array_get");
        return 0xffffffff;
    }
    
    void* $a1_1;
    int32_t $s1_1;
    
    switch (arg1)
    {
        case 0x432:
        {
            $a1_1 = &rdns_out_opt_array;
            $s1_1 = 4;
            break;
        }
        case 0x433:
        {
            $a1_1 = &rdns_awb_gain_par_cfg_array;
            $s1_1 = 0x10;
            break;
        }
        case 0x434:
        {
            $a1_1 = &rdns_oe_num_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x435:
        {
            $a1_1 = &rdns_opt_cfg_array;
            $s1_1 = 0x14;
            break;
        }
        case 0x436:
        {
            $a1_1 = &rdns_gray_stren_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x437:
        {
            $a1_1 = &rdns_slope_par_cfg_array;
            $s1_1 = 8;
            break;
        }
        case 0x438:
        {
            $a1_1 = &rdns_gray_std_thres_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x439:
        {
            $a1_1 = &rdns_text_base_thres_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x43a:
        {
            $a1_1 = &rdns_filter_sat_thres_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x43b:
        {
            $a1_1 = &rdns_oe_thres_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x43c:
        {
            $a1_1 = &rdns_flat_g_thres_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x43d:
        {
            $a1_1 = &rdns_text_g_thres_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x43e:
        {
            $a1_1 = &rdns_flat_rb_thres_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x43f:
        {
            $a1_1 = &rdns_text_rb_thres_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x440:
        {
            $a1_1 = &rdns_gray_np_array;
            $s1_1 = 0x20;
            break;
        }
        case 0x441:
        {
            $a1_1 = &sdns_aa_mv_det_opt;
            $s1_1 = 0x40;
            break;
        }
        case 0x442:
        {
            $a1_1 = &rdns_lum_np_array;
            $s1_1 = 0x40;
            break;
        }
        case 0x443:
        {
            $a1_1 = &rdns_std_np_array;
            $s1_1 = 0x40;
            break;
        }
        case 0x444:
        {
            $a1_1 = &rdns_mv_text_thres_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x445:
        {
            $a1_1 = &rdns_text_base_thres_wdr_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x446:
        {
            $a1_1 = &rdns_sl_par_cfg;
            $s1_1 = 8;
            break;
        }
    }
    
    memcpy(arg2, $a1_1, $s1_1);
    *arg3 = $s1_1;
    return 0;
}

