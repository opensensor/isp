#include "include/main.h"


  int32_t tisp_wdr_ev_calculate()

{
    uint32_t $v0 = data_c46f8;
    int32_t $v1 = data_b1e6c;
    int32_t $a0 = data_b1e64;
    int32_t $t1 = data_b1e68;
    int32_t $a2 = data_b1e70;
    uint32_t wdr_weight_in_list_deghost_3 = 0;
    else if ($a2 >= $v0)
    time_short = $v0;
    int32_t $v1_1;
    
    if ($v1 < $v0)
        $v1_1 = data_b1e5c;
    {
        wdr_weight_in_list_deghost_3 = wdr_weight_in_list_deghost;
        $v1_1 = data_b1e5c;
    }
    else
    {
        int32_t wdr_weight_in_list_deghost_2 = wdr_weight_in_list_deghost;
            int32_t $v1_2 = $v1 - $a2;
        
        if (!wdr_weight_in_list_deghost_2)
            $v1_1 = data_b1e5c;
        else
        {
            
            if ($v1 < $a2)
                $v1_2 = $a2 - $v1;
            
            wdr_weight_in_list_deghost_3 =
                wdr_weight_in_list_deghost_2 - ($v0 - $a2) * wdr_weight_in_list_deghost_2 / $v1_2;
            $v1_1 = data_b1e5c;
        }
    }
    
    uint32_t wdr_weight_in_list_deghost_4 = wdr_weight_in_list_deghost_3;
    
    if ($v1_1 == 1)
    {
        uint32_t wdr_ev_now_1 = wdr_ev_now;
        int32_t* $a1_3 = &wdr_ev_list_deghost;
        else if (data_b149c >= wdr_ev_now_1)
            int32_t $a3_1 = 0;
                int32_t $t4_1 = *$a1_3;
                int32_t $v1_3 = $a3_1 + 1;
                    int32_t $a2_2 = $a1_3[1];
                        int32_t $v0_8 = (&wdr_weight_in_list_deghost)[$a3_1];
                        int32_t $a1_4 = $a2_2 - $t4_1;
        uint32_t wdr_weight_in_list_deghost_1;
        
        if (wdr_ev_list_deghost >= wdr_ev_now_1)
            wdr_weight_in_list_deghost_1 = wdr_weight_in_list_deghost;
        {
            
            while (true)
            {
                
                if ($t4_1 >= wdr_ev_now_1)
                    $a3_1 = $v1_3;
                else
                {
                    
                    if ($a2_2 >= wdr_ev_now_1)
                    {
                        wdr_weight_in_list_deghost_1 = (
                            ((&wdr_weight_in_list_deghost)[$v1_3] - $v0_8) * (wdr_ev_now_1 - $t4_1)
                            + $v0_8 * $a1_4 + ($a1_4 >> 1)) / $a1_4;
                        break;
                    }
                    
                    $a3_1 = $v1_3;
                }
                
                $a1_3 = &$a1_3[1];
                
                if ($v1_3 == 8)
                {
                    wdr_weight_in_list_deghost_1 = 0;
                    break;
                }
            }
        }
        else
            wdr_weight_in_list_deghost_1 = data_b1478_1;
        
        if (wdr_weight_in_list_deghost_1 >= wdr_weight_in_list_deghost_3)
            wdr_weight_in_list_deghost_1 = wdr_weight_in_list_deghost_3;
        
        wdr_weight_in_list_deghost_4 = wdr_weight_in_list_deghost_1;
    }
    
    if (data_b1e60_1 == 1)
    {
        int32_t mdns_y_pspa_ref_bi_thres_array_1 = mdns_y_pspa_ref_bi_thres_array;
        uint32_t wdr_weight_in_list_deghost_5 = 0;
                int32_t $v0_11 = $a0 - $t1;
        if (!wdr_ev_changed)
            mdns_y_pspa_ref_bi_thres_array = 0;
        
        
        if (mdns_y_pspa_ref_bi_thres_array_1 < $a0)
        {
            wdr_weight_in_list_deghost_5 = 0x20;
            
            if (mdns_y_pspa_ref_bi_thres_array_1 >= $t1)
            {
                wdr_weight_in_list_deghost_5 =
                    ((($a0 - mdns_y_pspa_ref_bi_thres_array_1) << 5) + ($v0_11 >> 1)) / $v0_11;
            }
        }
        
        if (wdr_weight_in_list_deghost_5 < wdr_weight_in_list_deghost_4)
            wdr_weight_in_list_deghost_4 = wdr_weight_in_list_deghost_5;
    }
    
    uint32_t wdr_ev_changed_1 = wdr_ev_changed;
    data_b1ff8_1 = wdr_weight_in_list_deghost_4;
    
    if (wdr_ev_changed_1 == 1)
    {
        uint32_t wdr_ev_old_1 = wdr_ev_old;
        int32_t* $v0_12 = &wdr_ev_list;
        int32_t $a0_4 = 0;
            int32_t $t7_1 = *$v0_12;
                    int32_t $v0_20 = (&wdr_ev_list)[$a0_4 - 1];
                        int32_t $t8_1 = (&wdr_detail_w_in0_list)[$a0_4 - 1];
                        int32_t $t9_1 = (&wdr_detail_w_in0_list)[$a0_4];
                        int32_t $a1_8 = $v0_20 < wdr_ev_old_1 ? 1 : 0;
                        int32_t $t5_4 = $t7_1 < $v0_20 ? 1 : 0;
                            int32_t $t9_5 = wdr_ev_old_1 - $v0_20;
                            int32_t $ra_4 = $v0_20 - $t7_1;
        wdr_ev_changed = 0;
        
        while (true)
        {
            
            if ($t7_1 >= wdr_ev_old_1)
            {
                if ($a0_4)
                {
                    
                    if ($t7_1 != $v0_20)
                    {
                        int32_t $t8_2;
                        
                        if ($t9_1 >= $t8_1)
                        {
                            
                            if (!$a1_8)
                                $t9_5 = $v0_20 - wdr_ev_old_1;
                            
                            
                            if (!$t5_4)
                                $ra_4 = $t7_1 - $v0_20;
                            
                            $t8_2 = $t9_5 * ($t9_1 - $t8_1) / $ra_4 + $t8_1;
                        }
                        else
                        {
                            int32_t $t9_2 = wdr_ev_old_1 - $v0_20;
                            int32_t $ra_2 = $v0_20 - $t7_1;
                            
                            if (!$a1_8)
                                $t9_2 = $v0_20 - wdr_ev_old_1;
                            
                            
                            if (!$t5_4)
                                $ra_2 = $t7_1 - $v0_20;
                            
                            $t8_2 = $t8_1 - $t9_2 * ($t8_1 - $t9_1) / $ra_2;
                        }
                        
                        mdns_y_fiir_fus_wei1_wdr_array = $t8_2;
                        int32_t $t8_4 = (&wdr_detail_w_in1_list)[$a0_4 - 1];
                        int32_t $t4_4 = (&wdr_detail_w_in1_list)[$a0_4];
                        int32_t $t8_5;
                        
                        if ($t4_4 >= $t8_4)
                        {
                            int32_t $t4_8 = wdr_ev_old_1 - $v0_20;
                            int32_t $t9_11 = $v0_20 - $t7_1;
                            
                            if (!$a1_8)
                                $t4_8 = $v0_20 - wdr_ev_old_1;
                            
                            
                            if (!$t5_4)
                                $t9_11 = $t7_1 - $v0_20;
                            
                            $t8_5 = $t4_8 * ($t4_4 - $t8_4) / $t9_11 + $t8_4;
                        }
                        else
                        {
                            int32_t $t4_5 = wdr_ev_old_1 - $v0_20;
                            int32_t $t9_9 = $v0_20 - $t7_1;
                            
                            if (!$a1_8)
                                $t4_5 = $v0_20 - wdr_ev_old_1;
                            
                            
                            if (!$t5_4)
                                $t9_9 = $t7_1 - $v0_20;
                            
                            $t8_5 = $t8_4 - $t4_5 * ($t8_4 - $t4_4) / $t9_9;
                        }
                        
                        int32_t $t4_12 = wdr_detail_w_in4_list[$a0_4 + 0x11];
                        int32_t $t3_4 = wdr_detail_w_in4_list[0x12 + $a0_4];
                        data_c7634_1 = $t8_5;
                        int32_t $t4_13;
                        
                        if ($t3_4 >= $t4_12)
                        {
                            int32_t $t3_8 = wdr_ev_old_1 - $v0_20;
                            int32_t $t8_9 = $v0_20 - $t7_1;
                            
                            if (!$a1_8)
                                $t3_8 = $v0_20 - wdr_ev_old_1;
                            
                            
                            if (!$t5_4)
                                $t8_9 = $t7_1 - $v0_20;
                            
                            $t4_13 = $t3_8 * ($t3_4 - $t4_12) / $t8_9 + $t4_12;
                        }
                        else
                        {
                            int32_t $t3_5 = wdr_ev_old_1 - $v0_20;
                            int32_t $t8_7 = $v0_20 - $t7_1;
                            
                            if (!$a1_8)
                                $t3_5 = $v0_20 - wdr_ev_old_1;
                            
                            
                            if (!$t5_4)
                                $t8_7 = $t7_1 - $v0_20;
                            
                            $t4_13 = $t4_12 - $t3_5 * ($t4_12 - $t3_4) / $t8_7;
                        }
                        
                        int32_t $t3_12 = wdr_detail_w_in4_list[$a0_4 + 8];
                        int32_t $t2_3 = wdr_detail_w_in4_list[9 + $a0_4];
                        data_c7638_1 = $t4_13;
                        int32_t $t3_13;
                        
                        if ($t2_3 >= $t3_12)
                        {
                            int32_t $t2_7 = wdr_ev_old_1 - $v0_20;
                            int32_t $t4_17 = $v0_20 - $t7_1;
                            
                            if (!$a1_8)
                                $t2_7 = $v0_20 - wdr_ev_old_1;
                            
                            
                            if (!$t5_4)
                                $t4_17 = $t7_1 - $v0_20;
                            
                            $t3_13 = $t2_7 * ($t2_3 - $t3_12) / $t4_17 + $t3_12;
                        }
                        else
                        {
                            int32_t $t2_4 = wdr_ev_old_1 - $v0_20;
                            int32_t $t4_15 = $v0_20 - $t7_1;
                            
                            if (!$a1_8)
                                $t2_4 = $v0_20 - wdr_ev_old_1;
                            
                            
                            if (!$t5_4)
                                $t4_15 = $t7_1 - $v0_20;
                            
                            $t3_13 = $t3_12 - $t2_4 * ($t3_12 - $t2_3) / $t4_15;
                        }
                        
                        int32_t $t2_11 = wdr_detail_w_in4_list[$a0_4 - 1];
                        int32_t $t1_3 = wdr_detail_w_in4_list[$a0_4];
                        data_c763c_1 = $t3_13;
                        int32_t $t2_12;
                        
                        if ($t1_3 >= $t2_11)
                        {
                            int32_t $t1_7 = wdr_ev_old_1 - $v0_20;
                            int32_t $t3_17 = $v0_20 - $t7_1;
                            
                            if (!$a1_8)
                                $t1_7 = $v0_20 - wdr_ev_old_1;
                            
                            
                            if (!$t5_4)
                                $t3_17 = $t7_1 - $v0_20;
                            
                            $t2_12 = $t1_7 * ($t1_3 - $t2_11) / $t3_17 + $t2_11;
                        }
                        else
                        {
                            int32_t $t1_4 = wdr_ev_old_1 - $v0_20;
                            int32_t $t3_15 = $v0_20 - $t7_1;
                            
                            if (!$a1_8)
                                $t1_4 = $v0_20 - wdr_ev_old_1;
                            
                            
                            if (!$t5_4)
                                $t3_15 = $t7_1 - $v0_20;
                            
                            $t2_12 = $t2_11 - $t1_4 * ($t2_11 - $t1_3) / $t3_15;
                        }
                        
                        int32_t $t1_11 = (&wdr_weight_b_in_list)[$a0_4 - 1];
                        int32_t $t0_3 = (&wdr_weight_b_in_list)[$a0_4];
                        data_c7640_1 = $t2_12;
                        int32_t $t1_12;
                        
                        if ($t0_3 >= $t1_11)
                        {
                            int32_t $t0_7 = wdr_ev_old_1 - $v0_20;
                            int32_t $t2_16 = $v0_20 - $t7_1;
                            
                            if (!$a1_8)
                                $t0_7 = $v0_20 - wdr_ev_old_1;
                            
                            
                            if (!$t5_4)
                                $t2_16 = $t7_1 - $v0_20;
                            
                            $t1_12 = $t0_7 * ($t0_3 - $t1_11) / $t2_16 + $t1_11;
                        }
                        else
                        {
                            int32_t $t0_4 = wdr_ev_old_1 - $v0_20;
                            int32_t $t2_14 = $v0_20 - $t7_1;
                            
                            if (!$a1_8)
                                $t0_4 = $v0_20 - wdr_ev_old_1;
                            
                            
                            if (!$t5_4)
                                $t2_14 = $t7_1 - $v0_20;
                            
                            $t1_12 = $t1_11 - $t0_4 * ($t1_11 - $t0_3) / $t2_14;
                        }
                        
                        int32_t $a2_10 = (&wdr_weight_p_in_list)[$a0_4 - 1];
                        int32_t $a0_8 = (&wdr_weight_p_in_list)[$a0_4];
                        data_c7644_1 = $t1_12;
                        int32_t $a1_11;
                        
                        if ($a0_8 >= $a2_10)
                        {
                            int32_t $a3_8 = wdr_ev_old_1 - $v0_20;
                            int32_t $v0_28 = $t7_1 - $v0_20;
                            
                            if (!$a1_8)
                                $a3_8 = $v0_20 - wdr_ev_old_1;
                            
                            
                            if ($t5_4)
                                $v0_28 = $v0_20 - $t7_1;
                            
                            $a1_11 = $a3_8 * ($a0_8 - $a2_10) / $v0_28 + $a2_10;
                        }
                        else
                        {
                            int32_t $v1_12 = wdr_ev_old_1 - $v0_20;
                            int32_t $a1_9 = $v0_20 - $t7_1;
                            
                            if (!$a1_8)
                                $v1_12 = $v0_20 - wdr_ev_old_1;
                            
                            
                            if (!$t5_4)
                                $a1_9 = $t7_1 - $v0_20;
                            
                            $a1_11 = $a2_10 - $v1_12 * ($a2_10 - $a0_8) / $a1_9;
                        }
                        
                        data_c7648_1 = $a1_11;
                    }
                    else
                    {
                        mdns_y_fiir_fus_wei1_wdr_array = (&wdr_detail_w_in0_list)[$a0_4];
                        data_c7634 = (&wdr_detail_w_in1_list)[$a0_4];
                        data_c7638 = wdr_detail_w_in4_list[0x12 + $a0_4];
                        data_c763c = wdr_detail_w_in4_list[9 + $a0_4];
                        data_c7640 = wdr_detail_w_in4_list[$a0_4];
                        data_c7644 = (&wdr_weight_b_in_list)[$a0_4];
                        data_c7648 = (&wdr_weight_p_in_list)[$a0_4];
                    }
                }
                else
                {
                    mdns_y_fiir_fus_wei1_wdr_array = wdr_detail_w_in0_list;
                    data_c7634 = wdr_detail_w_in1_list;
                    data_c7638 = wdr_detail_w_in4_list[0x12];
                    data_c763c = wdr_detail_w_in4_list[9];
                    data_c7640 = wdr_detail_w_in4_list[0];
                    data_c7644 = wdr_weight_b_in_list;
                    data_c7648 = wdr_weight_p_in_list;
                }
                
                break;
            }
            
            $a0_4 += 1;
            $v0_12 = &$v0_12[1];
            
            if ($a0_4 == 9)
            {
                mdns_y_fiir_fus_wei1_wdr_array = data_d7690;
                data_c7634 = data_d766c;
                data_c7638 = wdr_detail_w_in4_list[0x1a];
                data_c763c = wdr_detail_w_in4_list[0x11];
                data_c7640 = wdr_detail_w_in4_list[8];
                data_c7644 = data_b1550;
                data_c7648 = data_b152c;
                break;
            }
        }
        
        tiziano_wdr_fusion1_curve();
    }
    
    param_wdr_detail_th_w_array = mdns_y_fiir_fus_wei1_wdr_array;
    data_b2340_1 = data_c7634_2;
    data_b2344_1 = data_c7638_2;
    data_b2348_1 = data_c763c_2;
    data_b234c_1 = data_c7640_2;
    wdr_s2l_ratio = data_c7644_2;
    wdr_para_array4 = 0x1ffe;
    uint32_t $v1_23 = data_c7648_2;
    
    if ($(uintptr_t)v1_23 < 0x401)
        $v1_23 = 0x401;
    
    wdr_para_array5 = $v1_23;
    uint32_t wdr_para_array5_1 = wdr_para_array5;
    wdr_para_init_div4 = 0x801;
    int32_t $v0_31 = (wdr_para_array5_1 / 2 + 0x1000000) / wdr_para_array5_1;
    
    if ($(uintptr_t)v0_31 >= 0x2000)
        $v0_31 = 0x1fff;
    
    wdr_para_init_div5 = $v0_31;
    data_b241c_1 = wdr_para_array5_1;
    data_b2420_1 = 0x801;
    uint32_t wdr_para_init_div5_1 = wdr_para_init_div5;
    data_b2418_1 = 0x1ffe;
    data_b2424_1 = wdr_para_init_div5_1;
    return 0;
}

