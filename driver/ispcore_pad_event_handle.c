#include "include/main.h"


  int32_t ispcore_pad_event_handle(int32_t* arg1, int32_t arg2, void* arg3)

{
    int32_t result = 0;
    uint32_t var_58_31;
    void* const $v0_13;
    int32_t $v1_7;
    
    if (*(arg1 + 5) && arg2 - 0x3000001 < 7)
        switch (arg2)
        {
            case 0x3000001:
            {
                void* $a1_3 = arg1[8];
                result = 0;
                
                if (arg3 && $a1_3)
                {
                    if (*(*($a1_3 + 0x7c) + 0x15c) != 1)
                    {
                        memcpy(arg3, $a1_3, 0x70);
                        return 0;
                    }
                    
                    *(arg3 + 4) = *($a1_3 + 4);
                    *(arg3 + 8) = *($a1_3 + 8);
                    __builtin_strncpy(arg3 + 0xc, "RG12", 4);
                    int32_t $v0_6 = *($a1_3 + 4);
                    int32_t $v1_2 = *($a1_3 + 8);
                    *(arg3 + 0x34) = 0;
                    *(arg3 + 0x48) = 0;
                    *(arg3 + 0x18) = ($v0_6 * $v1_2) << 1;
                }
                break;
            }
            case 0x3000002:
            {
                result = 0xffffffea;
                
                if (arg1 && arg1 < 0xfffff001)
                {
                    void* $v0_10 = *arg1;
                    
                    if ($v0_10)
                    {
                        if ($v0_10 >= 0xfffff001)
                            return 0xffffffea;
                        
                        void* $s4_1 = *($v0_10 + 0xd4);
                        
                        if ($s4_1 && $s4_1 < 0xfffff001)
                        {
                            void* $s3_1 = arg1[8];
                            void* $s2 = $v0_10 + 0x38;
                            
                            if (*($s4_1 + 0x15c) == 1)
                            {
                                memset($s4_1 + 0x1c0, 0, 0x18);
                                *($s4_1 + 0x1d4) = arg1;
                                *($s4_1 + 0x1c4) = ispcore_frame_channel_dqbuf;
                                void* $a0_3 = *$s2;
                                
                                while (true)
                                {
                                    if ($a0_3)
                                    {
                                        void* $v0_38 = **($a0_3 + 0xc4);
                                        
                                        if (!$v0_38)
                                            $s2 += 4;
                                        else
                                        {
                                            int32_t $v0_39 = *($v0_38 + 0x1c);
                                            
                                            if (!$v0_39)
                                                $s2 += 4;
                                            else
                                            {
                                                int32_t $v0_40 = $v0_39();
                                                
                                                if (!$v0_40)
                                                    $s2 += 4;
                                                else
                                                {
                                                    if ($v0_40 != 0xfffffdfd)
                                                        return 0;
                                                    
                                                    $s2 += 4;
                                                }
                                            }
                                        }
                                    }
                                    else
                                        $s2 += 4;
                                    
                                    if ($v0_10 + 0x78 == $s2)
                                        break;
                                    
                                    $a0_3 = *$s2;
                                }
                                
                                return 0;
                            }
                            
                            void* $a0_21 = *$s2;
                            void* $v0_42;
                            
                            while (true)
                            {
                                if ($a0_21)
                                {
                                    void* $v0_43 = **($a0_21 + 0xc4);
                                    
                                    if (!$v0_43)
                                        $s2 += 4;
                                    else
                                    {
                                        int32_t $v0_44 = *($v0_43 + 0x1c);
                                        
                                        if (!$v0_44)
                                            $s2 += 4;
                                        else
                                        {
                                            int32_t $v0_45 = $v0_44();
                                            
                                            if (!$v0_45)
                                                $s2 += 4;
                                            else
                                            {
                                                if ($v0_45 != 0xfffffdfd)
                                                {
                                                    $v0_42 = *($s3_1 + 0x7c);
                                                    break;
                                                }
                                                
                                                $s2 += 4;
                                            }
                                        }
                                    }
                                }
                                else
                                    $s2 += 4;
                                
                                if ($v0_10 + 0x78 == $s2)
                                {
                                    $v0_42 = *($s3_1 + 0x7c);
                                    break;
                                }
                                
                                $a0_21 = *$s2;
                            }
                            
                            int32_t $a1_12 = *($v0_42 + 0x148);
                            
                            while (true)
                            {
                                int32_t $v1_18 = *(arg3 + 0xc);
                                
                                if (*($v0_42 + 0x14c) < $a1_12)
                                {
                                    int32_t var_60_2_2 = $v1_18;
                                    isp_printf(2, "Err [VIC_INT] : mipi fid asfifo ovf!!!\\n", 
                                        "ispcore_frame_channel_s_fmt");
                                    break;
                                }
                                
                                if ($v1_18 == *($a1_12 * 0x2c + 0xb263c))
                                {
                                    int32_t $a3_1 = *(arg3 + 4);
                                    uint32_t $a0_27 = ($a3_1 * *($a1_12 * 0x2c + 0xb2640)) >> 3;
                                    *(arg3 + 0x14) = $a0_27;
                                    int32_t $v0_62 = *(arg3 + 8);
                                    int32_t $v0_63;
                                    
                                    if ($v1_18 != 0x3132564e)
                                    {
                                        $v0_63 = $v1_18 == 0x3231564e
                                            ? (($v0_62 + 0xf) & 0xfffffff0) * $a0_27
                                            : $v0_62 * $a0_27;
                                    }
                                    else
                                        $v0_63 = (($v0_62 + 0xf) & 0xfffffff0) * $a0_27;
                                    
                                    *(arg3 + 0x18) = $v0_63;
                                    *($s3_1 + 0xbc) = (*($a1_12 * 0x2c + 0xb2640) >> 3) * $a3_1;
                                    *(arg3 + 0x20) = &isp_output_fmt[$a1_12 * 0x2c];
                                    break;
                                }
                                
                                $a1_12 += 1;
                            }
                            
                            memset(&var_58_32, 0, 0x34);
                            uint32_t $a0_25 = arg1[1];
                            uint32_t var_38_1_17 = *(arg3 + 0x5c);
                            int32_t var_34_1_14 = *(arg3 + 0x64);
                            int32_t var_30_1_18 = *(arg3 + 0x60);
                            int32_t var_2c_1_13 = *(arg3 + 0x68);
                            int32_t var_28_1_7 = *(arg3 + 0x6c);
                            var_58_33 = *(arg3 + 0x48);
                            int32_t var_54_1_6 = *(arg3 + 0x4c);
                            int32_t var_50_1_6 = *(arg3 + 0x50);
                            uint32_t var_4c_1_6 = *(arg3 + 0x34);
                            int32_t var_48_1_11 = *(arg3 + 0x3c);
                            int32_t var_44_1_3 = *(arg3 + 0x38);
                            int32_t var_40_1_9 = *(arg3 + 0x40);
                            int32_t var_3c_1_7 = *(arg3 + 0x44);
                            
                            if (tisp_channel_attr_set($a0_25, &var_58_34))
                            {
                                isp_printf(2, "Err [VIC_INT] : dma syfifo ovf!!!\\n", 
                                    "ispcore_frame_channel_set_fmt");
                                return 0;
                            }
                            
                            memcpy($s3_1, arg3, 0x70);
                            return 0;
                        }
                    }
                }
                break;
            }
            case 0x3000003:
            {
                $v0_13 = nullptr;
                
                if (!arg1)
                    var_58_35 = 0;
                else if (arg1 >= 0xfffff001)
                    var_58_36 = 0;
                else
                {
                    void* $v1_6 = *arg1;
                    
                    if (!$v1_6)
                        var_58_37 = 0;
                    else if ($v1_6 < 0xfffff001)
                    {
                        $v0_13 = *($v1_6 + 0xd4);
                        var_58_38 = 0;
                    }
                    else
                        var_58_39 = 0;
                }
                
                void* $s2_1 = arg1[8];
                
                if (*($v0_13 + 0x15c) == 1)
                {
                    $v1_7 = *($v0_13 + 0x1cc);
                    
                    if (!$v1_7)
                        return 0;
                    
                    $v1_7(*($v0_13 + 0x1d0), 1);
                    return 0;
                }
                
                if (*(arg1 + 7) != 3)
                    return 0;
                
                __private_spin_lock_irqsave($s2_1 + 0x9c, &var_58_40);
                
                if (*($s2_1 + 0x74) != 4)
                {
                    tisp_channel_start(arg1[1]);
                    *($s2_1 + 0x74) = 4;
                    uint32_t $a1_6 = var_58_41;
                    *(arg1 + 7) = 4;
                    result = 0;
                    private_spin_unlock_irqrestore($s2_1 + 0x9c, $a1_6);
                }
                else
                {
                    arch_local_irq_restore(var_58_42);
                    void* entry_$gp;
                    *(entry_$gp + 0x14) -= 1;
                    result = 0;
                    
                    if (*(entry_$gp + 8) >> 2 & 1)
                        preempt_schedule();
                }
                break;
            }
            case 0x3000004:
            {
                ispcore_frame_channel_streamoff(arg1);
                return 0;
                break;
            }
            case 0x3000005:
            {
                void* const $v0_21;
                void* const $s3_4;
                
                if (!arg1 || arg1 >= 0xfffff001)
                {
                    $s3_4 = nullptr;
                    $v0_21 = nullptr;
                    var_58_43 = 0;
                }
                else
                {
                    $s3_4 = *arg1;
                    $v0_21 = nullptr;
                    
                    if (!$s3_4)
                        var_58_44 = 0;
                    else if ($s3_4 < 0xfffff001)
                    {
                        $v0_21 = *($s3_4 + 0xd4);
                        var_58_45 = 0;
                    }
                    else
                        var_58_46 = 0;
                }
                
                if (*($v0_21 + 0x15c) != 1)
                {
                    result = 0;
                    
                    if (!(arg1[5] & 0x20))
                    {
                        void* $s1_2 = arg1[8];
                        
                        if (!arg3 || !$s1_2)
                        {
                            void* var_5c_1_5 = $s1_2;
                            void* var_60_1_4 = arg3;
                            isp_printf(2, "Err [VIC_INT] : image syfifo ovf !!!\\n", 
                                "ispcore_frame_channel_qbuf");
                            return 0;
                        }
                        
                        *(arg3 - 0x1c) = 4;
                        __private_spin_lock_irqsave($s1_2 + 0x9c, &var_58_47);
                        
                        if (*($s1_2 + 0xc) != 0x3231564e)
                        {
                            isp_printf(2, "Err [VIC_INT] : control limit err!!!\\n", 
                                "ispcore_frame_channel_qbuf");
                            return 0xffffffff;
                        }
                        
                        int32_t $v0_26 = (*($s1_2 + 8) + 0xf) & 0xfffffff0;
                        int32_t $a1_9 = *($s1_2 + 4);
                        *(arg3 - 0x1c) = 5;
                        int32_t $a0_13 = *(arg3 + 8);
                        *(arg3 - 0x18) += 1;
                        *(arg3 + 0xc) = $v0_26 * $a1_9 + $a0_13;
                        *(*($s3_4 + 0xb8) + (*($s1_2 + 0x70) << 8) + 0x996c) = $a0_13;
                        *(*($s3_4 + 0xb8) + (*($s1_2 + 0x70) << 8) + 0x9984) = *(arg3 + 0xc);
                        private_spin_unlock_irqrestore($s1_2 + 0x9c, var_58_48);
                    }
                }
                else
                {
                    int32_t $v1_9 = *($v0_21 + 0x1c0);
                    result = 0;
                    
                    if ($v1_9)
                        $v1_9(*($v0_21 + 0x1d0), arg3);
                }
                break;
            }
            case 0x3000006:
            {
                return 0;
                break;
            }
            case 0x3000007:
            {
                $v0_13 = nullptr;
                
                if (!arg1)
                    var_58_49 = 0;
                else if (arg1 >= 0xfffff001)
                    var_58_50 = 0;
                else
                {
                    void* $v1_17 = *arg1;
                    
                    if (!$v1_17)
                        var_58_51 = 0;
                    else if ($v1_17 < 0xfffff001)
                    {
                        $v0_13 = *($v1_17 + 0xd4);
                        var_58_52 = 0;
                    }
                    else
                        var_58_53 = 0;
                }
                
                if (*($v0_13 + 0x15c) == 1)
                {
                    $v1_7 = *($v0_13 + 0x1c8);
                    
                    if (!$v1_7)
                        return 0;
                    
                    $v1_7(*($v0_13 + 0x1d0), arg3);
                    return 0;
                }
                
                result = 0;
                
                if (!(arg1[5] & 0x20))
                {
                    void* $s0_2 = arg1[8];
                    
                    if ($s0_2)
                    {
                        __private_spin_lock_irqsave($s0_2 + 0x9c, &var_58_54);
                        tisp_channel_fifo_clear(arg1[1]);
                        result = 0;
                        private_spin_unlock_irqrestore($s0_2 + 0x9c, var_58_55);
                    }
                }
                break;
            }
        }
    return result;
}

