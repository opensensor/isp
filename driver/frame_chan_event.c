#include "include/main.h"


  int32_t frame_chan_event(int32_t* arg1, int32_t arg2, void* arg3)

{
    if (!arg1)
        return 0xffffffea;
    
    if (arg1 >= 0xfffff001)
        return 0xffffffea;
    
    if (arg2 != 0x3000006)
        return 0;
    
    int32_t $v0_3 = *(*arg1 + 0xdc);
    uint32_t $s0_1 = arg1[1];
    int32_t var_38_15 = 0;
    
    if (arg3)
    {
        void* $s0_2 = $s0_1 * 0x2ec + $v0_3;
        int32_t $a2 = __private_spin_lock_irqsave($s0_2 + 0x2c4, &var_38_16);
        uint32_t $s2_2;
        void* $s3_1;
        void* $s3_3;
        
        if (!isp_ch0_pre_dequeue_time)
        {
            $s3_1 = *($s0_2 + 0x210);
        label_1aee0:
            $s2_2 = 0;
            $s3_3 = $s3_1 - 0x58;
            
            while (true)
            {
                int32_t $a2_3;
                
                if ($s0_2 + 0x210 == $s3_3 + 0x58)
                {
                    $s3_3 = nullptr;
                    
                    if (!$s2_2)
                        goto label_1b27c;
                    
                    $a2_3 = *($s0_2 + 0x2c0);
                    goto label_1af4c;
                }
                
                if (*($s3_3 + 0x34) == *(arg3 + 8))
                {
                    *($s3_3 + 0x4c) = 0x65;
                    
                    if (!$s2_2)
                        break;
                    
                    $a2_3 = *($s0_2 + 0x2c0);
                label_1af4c:
                    isp_printf(1, &$LC37, $a2_3);
                    void* $s5_2 = *($s0_2 + 0x210) - 0x58;
                    
                    while ($s0_2 + 0x210 != $s5_2 + 0x58)
                    {
                        isp_printf(1, "help:\\n", *($s5_2 + 0x34));
                        $s5_2 = *($s5_2 + 0x58) - 0x58;
                    }
                    
                    isp_printf(1, "\\t cmd:\\n", *(arg3 + 8));
                    break;
                }
                
                $s2_2 = $s2_2 + 1;
                $s3_3 = *($s3_3 + 0x58) - 0x58;
            }
            
            goto label_1afb8;
        }
        
        if (*($s0_2 + 0x2c0))
        {
            $s3_1 = *($s0_2 + 0x210);
            goto label_1aee0;
        }
        
        if (!*(arg3 + 0x14))
        {
            int32_t $a2_2 = data_b2c5c_1;
            
            if ($a2_2 != *(arg3 + 8))
                $a2_2 = isp_printf(1, "saveraw", $a2_2);
            
            if (isp_ch0_pre_dequeue_intc_ahead == 1)
                $a2_2 = isp_printf(2, 
                    "width is %d, height is %d, imagesize is %d\\n, save num is %d, buf size is %d", 
                    $a2_2);
            
            if (isp_ch0_pre_dequeue_flag != 1)
            {
                isp_printf(1, "help", $a2_2);
                uint8_t isp_ch0_pre_dequeue_intc_ahead_cnt_1 = isp_ch0_pre_dequeue_intc_ahead_cnt;
                isp_ch0_pre_dequeue_intc_ahead = 1;
                isp_ch0_pre_dequeue_intc_ahead_cnt = isp_ch0_pre_dequeue_intc_ahead_cnt_1 + 1;
                isp_ch0_pdq_intc_cnt += 1;
            }
            else
                isp_ch0_pre_dequeue_flag = 0;
        }
        else
        {
            int32_t $v0_7 = *($s0_2 + 0x210);
            
            if ($s0_2 + 0x210 != $v0_7 && *(0xa0000000 | *($v0_7 - 0x24)) != 0x12345678)
            {
                int16_t $v0_11 = *(arg3 + 0x18);
                uint32_t $s2_1 = $v0_11;
                void* $s6_1;
                
                if (*($s0_2 + 0x244) == $s2_1 || !$v0_11)
                {
                    if (isp_ch0_pre_dequeue_intc_ahead != 1)
                        $a2 = isp_printf(1, "nv12", $s2_1);
                    
                    $s6_1 = *($s0_2 + 0x210);
                }
                else
                    $s6_1 = *($s0_2 + 0x210);
                
                isp_ch0_pre_dequeue_flag = 1;
                isp_ch0_pre_dequeue_intc_ahead = 0;
                
                if ($s0_2 + 0x210 != $s6_1)
                {
                    $s3_3 = $s6_1 - 0x58;
                    memcpy(&ch0_pre_dequeue_vb, $s3_3, 0x68);
                    int32_t $v0_14 = $s2_1 << 0x10 | 1;
                    *($s6_1 - 0x18) = $v0_14;
                    data_b2c68_1 = $v0_14;
                    isp_ch0_pre_dequeue_valid_lines = $s2_1;
                    uint32_t isp_ch0_pdq_cnt_1 = isp_ch0_pdq_cnt;
                    *($s0_2 + 0x238) = 0;
                    isp_ch0_pdq_cnt = isp_ch0_pdq_cnt_1 + 1;
                    $s2_2 = 0;
                label_1afb8:
                    int32_t $v0_33;
                    
                    if (!$s3_3)
                    {
                    label_1b27c:
                        uint32_t var_58_2 = $s2_2;
                        int32_t var_54_2 = *(arg3 + 0x18);
                        isp_printf(1, 
                            "\\t\\t\\t use cmd " snapraw" you should set ispmem first!!!!!\\n", 
                            *(arg3 + 8));
                        isp_err3 += 1;
                        $v0_33 = *($s0_2 + 0x2c0);
                    label_1b2c4:
                        *(($v0_33 << 2) + &lastaddr) = *(arg3 + 8);
                    }
                    else
                    {
                        uint32_t $s3_5 = 0;
                        void** $v0_22 = *($s0_2 + 0x210);
                        
                        while (true)
                        {
                            char $s3_6;
                            
                            if ($v0_22 == 0x58)
                            {
                                uint32_t var_44_1_1 = $s2_2;
                            label_1b1f0:
                                uint32_t var_48_1_4 = $s3_5;
                                int32_t var_4c_1_2 = *($v0_22 - 0x10);
                                void* var_54_1 = &$v0_22[-0x16];
                                int32_t var_50_1_2 = *($v0_22 - 0x24);
                                int32_t var_58_1 = *($s0_2 + 0x2c0);
                                isp_printf(1, "\\t\\t snapraw\\n", "frame_channel_buffer_done");
                                $s3_6 = $s3_5 + 1;
                                *($s0_2 + 0x2e4) += 1;
                            }
                            else
                            {
                                if (*($v0_22 - 0x10) != 3)
                                {
                                    uint32_t var_44_2_1 = $s2_2;
                                    goto label_1b1f0;
                                }
                                
                                *($v0_22 - 0xc) = 0x66;
                                int32_t var_40_11;
                                private_getrawmonotonic(&var_40_12);
                                *($v0_22 - 4) += 1;
                                *($v0_22 - 0x44) = var_40_13;
                                *($v0_22 - 0x2c) = 0;
                                void* $a1_4 = *$v0_22;
                                int32_t var_3c_5;
                                *($v0_22 - 0x40) = var_3c_6 / 0x3e8;
                                void** $a0_9 = $v0_22[1];
                                *($a1_4 + 4) = $a0_9;
                                *$a0_9 = $a1_4;
                                *$v0_22 = 0x100100;
                                $v0_22[1] = 0x200200;
                                *($s0_2 + 0x218) -= 1;
                                int32_t $a0_14;
                                
                                if (!isp_ch0_pre_dequeue_time)
                                {
                                    *($v0_22 - 0x18) &= 0xffff0000;
                                    $a0_14 = *($s0_2 + 0x2c0);
                                }
                                else
                                {
                                    $a0_14 = *($s0_2 + 0x2c0);
                                    
                                    if ($a0_14)
                                    {
                                        *($v0_22 - 0x18) &= 0xffff0000;
                                        $a0_14 = *($s0_2 + 0x2c0);
                                    }
                                }
                                
                                if (!$a0_14)
                                {
                                    uint32_t isp_day_night_switch_drop_frame_cnt_1 =
                                        *isp_day_night_switch_drop_frame_cnt;
                                    
                                    if (isp_day_night_switch_drop_frame_cnt_1)
                                    {
                                        *isp_day_night_switch_drop_frame_cnt =
                                            isp_day_night_switch_drop_frame_cnt_1 - 1;
                                        
                                        if ($v0_22 == 0xb2c80)
                                            break;
                                        
                                        void*** $a0_17 = *($s0_2 + 0x214);
                                        *($s0_2 + 0x214) = $v0_22;
                                        $v0_22[1] = $a0_17;
                                        *$v0_22 = $s0_2 + 0x210;
                                        *$a0_17 = $v0_22;
                                        $a0_14 = 1;
                                        goto label_1b12c;
                                    }
                                }
                                else if ($a0_14 == 1)
                                {
                                    uint32_t $a2_6 = (*(isp_day_night_switch_drop_frame_cnt + 1));
                                    
                                    if ($a2_6)
                                    {
                                        (*(isp_day_night_switch_drop_frame_cnt + 1)) = $a2_6 - 1;
                                        void*** $a1_6 = *($s0_2 + 0x214);
                                        *($s0_2 + 0x214) = $v0_22;
                                        *$v0_22 = $s0_2 + 0x210;
                                        $v0_22[1] = $a1_6;
                                        *$a1_6 = $v0_22;
                                    label_1b12c:
                                        *($v0_22 - 0x10) = $a0_14;
                                    label_1b138:
                                        *($s0_2 + 0x218) += 1;
                                        __enqueue_in_driver(&$v0_22[-0x16]);
                                        break;
                                    }
                                }
                                else if ($a0_14 == 2)
                                {
                                    uint32_t $a1_7 = (*(isp_day_night_switch_drop_frame_cnt + 2));
                                    
                                    if ($a1_7)
                                    {
                                        (*(isp_day_night_switch_drop_frame_cnt + 2)) = $a1_7 - 1;
                                        void*** $a0_19 = *($s0_2 + 0x214);
                                        *($s0_2 + 0x214) = $v0_22;
                                        *$v0_22 = $s0_2 + 0x210;
                                        $v0_22[1] = $a0_19;
                                        *$a0_19 = $v0_22;
                                        *($v0_22 - 0x10) = 1;
                                        goto label_1b138;
                                    }
                                }
                                
                                *($v0_22 - 0x10) = 4;
                                void** $v1_11 = *($s0_2 + 0x220);
                                *($s0_2 + 0x220) = &$v0_22[2];
                                $v0_22[2] = $s0_2 + 0x21c;
                                $v0_22[3] = $v1_11;
                                *$v1_11 = &$v0_22[2];
                                *($s0_2 + 0x224) += 1;
                                private_wake_up($s0_2 + 0x228);
                                private_complete($s0_2 + 0x2d4);
                                $s3_6 = $s3_5 + 1;
                            }
                            
                            $s3_5 = $s3_6;
                            
                            if ($s2_2 < $s3_5)
                            {
                                $v0_33 = *($s0_2 + 0x2c0);
                                goto label_1b2c4;
                            }
                            
                            $v0_22 = *($s0_2 + 0x210);
                        }
                    }
                }
                else
                    isp_printf(2, &$LC33, $a2);
            }
            else
                isp_ch0_pre_dequeue_drop += 1;
        }
        
        private_spin_unlock_irqrestore($s0_2 + 0x2c4, var_38_17);
    }
    else
        isp_printf(1, "/tmp/snap%d.%s", "frame_channel_buffer_done");
    
    return 0;
}

