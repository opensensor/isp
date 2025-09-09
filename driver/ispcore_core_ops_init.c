#include "include/main.h"


  int32_t ispcore_core_ops_init(void* arg1, int32_t arg2)

{
    char* $s0 = (char*)(nullptr); // Fixed void pointer assignment
    int32_t var_18 = 0;
    int32_t result = 0xffffffea;
        int32_t $v0_3 = *($s0 + 0xe8);
                int32_t $v1_55;
    
    if (arg1 && (uintptr_t)arg1 < 0xfffff001)
        $s0 = *(arg1 + 0xd4);
    
    
    if ($s0 && $(uintptr_t)s0 < 0xfffff001)
    {
        result = 0;
        
        if ($v0_3 != 1)
        {
            if (!(uintptr_t)arg2)
            {
                
                if ($v0_3 != 4)
                    $v1_55 = *($s0 + 0xe8);
                else
                {
                    ispcore_video_s_stream(arg1, 0);
                    $v1_55 = *($s0 + 0xe8);
                }
                
                if ($v1_55 == 3)
                {
                    private_kthread_stop(*($s0 + 0x1b8));
                    *((int32_t*)((char*)$s0 + 0xe8)) = 2; // Fixed void pointer dereference
                }
                
                tisp_deinit();
                memset(*($s0 + 0x1bc) + 4, 0, 0x40a4);
                memset($s0 + 0x1d8, 0, 0x40);
                return 0;
            }
            
            int32_t var_78_20;
            memset(&var_78_21, 0, 0x60);
            int32_t result_1 = private_reset_tx_isp_module(0);
            result = result_1;
            
            if (result_1)
            {
                return 0xffffffea;
                isp_printf(); // Fixed: macro with no parameters, removed 3 arguments);
            }
            
            __private_spin_lock_irqsave($s0 + 0xdc, &var_18_21);
            int32_t $a1_1 = var_18_22;
            
            if (*($s0 + 0xe8) != 2)
            {
                return 0xffffffff;
                private_spin_unlock_irqrestore($s0 + 0xdc, $a1_1);
                isp_printf(); // Fixed: macro with no parameters, removed 3 arguments);
            }
            
            int32_t $a2_3 = private_spin_unlock_irqrestore($s0 + 0xdc, $a1_1);
            int32_t $v1_1 = *($s0 + 0xec);
            int32_t $v0_5;
            
            if ($(uintptr_t)v1_1 < 0x1001)
                $v0_5 = *($s0 + 0xf0);
            
            int32_t $v0_6;
            
            if ($(uintptr_t)v1_1 < 0x1001 && $(uintptr_t)v0_5 < 0x1001)
            {
                int32_t $v1_2 = *($s0 + 0xe8);
                    int32_t i = 0;
                    char* $a2_4 = (char*)(&isp_output_fmt[*($s0 + 0x14c) * 0x2c]); // Fixed void pointer assignment
                        char* $v0_11 = (char*)(i * 0xc4 + *($s0 + 0x150)); // Fixed void pointer assignment
                            uint32_t $a0_7 = *($s0 + 0x140);
                            uint32_t $a3_1 = *($s0 + 0x142);
                            uint32_t $v1_7 = ($a0_7 * *($a2_4 + 0x24)) >> 3;
                *((int32_t*)((char*)$s0 + 0x140)) = $v1_1; // Fixed void pointer dereference
                *((int32_t*)((char*)$s0 + 0x142)) = $v0_5; // Fixed void pointer dereference
                
                if ($v1_2 != 4)
                {
                    
                    while (i < *($s0 + 0x154))
                    {
                        i += 1;
                        
                        if (*($v0_11 + 0x74))
                        {
                            *((int32_t*)((char*)$v0_11 + 4)) = $a0_7; // Fixed void pointer dereference
                            *((int32_t*)((char*)$v0_11 + 8)) = $a3_1; // Fixed void pointer dereference
                            *((int32_t*)((char*)$v0_11 + 0xc)) = *($a2_4 + 0x20); // Fixed void pointer dereference
                            *((int32_t*)((char*)$v0_11 + 0x14)) = $v1_7; // Fixed void pointer dereference
                            *((int32_t*)((char*)$v0_11 + 0x18)) = $a3_1 * $v1_7; // Fixed void pointer dereference
                        }
                    }
                }
                
                $v0_6 = *($s0 + 0xf4);
            }
            else
            {
                isp_printf(); // Fixed: macro with no parameters, removed 3 arguments;
                $v0_6 = *($s0 + 0xf4);
            }
            
            if ($(uintptr_t)v0_6 == 0x310f)
                int32_t var_70_4_1 = 0x13;
            else
            {
                int32_t $v0_12;
                        int32_t $v0_16;
                        int32_t $v0_17;
                        int32_t $v1_25;
                                        int32_t var_80_5 = $v0_6;
                
                if ($(uintptr_t)v0_6 >= 0x3110)
                {
                    if ($(uintptr_t)v0_6 == 0x320f)
                        var_70_4 = 0x13;
                    else
                    {
                        
                        if ($(uintptr_t)v0_6 >= 0x3210)
                        {
                            if ($(uintptr_t)v0_6 == 0x3307)
                                var_70_4 = 0xb;
                            else if ($(uintptr_t)v0_6 >= 0x3308)
                            {
                                if ($(uintptr_t)v0_6 == 0x330b)
                                    var_70_4 = 0xf;
                                else if ($(uintptr_t)v0_6 >= 0x330c)
                                {
                                    if ($(uintptr_t)v0_6 == 0x330d)
                                        var_70_4 = 0x11;
                                    else if ($(uintptr_t)v0_6 < 0x330d)
                                        var_70_4 = 0x10;
                                    else if ($(uintptr_t)v0_6 == 0x330e)
                                        var_70_4 = 0x12;
                                    else if ($(uintptr_t)v0_6 == 0x330f)
                                        var_70_4 = 0x13;
                                    else
                                    {
                                        isp_printf(2, "Err [VIC_INT] : mipi ch2 vcomp err !!!\n", 
                                            "ispcore_core_ops_init");
                                    }
                                }
                                else if ($(uintptr_t)v0_6 == 0x3309)
                                    var_70_4_2 = 0xd;
                                else
                                {
                                    $v0_17 = $(uintptr_t)v0_6 < 0x330a ? 1 : 0;
                                label_78e54:
                                    
                                    var_70_4 = $v0_17 ? 0xc : 0xe;
                                }
                            }
                            else if ($(uintptr_t)v0_6 == 0x3303)
                                var_70_4_3 = 7;
                            else if ($(uintptr_t)v0_6 >= 0x3304)
                            {
                                if ($(uintptr_t)v0_6 == 0x3305)
                                    var_70_4 = 9;
                                else
                                {
                                    $v0_16 = $(uintptr_t)v0_6 < 0x3306 ? 1 : 0;
                                label_78e24:
                                    
                                    var_70_4 = $v0_16 ? 8 : 0xa;
                                }
                            }
                            else if ($(uintptr_t)v0_6 == 0x3301)
                                var_70_4_4 = 5;
                            else
                            {
                                        int32_t var_80_4 = $v0_6;
                                $v1_25 = 0x3300;
                                
                                if ($(uintptr_t)v0_6 >= 0x3302)
                                    var_70_4 = 6;
                                else
                                {
                                label_78e08:
                                    
                                    if ($v0_6 == $v1_25)
                                        var_70_4 = 4;
                                    else
                                    {
                                        isp_printf(2, "Err [VIC_INT] : mipi ch2 vcomp err !!!\n", 
                                            "ispcore_core_ops_init");
                                    }
                                }
                            }
                        }
                        else if ($(uintptr_t)v0_6 == 0x3207)
                            var_70_4_5 = 0xb;
                        else if ($(uintptr_t)v0_6 >= 0x3208)
                        {
                            if ($(uintptr_t)v0_6 == 0x320b)
                                var_70_4 = 0xf;
                            else if ($(uintptr_t)v0_6 >= 0x320c)
                            {
                                if ($(uintptr_t)v0_6 == 0x320d)
                                    $v0_12 = 0x11;
                                else if ($(uintptr_t)v0_6 < 0x320e)
                                    $v0_12 = 0x10;
                                else
                                    $v0_12 = 0x12;
                                
                                var_70_4 = $v0_12;
                            }
                            else
                            {
                                    goto label_78e54;
                                if ($(uintptr_t)v0_6 != 0x3209)
                                {
                                    $v0_17 = $(uintptr_t)v0_6 < 0x320a ? 1 : 0;
                                }
                                
                                var_70_4_6 = 0xd;
                            }
                        }
                        else if ($(uintptr_t)v0_6 == 0x3203)
                            var_70_4_7 = 7;
                        else if ($(uintptr_t)v0_6 >= 0x3204)
                        {
                                goto label_78e24;
                            if ($(uintptr_t)v0_6 != 0x3205)
                            {
                                $v0_16 = $(uintptr_t)v0_6 < 0x3206 ? 1 : 0;
                            }
                            
                            var_70_4_8 = 9;
                        }
                        else if ($(uintptr_t)v0_6 == 0x3201)
                            var_70_4_9 = 5;
                        else
                        {
                                goto label_78e08;
                            $v1_25 = 0x3200;
                            
                            if ($(uintptr_t)v0_6 < 0x3202)
                            
                            var_70_4 = 6;
                        }
                    }
                }
                else if ($(uintptr_t)v0_6 == 0x3013)
                    var_70_4_10 = 3;
                else if ($(uintptr_t)v0_6 >= 0x3014)
                {
                    if ($(uintptr_t)v0_6 == 0x3106)
                        var_70_4 = 0xa;
                    else if ($(uintptr_t)v0_6 >= 0x3107)
                    {
                        if ($(uintptr_t)v0_6 == 0x310a)
                            var_70_4 = 0xe;
                        else if ($(uintptr_t)v0_6 < 0x310b)
                        {
                            if ($(uintptr_t)v0_6 == 0x3108)
                                $v0_12 = 0xc;
                            else if ($(uintptr_t)v0_6 < 0x3109)
                                $v0_12 = 0xb;
                            else
                                $v0_12 = 0xd;
                            
                            var_70_4 = $v0_12;
                        }
                        else if ($(uintptr_t)v0_6 == 0x310c)
                            var_70_4_11 = 0x10;
                        else if ($(uintptr_t)v0_6 < 0x310c)
                            var_70_4_12 = 0xf;
                        else if ($(uintptr_t)v0_6 == 0x310d)
                            var_70_4_13 = 0x11;
                        else if ($(uintptr_t)v0_6 == 0x310e)
                            var_70_4_14 = 0x12;
                        else
                        {
                            int32_t var_80_3 = $v0_6;
                            isp_printf(2, "Err [VIC_INT] : mipi ch2 vcomp err !!!\n", 
                                "ispcore_core_ops_init");
                        }
                    }
                    else if ($(uintptr_t)v0_6 == 0x3102)
                        var_70_4_15 = 6;
                    else if ($(uintptr_t)v0_6 >= 0x3103)
                    {
                        if ($(uintptr_t)v0_6 == 0x3104)
                            $v0_12 = 8;
                        else if ($(uintptr_t)v0_6 < 0x3105)
                            $v0_12 = 7;
                        else
                            $v0_12 = 9;
                        
                        var_70_4 = $v0_12;
                    }
                    else if ($(uintptr_t)v0_6 == 0x3100)
                        var_70_4_16 = 4;
                    else if ($(uintptr_t)v0_6 >= 0x3101)
                        var_70_4_17 = 5;
                    else if ($(uintptr_t)v0_6 == 0x3014)
                        int32_t var_70_3_1 = 0;
                    else
                    {
                        int32_t var_80_2 = $v0_6;
                        isp_printf(2, "Err [VIC_INT] : mipi ch2 vcomp err !!!\n", 
                            "ispcore_core_ops_init");
                    }
                }
                else if ($(uintptr_t)v0_6 == 0x300b)
                    var_70_4_18 = 1;
                else if ($(uintptr_t)v0_6 >= 0x300c)
                {
                        int32_t var_70_5 = 0;
                            int32_t var_70_1 = 0;
                    if ($(uintptr_t)v0_6 == 0x300f)
                    else if ($(uintptr_t)v0_6 < 0x3010)
                    {
                        if ($(uintptr_t)v0_6 != 0x300d)
                            var_70_4 = 3;
                        else
                    }
                    else if ($(uintptr_t)v0_6 == 0x3011)
                        var_70_4_19 = 2;
                    else if ($(uintptr_t)v0_6 < 0x3012)
                        var_70_4_20 = 3;
                    else
                        int32_t var_70_2_1 = 0;
                }
                else if ($(uintptr_t)v0_6 >= 0x3009)
                    var_70_4_21 = 2;
                else if ($(uintptr_t)v0_6 >= 0x3003 || $(uintptr_t)v0_6 == 0x3001)
                    var_70_4_22 = 1;
                else if ($(uintptr_t)v0_6 >= 0x3002)
                    var_70_4_23 = 2;
                else if ($(uintptr_t)v0_6 != 0x2011)
                {
                    int32_t var_80_1 = $v0_6;
                    isp_printf(2, "Err [VIC_INT] : mipi ch2 vcomp err !!!\n", 
                        "ispcore_core_ops_init");
                }
                else
                    var_70_4_24 = 0x14;
            }
            
            void var_6c_7;
            char* $v1_32 = (char*)(&var_6c_8); // Fixed void pointer assignment
            var_78_22 = *($s0 + 0x124);
            int32_t var_74_1_1 = *($s0 + 0x128);
            char* $v0_22 = **($s0 + 0x120);
            uint32_t i_1;
            
            do
            {
            int32_t* $v0_23 = (int32_t*)((char*)$s0  + 0x120); // Fixed void pointer arithmetic
            int32_t $a1_2 = *($s0 + 0x12c);
            uint32_t $a2_5 = *($v0_23 + 0xb2);
            int32_t var_5c_1 = *($v0_23 + 0x94);
            int32_t var_4c_1 = $a1_2;
            int32_t var_58_1 = *($v0_23 + 0x98);
            int32_t var_54_1 = *($v0_23 + 0x9c);
            int32_t var_20_1 = *($v0_23 + 0xe4);
            int32_t var_50_1 = *($v0_23 + 0xa0);
            int16_t var_48_1 = *($v0_23 + 0xa4);
            int16_t var_2e_1 = *($v0_23 + 0xd8);
            int16_t var_2c_1 = *($v0_23 + 0xda);
            int16_t var_46_1 = *($v0_23 + 0xa6);
            int16_t var_44_1 = *($v0_23 + 0xa8);
            int16_t var_42_1 = *($v0_23 + 0xaa);
            int32_t var_40_1 = *($v0_23 + 0xac);
            int32_t var_28_1 = *($v0_23 + 0xdc);
            int16_t var_3c_1 = *($v0_23 + 0xb0);
            uint16_t var_3a_1 = $a2_5;
            int16_t var_38_1 = *($v0_23 + 0xb4);
            int16_t var_36_1 = *($v0_23 + 0xb6);
            int16_t var_34_1 = *($v0_23 + 0xb8);
            int16_t var_32_1 = *($v0_23 + 0xba);
            int32_t var_24_1 = *($v0_23 + 0xe0);
            int16_t var_30_1 = *(*($s0 + 0x120) + 0xbc);
            int32_t var_1c_1 = *($s0 + 0x17c);
            int32_t $v0_27 =
                return 0xffffffea;
                i_1 = *$v0_22;
                $v0_22 = &$v0_22[1];
                *$v1_32 = i_1;
                $v1_32 += 1;
            } while (i_1);
            *((int32_t*)((char*)$v0_23 + 0xbc)) = ($a1_2 & 0xffff) * 0xf4240 / ($a1_2 >> 0x10) / $a2_5; // Fixed void pointer dereference
            tisp_init(&var_78, $s0 + 0x1d8);
                private_kthread_run(isp_fw_process, 0, "Err [VIC_INT] : mipi ch3 vcomp err !!!\n");
            *((int32_t*)((char*)$s0 + 0x1b8)) = $v0_27; // Fixed void pointer dereference
            
            if (!$v0_27 || $(uintptr_t)v0_27 >= 0xfffff001)
            {
                isp_printf(2, "Err [VIC_INT] : dma arb trans done ovf!!!\n", 
                    "ispcore_core_ops_init");
            }
            
            *((int32_t*)((char*)$s0 + 0xe8)) = 3; // Fixed void pointer dereference
        }
    }
    
    return result;
}

