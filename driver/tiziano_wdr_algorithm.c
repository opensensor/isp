#include "include/main.h"


  int32_t tiziano_wdr_algorithm()

{
    uint32_t wdr_ev_now_1 = wdr_ev_now;
    char* $v1 = (char*)(&param_multiValueHigh_software_in_array); // Fixed void pointer assignment
    char* $a0 = (char*)(&param_multiValueLow_software_in_array); // Fixed void pointer assignment
    int32_t wdr_ev_list_deghost_1 = wdr_ev_list_deghost;
    int32_t $t5 = data_b1bcc;
    int32_t $t1 = data_b1c34;
    int32_t $v0 = data_b148c;
    int32_t $t6 = wdr_ev_now_1 - wdr_ev_list_deghost_1;
    int32_t $a1 = wdr_ev_list_deghost_1 - $v0;
    wchar_t* $a2_1 = U"JRZx";
    int32_t $a3 = wdr_ev_list_deghost_1 < wdr_ev_now_1 ? 1 : 0;
    int32_t i = 0;
    int32_t $t2 = wdr_ev_now_1 < $v0 ? 1 : 0;
            wchar_t $v0_4;
                int32_t $t0_1 = *$a0;
                int32_t $v0_5 = *$v1;
    tisp_wdr_expTime_updata();
    tisp_wdr_ev_calculate();
    
    if ($v0 >= wdr_ev_list_deghost_1)
        $a1 = $v0 - wdr_ev_list_deghost_1;
    
    
    do
    {
        if ((uintptr_t)i != 0x1a)
        {
            
            if (!$a3)
                $v0_4 = *$a0;
            else if ($t2)
            {
                
                $v0_4 = $v0_5 >= $t0_1 ? ($v0_5 - $t0_1) * $t6 / $a1 + $t0_1
                    : $t0_1 - ($t0_1 - $v0_5) * $t6 / $a1;
            }
            else
                $v0_4 = *$v1;
            
            *$a2_1 = $v0_4;
        }
        else if (!$a3)
            data_b16a8_1 = $t1;
        else if ($t2)
        {
            int32_t $v0_2;
            
            $v0_2 = $t5 >= $t1 ? ($t5 - $t1) * $t6 / $a1 + $t1 : $t1 - ($t1 - $t5) * $t6 / $a1;
            
            data_b16a8 = $v0_2;
        }
        else
            data_b16a8_2 = $t5;
        
        i += 1;
        $a0 += 4;
        $a2_1 = &$a2_1[1];
        $v1 += 4;
    } while ((uintptr_t)i != 0x1b);
    
    data_b1e54_1 = data_b1ff8_2;
    TizianoWdrFpgaStructMe = &param_computerModle_software_in_array;
    data_d94a8_1 = &param_xy_pix_low_software_in_array;
    data_d94ac_1 = &param_motionThrPara_software_in_array;
    data_d94b0_1 = &param_d_thr_normal_software_in_array;
    data_d94b4_1 = &param_d_thr_normal1_software_in_array;
    data_d94b8_1 = &param_d_thr_normal2_software_in_array;
    data_d94bc_1 = &param_d_thr_normal_min_software_in_array;
    data_d94c0_1 = &param_d_thr_2_software_in_array;
    data_d94cc_1 = &wdr_hist_R0;
    data_d94d0_1 = &wdr_hist_G0;
    data_d94d4_1 = &wdr_hist_B0;
    data_d94d8_1 = &mdns_y_ass_wei_adj_value1_intp;
    data_d94dc_1 = &mdns_c_false_edg_thres1_intp;
    data_d94e0_1 = &wdr_hist_B1;
    data_d94e4_1 = &wdr_mapR_software_out;
    data_d94e8_1 = &wdr_mapB_software_out;
    data_d94ec_1 = &wdr_mapG_software_out;
    data_d94f0_1 = &param_wdr_thrLable_array;
    data_d949c_1 = &param_x_thr_software_in_array;
    data_d94f4_1 = &wdr_thrLableN_software_out;
    data_d9494_1 = &param_deviationPara_software_in_array;
    data_d94a0_1 = &param_y_thr_software_in_array;
    data_d94fc_1 = &wdr_thrRangeK_software_out;
    data_d94c4_1 = &param_multiValueLow_software_in_array;
    data_d94a4_1 = &param_thrPara_software_in_array;
    data_d9500_1 = &param_wdr_detial_para_software_in_array;
    data_d9498_1 = &param_ratioPara_software_in_array;
    data_d94c8_1 = &param_multiValueHigh_software_in_array;
    data_d94f8_1 = U"JRZx";
    data_d9504_1 = &wdr_detial_para_software_out;
    
    for (int32_t i_1 = 0; (uintptr_t)i_1 < 0x68; i_1 += 1)
    {
        char var_80[0x68];
        var_80[i_1] = *(&data_d94a0 + i_1);
    }
    
    Tiziano_wdr_fpga(TizianoWdrFpgaStructMe, data_d9494_2, data_d9498_2, data_d949c_2);
    
    if (param_wdr_tool_control_array == 1)
        data_b1ff8_3 = 0;
    
    uint32_t $lo_5 = (data_b1ee8_1 << 0xc) / (param_ratioPara_software_in_array + 1);
    int32_t $a2_5 = data_b15a8_1;
    wdr_exp_ratio_def = $lo_5;
    data_b15a0_1 = $lo_5;
    
    if ($a2_5 == 1)
        wdr_exp_ratio_def = wdr_s2l_ratio;
    
    uint32_t wdr_exp_ratio_def_1 = wdr_exp_ratio_def;
    int32_t $a1_4 = data_b1598_1;
    data_b15a4_1 = wdr_exp_ratio_def_1;
    wdr_detial_para_software_out = 0;
    data_b15bc_1 = 0;
    data_b15c8_1 = 0;
    data_b15b4_1 = 0;
    data_b15c0_1 = 0;
    data_b15cc_1 = 0;
    
    if ($a1_4 == 1)
        wdr_exp_ratio_def_1 -= data_b159c_1;
    
    data_b15b8_1 = wdr_exp_ratio_def_1;
    data_b15c4_1 = wdr_exp_ratio_def_1;
    data_b15d0_1 = wdr_exp_ratio_def_1;
    int32_t* $v0_17;
    
    for (int32_t i_2 = 0; (uintptr_t)i_2 != 0x20; )
    {
        char* $v0_16 = (char*)(&wdr_block_mean1_max + i_2); // Fixed void pointer assignment
        i_2 += 4;
        *$v0_16 = 0;
        $v0_17 = &wdr_block_mean1_max;
    }
    
    int32_t $t5_1 = data_d951c_1;
    int32_t $t2_1 = data_d9520_1;
    int32_t $t1_1 = data_d9524_1;
    int32_t $t0_2 = data_d9528_1;
    int32_t i_3 = 0;
    char* $v1_6 = (char*)(&wdr_block_mean1); // Fixed void pointer assignment
    
    do
    {
        int32_t $v1_7 = *$v1_6;
                int32_t $s0_2 = *(&wdr_block_mean1 + j);
                char* $t9_1 = (char*)(&wdr_block_mean1_max + j); // Fixed void pointer assignment
        
        if (wdr_block_mean1_max < $v1_7)
        {
            for (int32_t j = 0; (uintptr_t)j != 0x1c; )
            {
                j += 4;
                *((int32_t*)((char*)$t9_1 + 4)) = $s0_2; // Fixed void pointer dereference
            }
            
            wdr_block_mean1_max = $v1_7;
        }
        else if (data_d7210_1 < $v1_7)
        {
                int32_t $s0_4 = *(j_1 + 0xd9514);
                char* $t9_2 = (char*)(&wdr_block_mean1_max + j_1); // Fixed void pointer assignment
            for (int32_t j_1 = 0; (uintptr_t)j_1 != 0x18; )
            {
                j_1 += 4;
                *((int32_t*)((char*)$t9_2 + 8)) = $s0_4; // Fixed void pointer dereference
            }
            
            data_d7210_2 = $v1_7;
        }
        else if (data_d7214_1 < $v1_7)
        {
                int32_t $s0_6 = *(j_2 + 0xd9518);
                char* $t9_3 = (char*)(&wdr_block_mean1_max + j_2); // Fixed void pointer assignment
            for (int32_t j_2 = 0; (uintptr_t)j_2 != 0x14; )
            {
                j_2 += 4;
                *((int32_t*)((char*)$t9_3 + 0xc)) = $s0_6; // Fixed void pointer dereference
            }
            
            data_d7214_2 = $v1_7;
        }
        else if (data_d7218_1 < $v1_7)
        {
            data_d721c = $t5_1;
            data_d7220 = $t2_1;
            data_d7224 = $t1_1;
            data_d7228 = $t0_2;
            data_d7218 = $v1_7;
        }
        else if (data_d721c_1 < $v1_7)
        {
            data_d7220 = $t2_1;
            data_d7224 = $t1_1;
            data_d7228 = $t0_2;
            data_d721c = $v1_7;
        }
        else if (data_d7220_1 < $v1_7)
        {
            data_d7224 = $t1_1;
            data_d7228 = $t0_2;
            data_d7220 = $v1_7;
        }
        else if (data_d7224_1 < $v1_7)
        {
            data_d7228 = $t0_2;
            data_d7224 = $v1_7;
        }
        else if (data_d7228_1 < $v1_7)
            data_d7228_2 = $v1_7;
        
        i_3 += 4;
        $v1_6 = &wdr_block_mean1 + i_3;
    } while ((uintptr_t)i_3 != 0x384);
    
    int32_t $v1_8 = data_d9080_1;
    wdr_block_mean1_end = 0;
    int32_t $t0_3;
    
    if ($v1_8 < 4)
    {
        data_d9080 = 4;
        $t0_3 = data_d9080;
    }
    else if ($v1_8 < 9)
        $t0_3 = data_d9080_2;
    else
    {
        data_d9080 = 8;
        $t0_3 = data_d9080;
    }
    
    int32_t $v1_11 = 0;
    uint32_t wdr_block_mean1_end_2 = 0;
    int32_t $a1_21 = 0;
    
    while ($a1_21 != $t0_3)
    {
        $a1_21 += 1;
        wdr_block_mean1_end_2 += *$v0_17;
        $v0_17 = &$v0_17[1];
        $v1_11 = 1;
    }
    
    uint32_t wdr_block_mean1_end_1 = wdr_block_mean1_end;
    
    if ($v1_11)
        wdr_block_mean1_end_1 = wdr_block_mean1_end_2;
    
    uint32_t $lo_6 = wdr_block_mean1_end_1 / $a1_21;
    wdr_block_mean1_end = $lo_6;
    uint32_t wdr_block_mean1_end_old_1 = wdr_block_mean1_end_old;
    data_d92fc_1 = $lo_6;
    uint32_t $v1_13 = $lo_6 - wdr_block_mean1_end_old_1;
    wdr_block_mean1_th = $v1_13;
    
    if ($v1_13 <= 0)
    {
            int32_t $v1_15 = -($v1_13);
            int32_t $t0_5 = data_d9078;
        if (!$v1_13)
            wdr_block_mean1_end_old = $lo_6;
        else if (data_d9074 != 1)
            wdr_block_mean1_end_old = $lo_6;
        else
        {
            wdr_block_mean1_th = $v1_15;
            
            if ($t0_5 >= $v1_15)
                wdr_block_mean1_end_old = $lo_6;
            else
                wdr_block_mean1_end_old = wdr_block_mean1_end_old_1 - $t0_5;
        }
    }
    else if (data_d9074_1 != 1)
        wdr_block_mean1_end_old = $lo_6;
    else
    {
        int32_t $t0_4 = data_d9078;
        
        if ($t0_4 < $v1_13)
            wdr_block_mean1_end_old = wdr_block_mean1_end_old_1 + $t0_4;
        else
            wdr_block_mean1_end_old = $lo_6;
    }
    
    if (param_wdr_gam_y_array == 2 && data_b15ac_2 == 1)
        tiziano_wdr_fusion1_curve_block_mean1();
    
    return 0;
}

