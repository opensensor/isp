#include "include/main.h"


  int32_t tisp_sharpen_param_array_set(int32_t arg1, int32_t arg2, int32_t* arg3)

{
        int32_t var_10_1 = arg1;
        return 0xffffffff;
    if (arg1 - (uintptr_t)0xb5 >= 0x31)
    {

    }
    
    int32_t $v0_1;
    
    switch (arg1)
    {
        case 0xb5:
        {
            memcpy(&y_sp_out_opt_array, arg2, 4);
            $v0_1 = 4;
            break;
        }
        case 0xb6:
        {
            memcpy(&y_sp_sl_exp_thres_array);
            $v0_1 = 0x24;
            break;
        }
        case 0xb7:
        {
            memcpy(&y_sp_sl_exp_num_array);
            $v0_1 = 0x24;
            break;
        }
        case 0xb8:
        {
            memcpy(&y_sp_std_cfg_array);
            $v0_1 = 8;
            break;
        }
        case 0xb9:
        {
            memcpy(&y_sp_uu_min_stren_array);
            $v0_1 = 0x24;
            break;
        }
        case 0xba:
        {
            memcpy(&y_sp_uu_min_thres_array);
            $v0_1 = 0x24;
            break;
        }
        case 0xbb:
        {
            memcpy(&y_sp_uu_thres_array);
            $v0_1 = 0x24;
            break;
        }
        case 0xbc:
        {
            memcpy(&y_sp_mv_uu_thres_array);
            $v0_1 = 0x24;
            break;
        }
        case 0xbd:
        {
            memcpy(&y_sp_mv_uu_stren_array);
            $v0_1 = 0x24;
            break;
        }
        case 0xbe:
        {
            memcpy(&y_sp_uu_stren_array);
            $v0_1 = 0x24;
            break;
        }
        case 0xbf:
        {
            memcpy(&y_sp_uu_par_cfg_array, arg2, 0x10);
            $v0_1 = 0x10;
            break;
        }
        case 0xc0:
        {
            memcpy(&y_sp_fl_std_thres_array);
            $v0_1 = 0x24;
            break;
        }
        case 0xc1:
        {
            memcpy(&y_sp_mv_fl_std_thres_array);
            $v0_1 = 0x24;
            break;
        }
        case 0xc2:
        {
            memcpy(&y_sp_fl_thres_array);
            $v0_1 = 0x24;
            break;
        }
        case 0xc3:
        {
            memcpy(&y_sp_fl_min_thres_array);
            $v0_1 = 0x24;
            break;
        }
        case 0xc4:
        {
            memcpy(&y_sp_mv_fl_thres_array);
            $v0_1 = 0x24;
            break;
        }
        case 0xc5:
        {
            memcpy(&y_sp_mv_fl_min_thres_array);
            $v0_1 = 0x24;
            break;
        }
        case 0xc6:
        {
            memcpy(&y_sp_fl_par_cfg_array);
            $v0_1 = 8;
            break;
        }
        case 0xc7:
        {
            memcpy(&y_sp_v2_win5_thres_array);
            $v0_1 = 0x24;
            break;
        }
        case 0xc8:
        {
            memcpy(&y_sp_v1_v2_coef_par_cfg_array, arg2, 0x30);
            $v0_1 = 0x30;
            break;
        }
        case 0xc9:
        {
            memcpy(&y_sp_w_b_ll_par_cfg_array);
            $v0_1 = 0x24;
            break;
        }
        case 0xca:
        {
            memcpy(&y_sp_uu_np_array);
            $v0_1 = 0x40;
            break;
        }
        case 0xcb:
        {
            memcpy(&y_sp_w_wei_np_array);
            $v0_1 = 0x40;
            break;
        }
        case 0xcc:
        {
            memcpy(&y_sp_b_wei_np_array);
            $v0_1 = 0x40;
            break;
        }
        case 0xcd:
        {
            memcpy(&y_sp_w_sl_stren_0_array);
            $v0_1 = 0x24;
            break;
        }
        case 0xce:
        {
            memcpy(&y_sp_w_sl_stren_1_array);
            $v0_1 = 0x24;
            break;
        }
        case 0xcf:
        {
            memcpy(&y_sp_w_sl_stren_2_array);
            $v0_1 = 0x24;
            break;
        }
        case 0xd0:
        {
            memcpy(&y_sp_w_sl_stren_3_array);
            $v0_1 = 0x24;
            break;
        }
        case 0xd1:
        {
            memcpy(&y_sp_b_sl_stren_0_array);
            $v0_1 = 0x24;
            break;
        }
        case 0xd2:
        {
            memcpy(&y_sp_b_sl_stren_1_array);
            $v0_1 = 0x24;
            break;
        }
        case 0xd3:
        {
            memcpy(&y_sp_b_sl_stren_2_array);
            $v0_1 = 0x24;
            break;
        }
        case 0xd4:
        {
            memcpy(&y_sp_b_sl_stren_3_array);
            $v0_1 = 0x24;
            break;
        }
        case 0xd5:
        {
            memcpy(&awb_dn_refresh_flag);
            $v0_1 = 0x24;
            break;
        }
        case 0xd6:
        {
            memcpy(&DumpNum.32174);
            $v0_1 = 0x24;
            break;
        }
        case 0xd7:
        {
            memcpy(&y_sp_uu_sl_2_array);
            $v0_1 = 0x24;
            break;
        }
        case 0xd8:
        {
            memcpy(&y_sp_uu_sl_3_array);
            $v0_1 = 0x24;
            break;
        }
        case 0xd9:
        {
            memcpy(&IspAwbCtDetectParam);
            $v0_1 = 0x24;
            break;
        }
        case 0xda:
        {
            memcpy(&y_sp_fl_sl_1_array);
            $v0_1 = 0x24;
            break;
        }
        case 0xdb:
        {
            memcpy(&y_sp_fl_sl_2_array);
            $v0_1 = 0x24;
            break;
        }
        case 0xdc:
        {
            memcpy(&y_sp_uu_thres_wdr_array);
            $v0_1 = 0x24;
            break;
        }
        case 0xdd:
        {
            memcpy(&y_sp_w_sl_stren_0_wdr_array);
            $v0_1 = 0x24;
            break;
        }
        case 0xde:
        {
            memcpy(&y_sp_w_sl_stren_1_wdr_array);
            $v0_1 = 0x24;
            break;
        }
        case 0xdf:
        {
            memcpy(&y_sp_w_sl_stren_2_wdr_array);
            $v0_1 = 0x24;
            break;
        }
        case 0xe0:
        {
            memcpy(&y_sp_w_sl_stren_3_wdr_array);
            $v0_1 = 0x24;
            break;
        }
        case 0xe1:
        {
            memcpy(&y_sp_b_sl_stren_0_wdr_array);
            $v0_1 = 0x24;
            break;
        }
        case 0xe2:
        {
            memcpy(&y_sp_b_sl_stren_1_wdr_array);
            $v0_1 = 0x24;
            break;
        }
        case 0xe3:
        {
            memcpy(&y_sp_b_sl_stren_2_wdr_array);
            $v0_1 = 0x24;
            break;
        }
        case 0xe4:
        {
            memcpy(&y_sp_b_sl_stren_3_wdr_array);
            $v0_1 = 0x24;
            break;
        }
        case 0xe5:
        {
            memcpy(&y_sp_fl_sl_3_array);
            $v0_1 = 0x24;
            break;
        }
    }
    
    *arg3 = $v0_1;
    tisp_sharpen_all_reg_refresh();
    return 0;
}

