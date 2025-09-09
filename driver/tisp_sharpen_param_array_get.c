#include "include/main.h"


  int32_t tisp_sharpen_param_array_get(int32_t arg1, int32_t arg2, int32_t* arg3)

{
        int32_t var_18_1 = arg1;
    if (arg1 - (uintptr_t)0xb5 >= 0x31)
    {
        isp_printf(); // Fixed: macro call, removed arguments;
        return 0xffffffff;
    }
    
    void* $a1_1;
    int32_t $s1_1;
    
    switch (arg1)
    {
        case 0xb5:
        {
            $a1_1 = &y_sp_out_opt_array;
            $s1_1 = 4;
            break;
        }
        case 0xb6:
        {
            $a1_1 = &y_sp_sl_exp_thres_array;
            $s1_1 = 0x24;
            break;
        }
        case 0xb7:
        {
            $a1_1 = &y_sp_sl_exp_num_array;
            $s1_1 = 0x24;
            break;
        }
        case 0xb8:
        {
            $a1_1 = &y_sp_std_cfg_array;
            $s1_1 = 8;
            break;
        }
        case 0xb9:
        {
            $a1_1 = &y_sp_uu_min_stren_array;
            $s1_1 = 0x24;
            break;
        }
        case 0xba:
        {
            $a1_1 = &y_sp_uu_min_thres_array;
            $s1_1 = 0x24;
            break;
        }
        case 0xbb:
        {
            $a1_1 = &y_sp_uu_thres_array;
            $s1_1 = 0x24;
            break;
        }
        case 0xbc:
        {
            $a1_1 = &y_sp_mv_uu_thres_array;
            $s1_1 = 0x24;
            break;
        }
        case 0xbd:
        {
            $a1_1 = &y_sp_mv_uu_stren_array;
            $s1_1 = 0x24;
            break;
        }
        case 0xbe:
        {
            $a1_1 = &y_sp_uu_stren_array;
            $s1_1 = 0x24;
            break;
        }
        case 0xbf:
        {
            $a1_1 = &y_sp_uu_par_cfg_array;
            $s1_1 = 0x10;
            break;
        }
        case 0xc0:
        {
            $a1_1 = &y_sp_fl_std_thres_array;
            $s1_1 = 0x24;
            break;
        }
        case 0xc1:
        {
            $a1_1 = &y_sp_mv_fl_std_thres_array;
            $s1_1 = 0x24;
            break;
        }
        case 0xc2:
        {
            $a1_1 = &y_sp_fl_thres_array;
            $s1_1 = 0x24;
            break;
        }
        case 0xc3:
        {
            $a1_1 = &y_sp_fl_min_thres_array;
            $s1_1 = 0x24;
            break;
        }
        case 0xc4:
        {
            $a1_1 = &y_sp_mv_fl_thres_array;
            $s1_1 = 0x24;
            break;
        }
        case 0xc5:
        {
            $a1_1 = &y_sp_mv_fl_min_thres_array;
            $s1_1 = 0x24;
            break;
        }
        case 0xc6:
        {
            $a1_1 = &y_sp_fl_par_cfg_array;
            $s1_1 = 8;
            break;
        }
        case 0xc7:
        {
            $a1_1 = &y_sp_v2_win5_thres_array;
            $s1_1 = 0x24;
            break;
        }
        case 0xc8:
        {
            $a1_1 = &y_sp_v1_v2_coef_par_cfg_array;
            $s1_1 = 0x30;
            break;
        }
        case 0xc9:
        {
            $a1_1 = &y_sp_w_b_ll_par_cfg_array;
            $s1_1 = 0x24;
            break;
        }
        case 0xca:
        {
            $a1_1 = &y_sp_uu_np_array;
            $s1_1 = 0x40;
            break;
        }
        case 0xcb:
        {
            $a1_1 = &y_sp_w_wei_np_array;
            $s1_1 = 0x40;
            break;
        }
        case 0xcc:
        {
            $a1_1 = &y_sp_b_wei_np_array;
            $s1_1 = 0x40;
            break;
        }
        case 0xcd:
        {
            $a1_1 = &y_sp_w_sl_stren_0_array;
            $s1_1 = 0x24;
            break;
        }
        case 0xce:
        {
            $a1_1 = &y_sp_w_sl_stren_1_array;
            $s1_1 = 0x24;
            break;
        }
        case 0xcf:
        {
            $a1_1 = &y_sp_w_sl_stren_2_array;
            $s1_1 = 0x24;
            break;
        }
        case 0xd0:
        {
            $a1_1 = &y_sp_w_sl_stren_3_array;
            $s1_1 = 0x24;
            break;
        }
        case 0xd1:
        {
            $a1_1 = &y_sp_b_sl_stren_0_array;
            $s1_1 = 0x24;
            break;
        }
        case 0xd2:
        {
            $a1_1 = &y_sp_b_sl_stren_1_array;
            $s1_1 = 0x24;
            break;
        }
        case 0xd3:
        {
            $a1_1 = &y_sp_b_sl_stren_2_array;
            $s1_1 = 0x24;
            break;
        }
        case 0xd4:
        {
            $a1_1 = &y_sp_b_sl_stren_3_array;
            $s1_1 = 0x24;
            break;
        }
        case 0xd5:
        {
            $a1_1 = &awb_dn_refresh_flag;
            $s1_1 = 0x24;
            break;
        }
        case 0xd6:
        {
            $a1_1 = &DumpNum.32174;
            $s1_1 = 0x24;
            break;
        }
        case 0xd7:
        {
            $a1_1 = &y_sp_uu_sl_2_array;
            $s1_1 = 0x24;
            break;
        }
        case 0xd8:
        {
            $a1_1 = &y_sp_uu_sl_3_array;
            $s1_1 = 0x24;
            break;
        }
        case 0xd9:
        {
            $a1_1 = &IspAwbCtDetectParam;
            $s1_1 = 0x24;
            break;
        }
        case 0xda:
        {
            $a1_1 = &y_sp_fl_sl_1_array;
            $s1_1 = 0x24;
            break;
        }
        case 0xdb:
        {
            $a1_1 = &y_sp_fl_sl_2_array;
            $s1_1 = 0x24;
            break;
        }
        case 0xdc:
        {
            $a1_1 = &y_sp_uu_thres_wdr_array;
            $s1_1 = 0x24;
            break;
        }
        case 0xdd:
        {
            $a1_1 = &y_sp_w_sl_stren_0_wdr_array;
            $s1_1 = 0x24;
            break;
        }
        case 0xde:
        {
            $a1_1 = &y_sp_w_sl_stren_1_wdr_array;
            $s1_1 = 0x24;
            break;
        }
        case 0xdf:
        {
            $a1_1 = &y_sp_w_sl_stren_2_wdr_array;
            $s1_1 = 0x24;
            break;
        }
        case 0xe0:
        {
            $a1_1 = &y_sp_w_sl_stren_3_wdr_array;
            $s1_1 = 0x24;
            break;
        }
        case 0xe1:
        {
            $a1_1 = &y_sp_b_sl_stren_0_wdr_array;
            $s1_1 = 0x24;
            break;
        }
        case 0xe2:
        {
            $a1_1 = &y_sp_b_sl_stren_1_wdr_array;
            $s1_1 = 0x24;
            break;
        }
        case 0xe3:
        {
            $a1_1 = &y_sp_b_sl_stren_2_wdr_array;
            $s1_1 = 0x24;
            break;
        }
        case 0xe4:
        {
            $a1_1 = &y_sp_b_sl_stren_3_wdr_array;
            $s1_1 = 0x24;
            break;
        }
        case 0xe5:
        {
            $a1_1 = &y_sp_fl_sl_3_array;
            $s1_1 = 0x24;
            break;
        }
    }
    
    memcpy(arg2, $a1_1, $s1_1);
    *arg3 = $s1_1;
    return 0;
}

