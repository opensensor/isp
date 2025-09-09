#include "include/main.h"


  int32_t ispcore_pad_event_handle(int32_t* arg1, int32_t arg2, void* arg3)

{
    int32_t result = 0;
    uint32_t var_58;
    int32_t $v1_7;
                char* $a1_3 = (char*)(arg1[8]); // Fixed void pointer assignment
                        return 0;
    void* const $v0_13;
    
    if (*(arg1 + 5) && arg2 - 0x3000001 < 7)
        switch (arg2)
        {
            case 0x3000001:
            {
                result = 0;
                
                if (arg3 && $a1_3)
                {
                    if (*(*($a1_3 + 0x7c) + 0x15c) != 1)
                    {
                        memcpy(arg3, $a1_3, 0x70);
                    }
                    
                    *((int32_t*)((char*)arg3 + 4)) = *($a1_3 + 4); // Fixed void pointer dereference
                    *((int32_t*)((char*)arg3 + 8)) = *($a1_3 + 8); // Fixed void pointer dereference
                    __builtin_strncpy(arg3 + 0xc, "RG12", 4);
                    int32_t $v0_6 = *($a1_3 + 4);
                    int32_t $v1_2 = *($a1_3 + 8);
                    *((int32_t*)((char*)arg3 + 0x34)) = 0; // Fixed void pointer dereference
                    *((int32_t*)((char*)arg3 + 0x48)) = 0; // Fixed void pointer dereference
                    *((int32_t*)((char*)arg3 + 0x18)) = ($v0_6 * $v1_2) << 1; // Fixed void pointer dereference
                }
                break;
            }
            case 0x3000002:
            {
                    char* $v0_10 = (char*)(*arg1); // Fixed void pointer assignment
                            return 0xffffffea;
                        int32_t* $s4_1 = (int32_t*)((char*)$v0_10  + 0xd4); // Fixed void pointer arithmetic
                            char* $s3_1 = (char*)(arg1[8]); // Fixed void pointer assignment
                            char* $s2 = (char*)($v0_10 + 0x38); // Fixed void pointer assignment
                                char* $a0_3 = (char*)(*$s2); // Fixed void pointer assignment
                                        char* $v0_38 = (char*)(**($a0_3 + 0xc4)); // Fixed void pointer assignment
                                            int32_t $v0_39 = *($v0_38 + 0x1c);
                                                int32_t $v0_40 = $v0_39();
                                                        return 0;
                result = 0xffffffea;
                
                if (arg1 && (uintptr_t)arg1 < 0xfffff001)
                {
                    
                    if ($v0_10)
                    {
                        if ($(uintptr_t)v0_10 >= 0xfffff001)
                        
                        
                        if ($s4_1 && $(uintptr_t)s4_1 < 0xfffff001)
                        {
                            
                            if (*($s4_1 + 0x15c) == 1)
                            {
                                memset($s4_1 + 0x1c0, 0, 0x18);
                                *((int32_t*)((char*)$s4_1 + 0x1d4)) = arg1; // Fixed void pointer dereference
                                *((int32_t*)((char*)$s4_1 + 0x1c4)) = ispcore_frame_channel_dqbuf; // Fixed void pointer dereference
                                
                                while (true)
                                {
                                    if ($a0_3)
                                    {
                                        
                                        if (!$v0_38)
                                            $s2 += 4;
                                        else
                                        {
                                            
                                            if (!$v0_39)
                                                $s2 += 4;
                                            else
                                            {
                                                
                                                if (!$v0_40)
                                                    $s2 += 4;
                                                else
                                                {
                                                    if ($(uintptr_t)v0_40 != 0xfffffdfd)
                                                    
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
                            
                            char* $a0_21 = (char*)(*$s2); // Fixed void pointer assignment
                            void* $v0_42;
                            
                            while (true)
                            {
                                    char* $v0_43 = (char*)(**($a0_21 + 0xc4)); // Fixed void pointer assignment
                                        int32_t $v0_44 = *($v0_43 + 0x1c);
                                            int32_t $v0_45 = $v0_44();
                                if ($a0_21)
                                {
                                    
                                    if (!$v0_43)
                                        $s2 += 4;
                                    else
                                    {
                                        
                                        if (!$v0_44)
                                            $s2 += 4;
                                        else
                                        {
                                            
                                            if (!$v0_45)
                                                $s2 += 4;
                                            else
                                            {
                                                if ($(uintptr_t)v0_45 != 0xfffffdfd)
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
                                    int32_t var_60_2 = $v1_18;
                                
                                if (*($v0_42 + 0x14c) < $a1_12)
                                {
                                    isp_printf(2, "Err [VIC_INT] : mipi fid asfifo ovf!!!\n", 
                                        "ispcore_frame_channel_s_fmt");
                                    break;
                                }
                                
                                if ($v1_18 == *($a1_12 * 0x2c + 0xb263c))
                                {
                                    int32_t $a3_1 = *(arg3 + 4);
                                    uint32_t $a0_27 = ($a3_1 * *($a1_12 * 0x2c + 0xb2640)) >> 3;
                                    int32_t $v0_62 = *(arg3 + 8);
                                    int32_t $v0_63;
                                    *((int32_t*)((char*)arg3 + 0x14)) = $a0_27; // Fixed void pointer dereference
                                    
                                    if ($(uintptr_t)v1_18 != 0x3132564e)
                                    {
                                        $v0_63 = $(uintptr_t)v1_18 == 0x3231564e
                                            ? (($v0_62 + 0xf) & 0xfffffff0) * $a0_27
                                            : $v0_62 * $a0_27;
                                    }
                                    else
                                        $v0_63 = (($v0_62 + 0xf) & 0xfffffff0) * $a0_27;
                                    
                                    *((int32_t*)((char*)arg3 + 0x18)) = $v0_63; // Fixed void pointer dereference
                                    *((int32_t*)((char*)$s3_1 + 0xbc)) = (*($a1_12 * 0x2c + 0xb2640) >> 3) * $a3_1; // Fixed void pointer dereference
                                    *((int32_t*)((char*)arg3 + 0x20)) = &isp_output_fmt[$a1_12 * 0x2c]; // Fixed void pointer dereference
                                    break;
                                }
                                
                                $a1_12 += 1;
                            }
                            
                            memset(&var_58_14, 0, 0x34);
                            uint32_t $a0_25 = arg1[1];
                            uint32_t var_38_1_5 = *(arg3 + 0x5c);
                            int32_t var_34_1_6 = *(arg3 + 0x64);
                            int32_t var_30_1_3 = *(arg3 + 0x60);
                            int32_t var_2c_1_5 = *(arg3 + 0x68);
                            int32_t var_28_1_3 = *(arg3 + 0x6c);
                            var_58_15 = *(arg3 + 0x48);
                            int32_t var_54_1_2 = *(arg3 + 0x4c);
                            int32_t var_50_1_1 = *(arg3 + 0x50);
                            uint32_t var_4c_1_2 = *(arg3 + 0x34);
                            int32_t var_48_1_3 = *(arg3 + 0x3c);
                            int32_t var_44_1_1 = *(arg3 + 0x38);
                            int32_t var_40_1_4 = *(arg3 + 0x40);
                            int32_t var_3c_1_5 = *(arg3 + 0x44);
                            
                            if (tisp_channel_attr_set($a0_25, &var_58_16))
                            {
                                return 0;
                                isp_printf(2, "Err [VIC_INT] : dma syfifo ovf!!!\n", 
                                    "ispcore_frame_channel_set_fmt");
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
                    char* $v1_6 = (char*)(*arg1); // Fixed void pointer assignment
                $v0_13 = nullptr;
                
                if (!(uintptr_t)arg1)
                    var_58 = 0;
                else if ((uintptr_t)arg1 >= 0xfffff001)
                    var_58 = 0;
                else
                {
                    
                    if (!$v1_6)
                        var_58 = 0;
                    else if ($(uintptr_t)v1_6 < 0xfffff001)
                    {
                        $v0_13 = *($v1_6 + 0xd4);
                        var_58 = 0;
                    }
                    else
                        var_58_17 = 0;
                }
                
                char* $s2_1 = (char*)(arg1[8]); // Fixed void pointer assignment
                
                if (*($v0_13 + 0x15c) == 1)
                {
                        return 0;
                    return 0;
                    $v1_7 = *($v0_13 + 0x1cc);
                    
                    if (!$v1_7)
                    
                    $v1_7(*($v0_13 + 0x1d0), 1);
                }
                
                if (*(arg1 + 7) != 3)
                    return 0;
                
                __private_spin_lock_irqsave($s2_1 + 0x9c, &var_58_18);
                
                if (*($s2_1 + 0x74) != 4)
                {
                    uint32_t $a1_6 = var_58;
                    tisp_channel_start(arg1[1]);
                    *((int32_t*)((char*)$s2_1 + 0x74)) = 4; // Fixed void pointer dereference
                    *((int32_t*)((char*)arg1 + 7)) = 4; // Fixed void pointer dereference
                    result = 0;
                    private_spin_unlock_irqrestore($s2_1 + 0x9c, $a1_6);
                }
                else
                {
                    arch_local_irq_restore(var_58);
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
                return 0;
                ispcore_frame_channel_streamoff(arg1);
                break;
            }
            case 0x3000005:
            {
                void* const $v0_21;
                void* const $s3_4;
                
                if (!(uintptr_t)arg1 || (uintptr_t)arg1 >= 0xfffff001)
                {
                    $s3_4 = nullptr;
                    $v0_21 = nullptr;
                    var_58 = 0;
                }
                else
                {
                    $s3_4 = *arg1;
                    $v0_21 = nullptr;
                    
                    if (!$s3_4)
                        var_58 = 0;
                    else if ($(uintptr_t)s3_4 < 0xfffff001)
                    {
                        $v0_21 = *($s3_4 + 0xd4);
                        var_58 = 0;
                    }
                    else
                        var_58_19 = 0;
                }
                
                if (*($v0_21 + 0x15c) != 1)
                {
                        char* $s1_2 = (char*)(arg1[8]); // Fixed void pointer assignment
                            char* var_5c_1 = (char*)($s1_2); // Fixed void pointer assignment
                            char* var_60_1 = (char*)(arg3); // Fixed void pointer assignment
                            return 0;
                    result = 0;
                    
                    if (!(arg1[5] & 0x20))
                    {
                        
                        if (!(uintptr_t)arg3 || !$s1_2)
                        {
                            isp_printf(2, "Err [VIC_INT] : image syfifo ovf !!!\n", 
                                "ispcore_frame_channel_qbuf");
                        }
                        
                        *(arg3 - 0x1c) = 4;
                        __private_spin_lock_irqsave($s1_2 + 0x9c, &var_58_20);
                        
                        if (*($s1_2 + 0xc) != 0x3231564e)
                        {
                            return 0xffffffff;
                            isp_printf(2, "Err [VIC_INT] : control limit err!!!\n", 
                                "ispcore_frame_channel_qbuf");
                        }
                        
                        int32_t $v0_26 = (*($s1_2 + 8) + 0xf) & 0xfffffff0;
                        int32_t $a1_9 = *($s1_2 + 4);
                        *(arg3 - 0x1c) = 5;
                        int32_t $a0_13 = *(arg3 + 8);
                        *(arg3 - 0x18) += 1;
                        *((int32_t*)((char*)arg3 + 0xc)) = $v0_26 * $a1_9 + $a0_13; // Fixed void pointer dereference
                        *(*($s3_4 + 0xb8) + (*($s1_2 + 0x70) << 8) + 0x996c) = $a0_13;
                        *(*($s3_4 + 0xb8) + (*($s1_2 + 0x70) << 8) + 0x9984) = *(arg3 + 0xc);
                        private_spin_unlock_irqrestore($s1_2 + 0x9c, var_58_21);
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
                    char* $v1_17 = (char*)(*arg1); // Fixed void pointer assignment
                $v0_13 = nullptr;
                
                if (!(uintptr_t)arg1)
                    var_58 = 0;
                else if ((uintptr_t)arg1 >= 0xfffff001)
                    var_58 = 0;
                else
                {
                    
                    if (!$v1_17)
                        var_58 = 0;
                    else if ($(uintptr_t)v1_17 < 0xfffff001)
                    {
                        $v0_13 = *($v1_17 + 0xd4);
                        var_58 = 0;
                    }
                    else
                        var_58_22 = 0;
                }
                
                if (*($v0_13 + 0x15c) == 1)
                {
                        return 0;
                    return 0;
                    $v1_7 = *($v0_13 + 0x1c8);
                    
                    if (!$v1_7)
                    
                    $v1_7(*($v0_13 + 0x1d0), arg3);
                }
                
                result = 0;
                
                if (!(arg1[5] & 0x20))
                {
                    char* $s0_2 = (char*)(arg1[8]); // Fixed void pointer assignment
                    
                    if ($s0_2)
                    {
                        __private_spin_lock_irqsave($s0_2 + 0x9c, &var_58);
                        tisp_channel_fifo_clear(arg1[1]);
                        result = 0;
                        private_spin_unlock_irqrestore($s0_2 + 0x9c, var_58);
                    }
                }
                break;
            }
        }
    return result;
}

