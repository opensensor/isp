#include "include/main.h"


  int32_t tisp_wdr_param_array_set(int32_t arg1, int32_t* arg2, int32_t* arg3)

{
        int32_t var_10_1 = arg1;
    if (arg1 - (uintptr_t)0x3ff >= 0x33)
    {
        isp_printf(); // Fixed: macro call, removed arguments;
        return 0xffffffff;
    }
    
    int32_t $v0_2;
    
    switch (arg1)
    {
        case 0x3ff:
        {
            memcpy(&param_wdr_para_array, arg2, 0x28);
            $v0_2 = 0x28;
            break;
        }
        case 0x400:
        {
            memcpy(&mdns_c_luma_wei_adj_value0_array);
            $v0_2 = 0x80;
            break;
        }
        case 0x401:
        {
            memcpy(&param_wdr_weightLUT02_array);
            $v0_2 = 0x80;
            break;
        }
        case 0x402:
        {
            memcpy(&param_wdr_weightLUT12_array);
            $v0_2 = 0x80;
            break;
        }
        case 0x403:
        {
            memcpy(&param_wdr_weightLUT22_array);
            $v0_2 = 0x80;
            break;
        }
        case 0x404:
        {
            memcpy(&param_wdr_weightLUT21_array);
            $v0_2 = 0x80;
            break;
        }
        case 0x405:
        {
            memcpy(&param_wdr_gam_y_array);
            $v0_2 = 0x84;
            break;
        }
        case 0x406:
        {
            memcpy(&param_wdr_w_point_weight_x_array);
            $v0_2 = 0x10;
            break;
        }
        case 0x407:
        {
            memcpy(&param_wdr_w_point_weight_y_array);
            $v0_2 = 0x10;
            break;
        }
        case 0x408:
        {
            memcpy(&param_wdr_w_point_weight_pow_array, arg2, 0xc);
            $v0_2 = 0xc;
            break;
        }
        case 0x409:
        {
            memcpy(U"#',17=DKS[clu~");
            $v0_2 = 0x84;
            break;
        }
        case 0x40a:
        {
            memcpy(&param_wdr_detail_th_w_array);
            $v0_2 = 0x1c;
            break;
        }
        case 0x40b:
        {
            memcpy(&param_wdr_contrast_t_y_mux_array);
            $v0_2 = 0x14;
            break;
        }
        case 0x40c:
        {
            memcpy(&param_wdr_ct_cl_para_array);
            $v0_2 = 0x10;
            break;
        }
        case 0x40d:
        {
            memcpy(&param_centre5x5_w_distance_array, arg2, 0x7c);
            $v0_2 = 0x7c;
            break;
        }
        case 0x40e:
        {
            memcpy(&param_wdr_stat_para_array);
            $v0_2 = 0x1c;
            break;
        }
        case 0x40f:
        {
            memcpy(&param_wdr_degost_para_array, arg2, 0x34);
            $v0_2 = 0x34;
            break;
        }
        case 0x410:
        {
            memcpy(&param_wdr_darkLable_array);
            $v0_2 = 0x14;
            break;
        }
        case 0x411:
        {
            memcpy(&param_wdr_darkLableN_array);
            $v0_2 = 0x10;
            break;
        }
        case 0x412:
        {
            memcpy(&param_wdr_darkWeight_array);
            $v0_2 = 0x14;
            break;
        }
        case 0x413:
        {
            memcpy(&param_wdr_thrLable_array);
            $v0_2 = 0x6c;
            break;
        }
        case 0x414:
        {
            memcpy(&param_computerModle_software_in_array);
            $v0_2 = 0x10;
            break;
        }
        case 0x415:
        {
            memcpy(&param_deviationPara_software_in_array);
            $v0_2 = 0x14;
            break;
        }
        case 0x416:
        {
            memcpy(&param_ratioPara_software_in_array);
            $v0_2 = 0x1c;
            break;
        }
        case 0x417:
        {
            memcpy(&param_x_thr_software_in_array);
            $v0_2 = 0x10;
            break;
        }
        case 0x418:
        {
            memcpy(&param_y_thr_software_in_array);
            $v0_2 = 0x10;
            break;
        }
        case 0x419:
        {
            memcpy(&param_thrPara_software_in_array, arg2, 0x50);
            $v0_2 = 0x50;
            break;
        }
        case 0x41a:
        {
            memcpy(&param_xy_pix_low_software_in_array, arg2, 0x58);
            $v0_2 = 0x58;
            break;
        }
        case 0x41b:
        {
            memcpy(&param_motionThrPara_software_in_array, arg2, 0x44);
            $v0_2 = 0x44;
            break;
        }
        case 0x41c:
        {
            memcpy(&param_d_thr_normal_software_in_array);
            $v0_2 = 0x68;
            break;
        }
        case 0x41d:
        {
            memcpy(&param_d_thr_normal1_software_in_array);
            $v0_2 = 0x68;
            break;
        }
        case 0x41e:
        {
            memcpy(&param_d_thr_normal2_software_in_array);
            $v0_2 = 0x68;
            break;
        }
        case 0x41f:
        {
            memcpy(&param_d_thr_normal_min_software_in_array);
            $v0_2 = 0x68;
            break;
        }
        case 0x420:
        {
            memcpy(&param_multiValueLow_software_in_array);
            $v0_2 = 0x68;
            break;
        }
        case 0x421:
        {
            memcpy(&param_multiValueHigh_software_in_array);
            $v0_2 = 0x68;
            break;
        }
        case 0x422:
        {
            memcpy(&param_d_thr_2_software_in_array);
            $v0_2 = 0x68;
            break;
        }
        case 0x423:
        {
            memcpy(&param_wdr_detial_para_software_in_array, arg2, 0x20);
            $v0_2 = 0x20;
            break;
        }
        case 0x424:
        {
            memcpy(U"JRZx");
            $v0_2 = 0x6c;
            break;
        }
        case 0x425:
        {
            memcpy(&param_wdr_dbg_out_array, arg2, 8);
            $v0_2 = 8;
            break;
        }
        case 0x426:
        {
            memcpy(&wdr_ev_list);
            $v0_2 = 0x24;
            break;
        }
        case 0x427:
        {
            memcpy(&wdr_weight_b_in_list);
            $v0_2 = 0x24;
            break;
        }
        case 0x428:
        {
            memcpy(&wdr_weight_p_in_list);
            $v0_2 = 0x24;
            break;
        }
        case 0x429:
        {
            memcpy(&wdr_ev_list_deghost);
            $v0_2 = 0x24;
            break;
        }
        case 0x42a:
        {
            memcpy(&wdr_weight_in_list_deghost);
            $v0_2 = 0x24;
            break;
        }
        case 0x42b:
        {
            memcpy(&wdr_detail_w_in0_list);
            $v0_2 = 0x24;
            break;
        }
        case 0x42c:
        {
            memcpy(&wdr_detail_w_in1_list);
            $v0_2 = 0x24;
            break;
        }
        case 0x42d:
        {
            memcpy(&wdr_detail_w_in4_list[0x12]);
            $v0_2 = 0x24;
            break;
        }
        case 0x42e:
        {
            memcpy(&wdr_detail_w_in4_list[9]);
            $v0_2 = 0x24;
            break;
        }
        case 0x42f:
        {
            memcpy(U"@@@@@@@@@@@@@@@@@@         ");
            $v0_2 = 0x24;
            break;
        }
        case 0x430:
        {
            memcpy(&mdns_y_fspa_ref_fus_wei_224_wdr_array, arg2, 0x40);
            $v0_2 = 0x40;
            break;
        }
        case 0x431:
        {
            int32_t* $v0_3 = &param_wdr_tool_control_array;
                int32_t i_1 = i;
            
            for (int32_t i = 0; (uintptr_t)i != 0xe; )
            {
                i += 1;
                
                if (i_1 != 2)
                    *$v0_3 = *arg2;
                
                arg2 = &arg2[1];
                $v0_3 = &$v0_3[1];
            }
            
            tiziano_wdr_params_init();
            wdr_ev_changed = 1;
            $v0_2 = 0x38;
            break;
        }
    }
    
    *arg3 = $v0_2;
    return 0;
}

