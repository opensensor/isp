#include "include/main.h"


  int32_t tx_isp_vic_start(void* arg1)

{
    int32_t* $v1 = (int32_t*)((char*)arg1  + 0x110); // Fixed void pointer arithmetic
    int32_t $v0 = *($v1 + 0x14);
    int32_t* $v0_47;
        int32_t $v0_2;
        void* $v1_2;
    int32_t entry_$a2;
    
    if ($v0 == 1)
    {
        
        if (*($v1 + 0x18) != $v0)
        {
            isp_printf(); // Fixed: macro with no parameters, removed 5 arguments;
            $v1_2 = *(arg1 + 0xb8);
            $v0_2 = 0xa000a;
        }
        else
        {
            isp_printf(); // Fixed: macro with no parameters, removed 3 arguments;
            *(*(arg1 + 0xb8) + 0x10) = &data_20000;
            $v1_2 = *(arg1 + 0xb8);
            $v0_2 = 0x100010;
        }
        
        *((int32_t*)((char*)$v1_2 + 0x1a4)) = $v0_2; // Fixed void pointer dereference
        int32_t* $a0 = (int32_t*)((char*)arg1  + 0x110); // Fixed void pointer arithmetic
        int32_t $v1_3 = *($a0 + 0x7c);
        int32_t $v0_3 = 8;
        
        if ($v1_3)
        {
            $v0_3 = 0xa;
            
            if ($v1_3 != 1)
            {
                $v0_3 = 0xc;
                
                if ($v1_3 != 2)
                {
                    $v0_3 = 0x10;
                    
                    if ($v1_3 != 7)
                        $v0_3 = 0;
                }
            }
        }
        
        int32_t $v0_4 = $v0_3 * *($a0 + 0x2c);
        *(*(arg1 + 0xb8) + 0x100) = ($v0_4 >> 5) + (0 < ($v0_4 & 0x1f) ? 1 : 0);
        *(*(arg1 + 0xb8) + 0xc) = 2;
        *(*(arg1 + 0xb8) + 0x14) = *(*(arg1 + 0x110) + 0x7c);
        *(*(arg1 + 0xb8) + 4) = *(arg1 + 0xdc) << 0x10 | *(arg1 + 0xe0);
        int32_t* $a3_1 = (int32_t*)((char*)arg1  + 0x110); // Fixed void pointer arithmetic
        int32_t $v0_16 = *($a3_1 + 0x40) << 0x19 | *($a3_1 + 0x44) << 0x18 | *($a3_1 + 0x78)
            | *($a3_1 + 0x48) << 0x17 | *($a3_1 + 0x5c) << 0x16 | *($a3_1 + 0x60) << 0x14;
        int32_t $a2_2 = $v0_16 | *($a3_1 + 0x64) << 0x12;
        *(*(arg1 + 0xb8) + 0x10c) = $a2_2 | *($a3_1 + 0x68) << 0xc | *($a3_1 + 0x6c) << 8
            | *($a3_1 + 0x74) << 4 | *($a3_1 + 0x70) << 2;
        int32_t* $v1_19 = (int32_t*)((char*)arg1  + 0x110); // Fixed void pointer arithmetic
        *(*(arg1 + 0xb8) + 0x110) = *($v1_19 + 0x2c) << 0x10 | *($v1_19 + 0x4c);
        *(*(arg1 + 0xb8) + 0x114) = *(*(arg1 + 0x110) + 0x50);
        *(*(arg1 + 0xb8) + 0x118) = *(*(arg1 + 0x110) + 0x54);
        *(*(arg1 + 0xb8) + 0x11c) = *(*(arg1 + 0x110) + 0x58);
        int32_t $v0_32 = *(*(arg1 + 0x110) + 0x74);
        void* $v0_34;
        int32_t $v0_33;
        void* $v1_25;
        
        if ($v0_32)
        {
                goto label_1043c;
            if ($v0_32 == 1)
            {
                $v1_25 = *(arg1 + 0xb8);
                $v0_33 = 0x4140;
            }
            
            if ($v0_32 == 2)
            {
                goto label_1043c;
                $v1_25 = *(arg1 + 0xb8);
                $v0_33 = 0x4240;
            }
            
            isp_printf(); // Fixed: macro with no parameters, removed 3 arguments;
            $v0_34 = *(arg1 + 0xb8);
        }
        else
        {
            $v1_25 = *(arg1 + 0xb8);
            $v0_33 = 0x4440;
        label_1043c:
            *((int32_t*)((char*)$v1_25 + 0x1ac)) = $v0_33; // Fixed void pointer dereference
            *(*(arg1 + 0xb8) + 0x1a8) = $v0_33;
            $v0_34 = *(arg1 + 0xb8);
        }
        *((int32_t*)((char*)$v0_34 + 0x1b0)) = 0x10; // Fixed void pointer dereference
        **((int32_t*)((char*)arg1 + 0xb8)) = 2; // Fixed void pointer dereference
        **((int32_t*)((char*)arg1 + 0xb8)) = 4; // Fixed void pointer dereference
        int32_t* $v1_27 = (int32_t*)((char*)arg1  + 0x110); // Fixed void pointer arithmetic
        *(*(arg1 + 0xb8) + 0x1a0) = *($v1_27 + 0x74) << 4 | *($v1_27 + 0x78);
        int32_t* $v1_30 = *(arg1 + 0xb8);
        
        while (*$v1_30)
            /* nop */
        
        int32_t* $a0_9 = (int32_t*)((char*)arg1  + 0x110); // Fixed void pointer arithmetic
        $v1_30[0x41] = *($a0_9 + 0x52) << 0x10 | *($a0_9 + 0x4e);
        int32_t* $v1_31 = (int32_t*)((char*)arg1  + 0x110); // Fixed void pointer arithmetic
        *(*(arg1 + 0xb8) + 0x108) = *($v1_31 + 0x5a) << 0x10 | *($v1_31 + 0x56);
        $v0_47 = *(arg1 + 0xb8);
        goto label_107d4;
    }
    
    void* $v0_125;
    int32_t result;
    int32_t* $v0_58;
    int32_t $a2_3;
    
    if ($v0 != 5)
    {
                return 0xffffffff;
        if ($v0 == 4)
        {
            $a2_3 = isp_printf(); // Fixed: macro with no parameters, removed 3 arguments;
            *(*(arg1 + 0xb8) + 0xc) = 0;
            
            if (*(*(arg1 + 0x110) + 0x18))
            {
                isp_printf(); // Fixed: macro with no parameters, removed 3 arguments;
            }
            
            *(*(arg1 + 0xb8) + 4) = *(arg1 + 0xdc) << 0x10 | *(arg1 + 0xe0);
            *(*(arg1 + 0xb8) + 0xc) = 0;
            *(*(arg1 + 0xb8) + 0x10) = 0x800c0000;
            *(*(arg1 + 0xb8) + 0x18) = *(arg1 + 0xdc) << 1;
            *(*(arg1 + 0xb8) + 0x1a4) = 0x100010;
            *(*(arg1 + 0xb8) + 0x1ac) = 0x4440;
            *(*(arg1 + 0xb8) + 0x1d0) = 0x200;
            *(*(arg1 + 0xb8) + 0x1d4) = 0x200;
            $v0_58 = *(arg1 + 0xb8);
            goto label_1065c;
        }
        
        if ($v0 != 3)
        {
                return 0xffffffff;
            if ($v0 != 2)
            {
                isp_printf(); // Fixed: macro with no parameters, removed 3 arguments;
            }
            
            *(*(arg1 + 0xb8) + 0xc) = 3;
            int32_t $v0_95 = *(arg1 + 0xe4);
            void* $v1_50;
            void* $v0_96;
            int32_t $v1_57;
            char const* const $a1_6;
            
            if ($(uintptr_t)v0_95 >= 0x3010)
            {
                            goto label_10928;
                            goto label_109ac;
                if ($(uintptr_t)v0_95 >= 0x3110)
                {
                    if ($(uintptr_t)v0_95 >= 0x3200)
                    {
                        if ($(uintptr_t)v0_95 < 0x3210)
                        
                        if ($v0_95 - (uintptr_t)0x3300 < 0x10)
                        {
                            $v0_96 = *(arg1 + 0x110);
                        }
                        
                        int32_t var_18_6 = $v0_95;
                        goto label_109d4;
                    }
                    
                    int32_t var_18_5 = $v0_95;
                label_109d4:
                    isp_printf(); // Fixed: macro with no parameters, removed 3 arguments;
                    $v1_50 = nullptr;
                    result = 0xffffffff;
                }
                else
                {
                    void* $v0_98;
                        int32_t $v0_99 = *($v0_98 + 0x18);
                    
                    if ($(uintptr_t)v0_95 >= 0x3100)
                    {
                        $v0_98 = *(arg1 + 0x110);
                    label_108fc:
                        $v1_50 = nullptr;
                        
                        if ($v0_99 == 3)
                            result = 0;
                        else if ($v0_99 == 4)
                        {
                            $v1_50 = 0x100000;
                            result = 0;
                        }
                        else
                        {
                            $a1_6 = "%s[%d] VIC failed to config DVP mode!(8bits-sensor)\n";
                        label_10990:
                            isp_printf(); // Fixed: macro with no parameters, removed 3 arguments;
                            $v1_50 = nullptr;
                            result = 0xffffffff;
                        }
                    }
                    else
                    {
                                goto label_108fc;
                        if ($(uintptr_t)v0_95 >= 0x3013)
                        {
                            $v1_57 = $(uintptr_t)v0_95 < 0x3015 ? 1 : 0;
                        label_108b8:
                            
                            if ($v1_57)
                            {
                                $v0_98 = *(arg1 + 0x110);
                            }
                            
                            int32_t var_18_4 = $v0_95;
                            goto label_109d4;
                        }
                        
                        $v0_96 = *(arg1 + 0x110);
                    label_109ac:
                        $v1_50 = &data_40000;
                        
                        if (*($v0_96 + 0x1c) == 2)
                            $v1_50 = &data_50000;
                        
                        result = 0;
                    }
                }
            }
            else
            {
                void* $v0_97;
                    int32_t $v0_100 = *($v0_97 + 0x18);
                                goto label_10990;
                
                if ($(uintptr_t)v0_95 >= 0x300e)
                {
                label_10928:
                    $v0_97 = *(arg1 + 0x110);
                label_10938:
                    
                    if (*($v0_97 + 0x1c) != 2)
                    {
                        $v1_50 = &data_20000;
                        
                        if (!$v0_100)
                            result = 0;
                        else
                        {
                            if ($v0_100 != 1)
                            {
                                $a1_6 = "%s[%d] VIC failed to config DVP mode!(10bits-sensor)\n";
                            }
                            
                            $v1_50 = 0x120000;
                            result = 0;
                        }
                    }
                    else
                    {
                                goto label_10990;
                        $v1_50 = &data_30000;
                        
                        if (!$v0_100)
                            result = 0;
                        else
                        {
                            if ($v0_100 != 1)
                            {
                                $a1_6 =
                                    "%s[%d] VIC failed to config DVP SONY mode!(10bits-sensor)\n";
                            }
                            
                            $v1_50 = 0x130000;
                            result = 0;
                        }
                    }
                }
                else
                {
                                goto label_10928;
                                goto label_108b8;
                    $v1_50 = &data_c0000;
                    
                    if ($(uintptr_t)v0_95 == 0x2011)
                        result = 0;
                    else
                    {
                        if ($(uintptr_t)v0_95 >= 0x2012)
                        {
                            if ($(uintptr_t)v0_95 == 0x3007)
                            
                            if ($(uintptr_t)v0_95 < 0x3008)
                            {
                                $v1_57 = $v0_95 - 0x3001 < 2 ? 1 : 0;
                            }
                            
                            if ($(uintptr_t)v0_95 == 0x3008)
                            {
                                goto label_109ac;
                                $v0_96 = *(arg1 + 0x110);
                            }
                            
                            if ($(uintptr_t)v0_95 == 0x300a)
                            {
                                goto label_10938;
                                $v0_97 = *(arg1 + 0x110);
                            }
                            
                            int32_t var_18_3 = $v0_95;
                            goto label_109d4;
                        }
                        
                        $v1_50 = &data_80000;
                        
                        if ($(uintptr_t)v0_95 == 0x1008)
                            result = 0;
                        else if ($(uintptr_t)v0_95 >= 0x1009)
                        {
                                int32_t var_18_2 = $v0_95;
                                goto label_109d4;
                            if ($v0_95 - 0x2002 >= 4)
                            {
                            }
                            
                            $v1_50 = &data_c0000;
                            result = 0;
                        }
                        else
                        {
                                int32_t var_18_1 = $v0_95;
                                goto label_109d4;
                            $v1_50 = &data_a0000;
                            result = 0;
                            
                            if ($(uintptr_t)v0_95 != 0x1006)
                            {
                            }
                        }
                    }
                }
            }
            int32_t* $v0_102 = (int32_t*)((char*)arg1  + 0x110); // Fixed void pointer arithmetic
            
            if (*($v0_102 + 0x24) == 2)
                $v1_50 |= 2;
            
            uint32_t $v0_103 = *($v0_102 + 0x22);
            
            if (*($v0_102 + 0x25) == 2)
                $v1_50 |= 1;
            
            if ($v0_103)
                *(*(arg1 + 0xb8) + 0x18) = ($v0_103 << 0x10) + *(arg1 + 0xdc);
            
            uint32_t $v0_107 = *(*(arg1 + 0x110) + 0x20);
            void* $v0_108;
            
            if (!$v0_107)
                $v0_108 = *(arg1 + 0x110);
            else
            {
                *(*(arg1 + 0xb8) + 0x3c) = $v0_107;
                $v0_108 = *(arg1 + 0x110);
            }
            
            *(*(arg1 + 0xb8) + 0x18) = (*($v0_108 + 0x22) << 0x10) + *(arg1 + 0xdc);
            *(*(arg1 + 0xb8) + 0x10) = *(*(arg1 + 0x110) + 0x28) << 0x1f | $v1_50;
            *(*(arg1 + 0xb8) + 4) = *(arg1 + 0xdc) << 0x10 | *(arg1 + 0xe0);
            **((int32_t*)((char*)arg1 + 0xb8)) = 2; // Fixed void pointer dereference
            **((int32_t*)((char*)arg1 + 0xb8)) = 4; // Fixed void pointer dereference
            int32_t* $v0_121 = *(arg1 + 0xb8);
            
            while (*$v0_121)
                /* nop */
            
            *$v0_121 = 1;
            *(*(arg1 + 0xb8) + 0x1a4) = 0x100010;
            *(*(arg1 + 0xb8) + 0x1ac) = 0x4210;
            *(*(arg1 + 0xb8) + 0x1b0) = 0x10;
            *(*(arg1 + 0xb8) + 0x1b4) = 0;
            $v0_125 = *(arg1 + 0x110);
        }
        else
        {
            int32_t $v1_44 = *(*(arg1 + 0x110) + 0x18);
            int32_t $v0_79;
            void* $v1_47;
                    return 0xffffffff;
            $a2_3 = isp_printf(); // Fixed: macro with no parameters, removed 3 arguments;
            *(*(arg1 + 0xb8) + 0xc) = 1;
            
            if ($v1_44)
            {
                if ($v1_44 != 1)
                {
                    isp_printf(); // Fixed: macro with no parameters, removed 3 arguments;
                }
                
                *(*(arg1 + 0xb8) + 4) = *(arg1 + 0xdc) << 0x10 | *(arg1 + 0xe0);
                *(*(arg1 + 0xb8) + 0xc) = $v1_44;
                $v1_47 = *(arg1 + 0xb8);
                $v0_79 = 0x88060820;
            }
            else
            {
                *(*(arg1 + 0xb8) + 4) = *(arg1 + 0xdc) << 0x10 | *(arg1 + 0xe0);
                *(*(arg1 + 0xb8) + 0xc) = 1;
                $v1_47 = *(arg1 + 0xb8);
                $v0_79 = 0x800c8000;
            }
            
            *((int32_t*)((char*)$v1_47 + 0x10)) = $v0_79; // Fixed void pointer dereference
            *(*(arg1 + 0xb8) + 0x18) = *(arg1 + 0xdc) << 1 | 0x100000;
            *(*(arg1 + 0xb8) + 0x3c) = 0x30;
            *(*(arg1 + 0xb8) + 0x1c) = 0x1b8;
            *(*(arg1 + 0xb8) + 0x30) = 0x1402d0;
            *(*(arg1 + 0xb8) + 0x34) = 0x50014;
            *(*(arg1 + 0xb8) + 0x38) = 0x2d00014;
            *(*(arg1 + 0xb8) + 0x1a0) = 0;
            *(*(arg1 + 0xb8) + 0x1a4) = 0x100010;
            *(*(arg1 + 0xb8) + 0x1ac) = 0x4440;
            **((int32_t*)((char*)arg1 + 0xb8)) = 2; // Fixed void pointer dereference
            isp_printf(); // Fixed: macro with no parameters, removed 3 arguments);
            $v0_47 = *(arg1 + 0xb8);
        label_107d4:
            *$v0_47 = 1;
            result = 0;
            $v0_125 = *(arg1 + 0x110);
        }
    }
    else
    {
            return 0xffffffff;
        $a2_3 = isp_printf(); // Fixed: macro with no parameters, removed 3 arguments;
        *(*(arg1 + 0xb8) + 0xc) = 4;
        
        if (*(*(arg1 + 0x110) + 0x18))
        {
            isp_printf(); // Fixed: macro with no parameters, removed 3 arguments;
        }
        
        *(*(arg1 + 0xb8) + 4) = *(arg1 + 0xdc) << 0x10 | *(arg1 + 0xe0);
        *(*(arg1 + 0xb8) + 0x10) = 0x800c0000;
        *(*(arg1 + 0xb8) + 0x18) = *(arg1 + 0xdc) << 1;
        *(*(arg1 + 0xb8) + 0x1a4) = 0x100010;
        *(*(arg1 + 0xb8) + 0x1ac) = 0x4440;
        $v0_58 = *(arg1 + 0xb8);
    label_1065c:
        *$v0_58 = 2;
        isp_printf(); // Fixed: macro with no parameters, removed 3 arguments);
        **((int32_t*)((char*)arg1 + 0xb8)) = 1; // Fixed void pointer dereference
        result = 0;
        $v0_125 = *(arg1 + 0x110);
    }
    char const* const $a1_9;
    
    if (*($v0_125 + 0x90))
        $a1_9 = "%s:%d::wdr mode\\n";
    else
        $a1_9 = "%s:%d::linear mode\\n";
    
    isp_printf(); // Fixed: macro with no parameters, removed 3 arguments;
    vic_start_ok = 1;
    return result;
}

