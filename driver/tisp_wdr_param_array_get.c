#include "include/main.h"


  int32_t tisp_wdr_param_array_get(int32_t arg1, int32_t arg2, int32_t* arg3)

{
        int32_t var_18_1 = arg1;
        return 0xffffffff;
    if (arg1 - (uintptr_t)0x3ff >= 0x33)
    {

    }
    
    void* $a1_1;
    int32_t $s1_1;
    
    switch (arg1)
    {
        case 0x3ff:
        {
            $a1_1 = &param_wdr_para_array;
            $s1_1 = 0x28;
            break;
        }
        case 0x400:
        {
            $a1_1 = &mdns_c_luma_wei_adj_value0_array;
            $s1_1 = 0x80;
            break;
        }
        case 0x401:
        {
            $a1_1 = &param_wdr_weightLUT02_array;
            $s1_1 = 0x80;
            break;
        }
        case 0x402:
        {
            $a1_1 = &param_wdr_weightLUT12_array;
            $s1_1 = 0x80;
            break;
        }
        case 0x403:
        {
            $a1_1 = &param_wdr_weightLUT22_array;
            $s1_1 = 0x80;
            break;
        }
        case 0x404:
        {
            $a1_1 = &param_wdr_weightLUT21_array;
            $s1_1 = 0x80;
            break;
        }
        case 0x405:
        {
            $a1_1 = &param_wdr_gam_y_array;
            $s1_1 = 0x84;
            break;
        }
        case 0x406:
        {
            $a1_1 = &param_wdr_w_point_weight_x_array;
            $s1_1 = 0x10;
            break;
        }
        case 0x407:
        {
            $a1_1 = &param_wdr_w_point_weight_y_array;
            $s1_1 = 0x10;
            break;
        }
        case 0x408:
        {
            $a1_1 = &param_wdr_w_point_weight_pow_array;
            $s1_1 = 0xc;
            break;
        }
        case 0x409:
        {
            $a1_1 = U"#',17=DKS[clu~";
            $s1_1 = 0x84;
            break;
        }
        case 0x40a:
        {
            $a1_1 = &param_wdr_detail_th_w_array;
            $s1_1 = 0x1c;
            break;
        }
        case 0x40b:
        {
            $a1_1 = &param_wdr_contrast_t_y_mux_array;
            $s1_1 = 0x14;
            break;
        }
        case 0x40c:
        {
            $a1_1 = &param_wdr_ct_cl_para_array;
            $s1_1 = 0x10;
            break;
        }
        case 0x40d:
        {
            $a1_1 = &param_centre5x5_w_distance_array;
            $s1_1 = 0x7c;
            break;
        }
        case 0x40e:
        {
            $a1_1 = &param_wdr_stat_para_array;
            $s1_1 = 0x1c;
            break;
        }
        case 0x40f:
        {
            $a1_1 = &param_wdr_degost_para_array;
            $s1_1 = 0x34;
            break;
        }
        case 0x410:
        {
            $a1_1 = &param_wdr_darkLable_array;
            $s1_1 = 0x14;
            break;
        }
        case 0x411:
        {
            $a1_1 = &param_wdr_darkLableN_array;
            $s1_1 = 0x10;
            break;
        }
        case 0x412:
        {
            $a1_1 = &param_wdr_darkWeight_array;
            $s1_1 = 0x14;
            break;
        }
        case 0x413:
        {
            $a1_1 = &param_wdr_thrLable_array;
            $s1_1 = 0x6c;
            break;
        }
        case 0x414:
        {
            $a1_1 = &param_computerModle_software_in_array;
            $s1_1 = 0x10;
            break;
        }
        case 0x415:
        {
            $a1_1 = &param_deviationPara_software_in_array;
            $s1_1 = 0x14;
            break;
        }
        case 0x416:
        {
            $a1_1 = &param_ratioPara_software_in_array;
            $s1_1 = 0x1c;
            break;
        }
        case 0x417:
        {
            $a1_1 = &param_x_thr_software_in_array;
            $s1_1 = 0x10;
            break;
        }
        case 0x418:
        {
            $a1_1 = &param_y_thr_software_in_array;
            $s1_1 = 0x10;
            break;
        }
        case 0x419:
        {
            $a1_1 = &param_thrPara_software_in_array;
            $s1_1 = 0x50;
            break;
        }
        case 0x41a:
        {
            $a1_1 = &param_xy_pix_low_software_in_array;
            $s1_1 = 0x58;
            break;
        }
        case 0x41b:
        {
            $a1_1 = &param_motionThrPara_software_in_array;
            $s1_1 = 0x44;
            break;
        }
        case 0x41c:
        {
            $a1_1 = &param_d_thr_normal_software_in_array;
            $s1_1 = 0x68;
            break;
        }
        case 0x41d:
        {
            $a1_1 = &param_d_thr_normal1_software_in_array;
            $s1_1 = 0x68;
            break;
        }
        case 0x41e:
        {
            $a1_1 = &param_d_thr_normal2_software_in_array;
            $s1_1 = 0x68;
            break;
        }
        case 0x41f:
        {
            $a1_1 = &param_d_thr_normal_min_software_in_array;
            $s1_1 = 0x68;
            break;
        }
        case 0x420:
        {
            $a1_1 = &param_multiValueLow_software_in_array;
            $s1_1 = 0x68;
            break;
        }
        case 0x421:
        {
            $a1_1 = &param_multiValueHigh_software_in_array;
            $s1_1 = 0x68;
            break;
        }
        case 0x422:
        {
            $a1_1 = &param_d_thr_2_software_in_array;
            $s1_1 = 0x68;
            break;
        }
        case 0x423:
        {
            $a1_1 = &param_wdr_detial_para_software_in_array;
            $s1_1 = 0x20;
            break;
        }
        case 0x424:
        {
            $a1_1 = U"JRZx";
            $s1_1 = 0x6c;
            break;
        }
        case 0x425:
        {
            $a1_1 = &param_wdr_dbg_out_array;
            $s1_1 = 8;
            break;
        }
        case 0x426:
        {
            $a1_1 = &wdr_ev_list;
            $s1_1 = 0x24;
            break;
        }
        case 0x427:
        {
            $a1_1 = &wdr_weight_b_in_list;
            $s1_1 = 0x24;
            break;
        }
        case 0x428:
        {
            $a1_1 = &wdr_weight_p_in_list;
            $s1_1 = 0x24;
            break;
        }
        case 0x429:
        {
            $a1_1 = &wdr_ev_list_deghost;
            $s1_1 = 0x24;
            break;
        }
        case 0x42a:
        {
            $a1_1 = &wdr_weight_in_list_deghost;
            $s1_1 = 0x24;
            break;
        }
        case 0x42b:
        {
            $a1_1 = &wdr_detail_w_in0_list;
            $s1_1 = 0x24;
            break;
        }
        case 0x42c:
        {
            $a1_1 = &wdr_detail_w_in1_list;
            $s1_1 = 0x24;
            break;
        }
        case 0x42d:
        {
            $a1_1 = &wdr_detail_w_in4_list[0x12];
            $s1_1 = 0x24;
            break;
        }
        case 0x42e:
        {
            $a1_1 = &wdr_detail_w_in4_list[9];
            $s1_1 = 0x24;
            break;
        }
        case 0x42f:
        {
            $a1_1 = U"@@@@@@@@@@@@@@@@@@         ";
            $s1_1 = 0x24;
            break;
        }
        case 0x430:
        {
            $a1_1 = &mdns_y_fspa_ref_fus_wei_224_wdr_array;
            $s1_1 = 0x40;
            break;
        }
        case 0x431:
        {
            $a1_1 = &param_wdr_tool_control_array;
            $s1_1 = 0x38;
            break;
        }
    }
    
    memcpy(arg2, $a1_1, $s1_1);
    *arg3 = $s1_1;
    return 0;
}

