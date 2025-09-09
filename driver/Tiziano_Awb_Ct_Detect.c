#include "include/main.h"


  int32_t* Tiziano_Awb_Ct_Detect(void* arg1, int32_t* arg2, int32_t* arg3, int32_t arg4, int32_t* arg5, int32_t* arg6, void* arg7, int32_t arg8, int32_t arg9, int32_t* arg10, int32_t* arg11, void* arg12, void* arg13, int32_t* arg14, int32_t* arg15, void* arg16, int32_t* arg17, int32_t* arg18, int32_t* arg19, int32_t* arg20)

{
    int32_t* arg_8 = arg3;
    int32_t arg_c = arg4;
    int32_t $v1_7 = *arg14;
    int32_t* $s0 = arg2;
    int32_t $v1_8 = arg14[1];
    int32_t $v0_1 = arg14[2];
    int32_t* $s1 = arg3;
    int32_t $fp = *arg17;
    void var_118;
    int32_t $v0_5 = *arg6;
    int32_t $v0_6 = arg6[1];
    int32_t $v0_7 = *arg18;
    int32_t $v0_8 = arg18[1];
    int32_t $v0_9 = arg18[2];
    int32_t $v0_10 = arg18[3];
    int32_t $v0_11 = arg18[4];
    int32_t $v0_12 = arg18[5];
    int32_t var_a4;
    int32_t var_a0;
    memset(&var_118, 0, 0x50);
    
    if ($v0_5 != 1)
    {
        var_a0 = 0;
        var_a4 = 0;
    }
    else
    {
        int32_t $s4_1 = *arg5;
        int32_t $s5_1 = arg5[1];
        int32_t $s2_2 = (0x100 - $v0_6) << ($fp & 0x1f);
        var_a4 = fix_point_div_32($fp, $s2_2, (arg5[3] - arg5[2]) << ($fp & 0x1f));
        var_a0 = fix_point_div_32($fp, $s2_2, ($s4_1 - $s5_1) << ($fp & 0x1f));
    }
    
    char* var_8c_5 = (char*)(&var_118); // Fixed void pointer assignment
    char* $v1_11 = (char*)(var_8c_6); // Fixed void pointer assignment
    
    while (true)
    {
        $v1_11 += 4;
        
        if ($s1 == &$s1[arg4 * 2])
            break;
        
        *($v1_11 - 4) = *$s1 - 1;
        $s1 = &$s1[1];
    }
    
    int32_t $s4_2 = *$s0;
    int32_t $v0_23 = arg8 << 2;
    int32_t* $v1_12 = $s0;
    int32_t $v0_24 = 0;
    
    while (true)
    {
        int32_t* $a1_2 = $v1_12;
        int32_t $a0_6 = 0;
            int32_t temp1_1 = $a0_6;
            int32_t $a2_6 = *$a1_2;
        
        if ($v0_24 == arg9)
            break;
        
        
        while (true)
        {
            $a0_6 += 1;
            
            if (temp1_1 == arg8)
                break;
            
            
            if ($s4_2 < $a2_6)
                $s4_2 = $a2_6;
            
            $a1_2 = &$a1_2[1];
        }
        
        $v0_24 += 1;
        $v1_12 = &$v1_12[arg8];
    }
    
    int32_t $s1_1 = 0;
label_2a0c0:
    
    if ($s1_1 != arg9)
    {
        int32_t $s3_1 = 0;
                goto label_2a0c0;
        
        while (true)
        {
            if ($s3_1 == arg8)
            {
                $s1_1 += 1;
                $s0 = &$s0[arg8];
            }
            
            int32_t $s2_3 = $s3_1 << 2;
            
            if (!$s4_2)
            {
                *(&zone_pix_wgh + (($s1_1 * 0xf + $s3_1) << 2)) = 0;
                break;
            }
            
            $s3_1 += 1;
            *(&zone_pix_wgh + $s2_3 + $s1_1 * 0x3c) =
                (&data_20000_1 + 0x1044)($fp, $s0[$s3_1] << ($fp & 0x1f), $s4_2 << ($fp & 0x1f));
        }
        
        goto label_2b2cc;
    }
    
    char* var_80_1_1 = (char*)(arg1); // Fixed void pointer assignment
    char* $a0_8 = (char*)(arg1); // Fixed void pointer assignment
    int32_t $v0_28 = arg8 * arg9;
    int32_t $t1_1 = 0;
    int32_t $v0_29 = $v0_28 << 2;
    
    while ($t1_1 != arg9)
    {
        int32_t $a3_2 = 0;
        int32_t* $v1_15 = $v0_29 + $a0_8;
        int32_t* $v0_40 = $a0_8;
            int32_t $a2_8 = *$v0_40;
            int32_t $a1_5 = arg10[0xe] << ($fp & 0x1f);
                int32_t $a1_7 = *arg10 << ($fp & 0x1f);
        
        while ($a3_2 != arg8)
        {
            
            if ($a1_5 < $a2_8)
                *$v0_40 = $a1_5;
            else
            {
                
                if ($a2_8 < $a1_7)
                    *$v0_40 = $a1_7;
            }
            
            int32_t $a2_10 = *$v1_15;
            int32_t $a1_10 = arg11[0xe] << ($fp & 0x1f);
            
            if ($a1_10 < $a2_10)
                *$v1_15 = $a1_10;
            else
            {
                int32_t $a1_13 = *arg11 << ($fp & 0x1f);
                
                if ($a2_10 < $a1_13)
                    *$v1_15 = $a1_13;
            }
            
            $a3_2 += 1;
            $v1_15 = &$v1_15[1];
            $v0_40 = &$v0_40[1];
        }
        
        $t1_1 += 1;
        $a0_8 += $v0_23;
    }
    
    int32_t var_84_1_1 = 0;
    char* var_7c_1 = (char*)(arg1); // Fixed void pointer assignment
    char* var_34_1_4 = (char*)(&rgbg_wght); // Fixed void pointer assignment
    int32_t (* var_ac_2)(int32_t arg1, int32_t arg2, int32_t arg3, int32_t arg4, int32_t arg5, 
        int32_t arg6) = ISPAWBInterpolation1;
    void* void* var_30_1 = (void*)&data_30000; // Fixed function pointer assignment
    int32_t var_128;
    int32_t var_124;
    int32_t var_120;
    
    while (var_84_1_2 != arg9)
    {
        char* $s1_2 = (char*)(var_84_1 * 0x3c + var_34_1); // Fixed void pointer assignment
        int32_t var_78_1 = 0;
        char* var_a8_2 = (char*)($v0_29 + var_7c_1); // Fixed void pointer assignment
        char* $s3_2 = (char*)(var_7c_1); // Fixed void pointer assignment
        char* var_88_1 = (char*)(&Cluster_rgbg_index_num); // Fixed void pointer assignment
            int32_t $v1_20 = *$s3_2;
            int32_t $v0_52 = 0;
            int32_t* $a0_16;
                int32_t $a0_15 = arg10[0xe] << ($fp & 0x1f);
                        int32_t* $a1_17 = arg10;
                        int32_t $v0_53 = 0;
        
        while (var_78_1 != arg8)
        {
            
            if ($v1_20 < *arg10 << ($fp & 0x1f))
                $a0_16 = arg11;
            else
            {
                
                if ($a0_15 < $v1_20)
                    $a0_16 = arg11;
                else
                {
                    $v0_52 = 0xf;
                    
                    if ($v1_20 != $a0_15)
                    {
                        
                        while (true)
                        {
                            if ($v1_20 >= *$a1_17 << ($fp & 0x1f)
                                && $v1_20 < $a1_17[1] << ($fp & 0x1f))
                            {
                                $v0_52 = $v0_53 + 1;
                                break;
                            }
                            
                            $a1_17 = &$a1_17[1];
                            
                            if ($(uintptr_t)v0_53 == 0xe)
                            {
                                $v0_52 = $v0_53 + 1;
                                break;
                            }
                            
                            $v0_53 += 1;
                        }
                    }
                    
                    $a0_16 = arg11;
                }
            }
            
            int32_t i = *var_a8_2_1;
            int32_t $a0_28;
            
            if (i >= *$a0_16 << ($fp & 0x1f))
                $a0_28 = arg11[0xe] << ($fp & 0x1f);
            
            int32_t $v0_64;
            int32_t var_128_1;
            int32_t var_124_1;
            uint32_t $v0_123;
            int32_t $v1_52;
            int32_t $a0_38;
            int32_t $a1_22;
            int32_t $a2_15;
            int32_t $a3_4;
            int32_t $s0_2;
            
            if (i < *$a0_16 << ($fp & 0x1f) || $a0_28 < i)
            {
                    char* $v1_33 = (char*)((($a0_38 * 0xe + $v0_52 - 1) << 2) + var_88_1); // Fixed void pointer assignment
                        goto label_2a468;
                $s0_2 = 0;
                
                if ($(uintptr_t)v0_52 != 0xf)
                {
                    $a0_38 = 0xffffffff;
                label_2a454:
                    *$v1_33 += 1;
                    
                    if ($v0_52)
                    
                    *$s1_2 = 0;
                    $v0_64 = var_78_1;
                }
                else
                {
                    char* $v1_26 = (char*)(($s0_2 - 1) * 0x38 + var_88_1); // Fixed void pointer assignment
                label_2a3f4:
                    *($v1_26 + 0x34) += 1;
                label_2a468:
                    
                    if (!$s0_2)
                    {
                        *$s1_2 = 0;
                        $v0_64 = var_78_1;
                    }
                    else
                    {
                        int32_t $a0_51;
                            int32_t $s5_3 = ($s0_2 * 0xf + $v0_52 + 0x3ffffff0) << 2;
                            char* $s4_6 = (char*)(&arg10[$v0_52 + 0x3fffffff]); // Fixed void pointer assignment
                            char* $s2_12 = (char*)(arg10 + (($v0_52 + 0x3fffffff) << 2) + 4); // Fixed void pointer assignment
                            int32_t $v0_95 = var_ac_2($fp, *$s3_2, *$s4_6, *$s2_12, 
                            char* $s7_5 = (char*)(&arg11[$s0_2 + 0x3fffffff]); // Fixed void pointer assignment
                            char* $s0_10 = (char*)(arg11 + (($s0_2 + 0x3fffffff) << 2) + 4); // Fixed void pointer assignment
                                int32_t $v0_114 = var_ac_2($fp, *$s3_2, *$s4_6, *$s2_12, 
                                int32_t $v0_121 = var_ac_2($fp, *$s3_2, *$s4_6, *$s2_12, 
                        int32_t (* $v0_86)(int32_t arg1, int32_t arg2, int32_t arg3, int32_t arg4, 
                            int32_t arg5, int32_t arg6);
                        
                        if ($(uintptr_t)v0_52 != 0xf)
                        {
                                *(arg7 + $s5_3), *(arg7 + $s5_3 + 4));
                            var_124 = var_ac_2($fp, *$s3_2, *$s4_6, *$s2_12, 
                                *(arg7 + $s5_3 + 0x3c), *(arg7 + $s5_3 + 0x40));
                            var_128 = $v0_95;
                            *$s1_2 = (var_30_1 - 0x62f0)($fp, *var_a8_2, *$s7_5, *$s0_10, var_128, 
                                var_124, var_120);
                            
                            if ($v0_5 != 1)
                                $v0_64 = var_78_1;
                            else
                            {
                                    *(arg12 + $s5_3), *(arg12 + $s5_3 + 4));
                                    *(arg12 + $s5_3 + 0x3c), *(arg12 + $s5_3 + 0x40));
                                $a3_4 = *$s0_10;
                                $a1_22 = *var_a8_2;
                                $a2_15 = *$s7_5;
                                var_124_1 = $v0_121;
                                $v0_86 = var_30_1 - 0x62f0;
                                var_128_1 = $v0_114;
                                $a0_51 = $fp;
                            label_2a7a4:
                                $v0_123 =
                                    $v0_86($a0_51, $a1_22, $a2_15, $a3_4, var_128_1, var_124_1)
                                    >> ($fp & 0x1f);
                                var_120 = var_a0;
                                var_124 = var_a4;
                                $v1_52 = *$s1_2;
                            label_2a7c8:
                                var_128 = $v1_52;
                                *$s1_2 = func_zone_ct_weight(0xf4240 / $v0_123, arg5, $v0_6, $fp, 
                                    var_128, var_124, var_120);
                                $v0_64 = var_78_1;
                            }
                        }
                        else
                        {
                            char* $s4_3 = (char*)(&arg11[$s0_2 + 0x3fffffff]); // Fixed void pointer assignment
                            char* $s7_4 = (char*)(arg11 + (($s0_2 + 0x3fffffff) << 2) + 4); // Fixed void pointer assignment
                            int32_t $s0_3 = $s0_2 * 0x3c;
                                goto label_2a7a4;
                            var_124 = *(arg7 + $s0_3 + 0x38);
                            var_128 = *(arg7 + $s0_3 - 4);
                            *$s1_2 = var_ac_2($fp, *var_a8_2, *$s4_3, *$s7_4, var_128, var_124);
                            
                            if ($v0_5 == 1)
                            {
                                $a3_4 = *$s7_4;
                                $a1_22 = *var_a8_2;
                                $a2_15 = *$s4_3;
                                var_124_1 = *(arg12 + $s0_3 + 0x38);
                                var_128_1 = *(arg12 + $s0_3 - 4);
                            label_2a5c8:
                                $a0_51 = $fp;
                                $v0_86 = var_ac_2;
                            }
                            
                            $v0_64 = var_78_1_1;
                        }
                    }
                }
            }
            else
            {
                int32_t* $a1_19 = arg11;
                void* $v1_22;
                void* $a0_35;
                        goto label_2a41c;
                            goto label_2a7c8;
                
                if (i == $a0_28)
                {
                    $a0_35 = var_88_1;
                    
                    if ($(uintptr_t)v0_52 != 0xf)
                    
                    $v1_22 = var_88_1;
                label_2a3d8:
                    *((int32_t*)((char*)$a0_35 + 0x30c)) = *($v1_22 + 0x30c) + 1; // Fixed void pointer dereference
                label_2a530:
                    
                    if ($(uintptr_t)v0_52 == 0xf)
                    {
                        $v1_52 = *(arg7 + 0x380) << ($fp & 0x1f);
                        *$s1_2 = $v1_52;
                        
                        if ($v0_5 == 1)
                        {
                            $v0_123 = *(arg12 + 0x380);
                            var_120 = var_a0;
                            var_124 = var_a4;
                        }
                        
                        $v0_64 = var_78_1_2;
                    }
                    else
                    {
                        char* $s5_2 = (char*)(arg10 + ($v0_52 << 2) - 4); // Fixed void pointer assignment
                        char* $s2_8 = (char*)(&arg10[$v0_52]); // Fixed void pointer assignment
                            goto label_2a5c8;
                        var_124 = *(arg7 + ($v0_52 << 2) + 0x348);
                        var_128 = *(arg7 + ($v0_52 << 2) + 0x344);
                        *$s1_2 = var_ac_2($fp, *$s3_2, *$s5_2, *$s2_8, var_128, var_124);
                        
                        if ($v0_5 == 1)
                        {
                            $a3_4 = *$s2_8;
                            $a2_15 = *$s5_2;
                            var_124_1 = *(arg12 + ($v0_52 << 2) + 0x348);
                            var_128_1 = *(arg12 + ($v0_52 << 2) + 0x344);
                            $a1_22 = *$s3_2;
                        }
                        
                        $v0_64 = var_78_1_3;
                    }
                }
                else
                {
                    int32_t $s0_1 = 0;
                    
                    while (i < *$a1_19 << ($fp & 0x1f) || i >= $a1_19[1] << ($fp & 0x1f))
                    {
                        $a1_19 = &$a1_19[1];
                        
                        if ($(uintptr_t)s0_1 == 0xe)
                            break;
                        
                        $s0_1 += 1;
                    }
                    
                    $s0_2 = $s0_1 + 1;
                    
                    if ($(uintptr_t)v0_52 == 0xf)
                    {
                            goto label_2a3f4;
                        goto label_2a3d8;
                        $v1_22 = var_88_1;
                        
                        if ($s0_2 != $v0_52)
                        
                        $a0_35 = var_88_1;
                    }
                    
                    $a0_38 = $s0_2 - 1;
                    
                    if ($(uintptr_t)s0_2 != 0xf)
                        goto label_2a454;
                    
                    $a0_35 = var_88_1_1;
                label_2a41c:
                    char* $v1_29 = (char*)((($v0_52 + 0xb5) << 2) + $a0_35); // Fixed void pointer assignment
                    *$v1_29 += 1;
                    
                    if ($v0_52)
                        goto label_2a530;
                    
                    *$s1_2 = 0;
                    $v0_64 = var_78_1_4;
                }
            }
            $s1_2 += 4;
            var_78_1_5 = $v0_64 + 1;
            $s3_2 += 4;
            var_a8_2_2 += 4;
        }
        
        var_84_1_3 += 1;
        var_7c_1_1 += $v0_23;
    }
    
    int32_t $v0_47 = 1 << (($fp - 1) & 0x1f);
    int32_t $t1_2 = 0;
    
    while (true)
    {
        char* $a1_30 = (char*)(arg1); // Fixed void pointer assignment
        char* $a3_12 = (char*)(&rgbg_wght); // Fixed void pointer assignment
        int32_t $a2_23 = 0;
                uint32_t $v1_56 = ($v0_47 + *($a1_30 + i_1)) >> ($fp & 0x1f);
                int32_t $t4_1 = *var_8c;
                int32_t $v0_140 = $t4_1 - $v1_56;
                uint32_t $v1_61 = ($v0_47 + *($v0_29 + $a1_30 + i_1)) >> ($fp & 0x1f);
                int32_t $t5_2 = *(var_8c + 4);
                int32_t $v1_62 = $v1_61 - $t5_2;
                int32_t $lo_3;
                int32_t $hi_3;
                int32_t $v1_63 = 0;
        
        if ($t1_2 == arg4)
            break;
        
        
        while ($a2_23 != arg9)
        {
            for (int32_t i_1 = 0; $v0_23 != i_1; i_1 += 4)
            {
                
                if ($v1_56 >= $t4_1)
                    $v0_140 = $v1_56 - $t4_1;
                
                
                if ($v1_61 < $t5_2)
                    $v1_62 = $t5_2 - $v1_61;
                
                $hi_3 = HIGHD($v1_62 * $v1_62 + $v0_140 * $v0_140);
                $lo_3 = LOWD($v1_62 * $v1_62 + $v0_140 * $v0_140);
                
                if ($(uintptr_t)lo_3 < 0x332)
                {
                    $v1_63 = 1;
                    
                    if ($(uintptr_t)lo_3 < 0x2a7)
                    {
                        $v1_63 = 2;
                        
                        if ($(uintptr_t)lo_3 < 0x267)
                        {
                            $v1_63 = 3;
                            
                            if ($(uintptr_t)lo_3 < 0x23c)
                            {
                                $v1_63 = 4;
                                
                                if ($(uintptr_t)lo_3 < 0x21c)
                                {
                                    $v1_63 = 5;
                                    
                                    if ($(uintptr_t)lo_3 < 0x202)
                                        $v1_63 = *(arg16 + ($lo_3 << 2));
                                }
                            }
                        }
                    }
                }
                
                char* $t4_9 = (char*)($a3_12 + i_1); // Fixed void pointer assignment
                int32_t $v0_144 = $v1_63 << ($fp & 0x1f);
                
                if (*$t4_9 < $v0_144)
                    *$t4_9 = $v0_144;
            }
            
            $a2_23 += 1;
            $a3_12 += 0x3c;
            $a1_30 += $v0_23;
        }
        
        $t1_2 += 1;
        var_8c_7 += 8;
    }
    
    if ($v0_7 == 1)
    {
        char* $a1_31 = (char*)(&Cluster_rgbg_index_max); // Fixed void pointer assignment
        char* i_2 = (char*)(&Cluster_rgbg_index_max); // Fixed void pointer assignment
            int32_t $a2_24 = 0;
            int32_t $t6_2 = 0;
                int32_t j = 0;
                int32_t $v1_68 = 0;
                    int32_t $v0_145 = *(&Cluster_rgbg_index_num + $v1_68 + $t6_2);
        
        do
        {
            
            while (true)
            {
                
                do
                {
                    
                    if (!$v0_145)
                        j += 1;
                    else if (*(i_2 + 0x60) >= $v0_145)
                        j += 1;
                    else
                    {
                        *i_2 = j;
                        *((int32_t*)((char*)i_2 + 0x30)) = $a2_24; // Fixed void pointer dereference
                        *((int32_t*)((char*)i_2 + 0x60)) = $v0_145; // Fixed void pointer dereference
                        j += 1;
                    }
                    
                    $v1_68 = j << 2;
                } while ((uintptr_t)j != 0xe);
                
                $a2_24 += 1;
                
                if ($a2_24 == j)
                    break;
                
                $t6_2 = $a2_24 * 0x38;
            }
            
            int32_t $v0_146 = *i_2;
            int32_t $a3_13 = *(i_2 + 0x30) * $a2_24;
            i_2 += 4;
            *(&Cluster_rgbg_index_num + (($a3_13 + $v0_146) << 2)) = 0;
        } while (0xb75b4 != i_2);
        
        for (int32_t* i_3 = &Cluster_rgbg_value1; (uintptr_t)i_3 != 0xb73a4; )
        {
            int32_t $v0_151 = *$a1_31 << 2;
            char* $t2_3 = (char*)(&arg10[*$a1_31]); // Fixed void pointer assignment
            int32_t $a0_55 = *($a1_31 + 0x30) << 2;
            char* $a2_26 = (char*)(&arg11[*($a1_31 + 0x30)]); // Fixed void pointer assignment
            char* $a0_57 = (char*)(arg11 + $a0_55 + 4); // Fixed void pointer assignment
            char* $a3_18 = (char*)(arg10 + $v0_151 + 4); // Fixed void pointer assignment
            *i_3 = *$t2_3;
            i_3 = &i_3[5];
            i_3[0x37] = *$a2_26;
            $a1_31 += 4;
            *(i_3 - 0x10) = *$t2_3;
            i_3[0x38] = *$a0_57;
            *(i_3 - 0xc) = (*$t2_3 + 1 + *$a3_18) >> 1;
            i_3[0x39] = (*$a2_26 + 1 + *$a0_57) >> 1;
            *(i_3 - 8) = *$a3_18;
            i_3[0x3a] = *$a2_26;
            *(i_3 - 4) = *$a3_18;
            i_3[0x3b] = *$a0_57;
        }
        
        int32_t* $v0_165 = &Cluster_rgbg_value1;
        
        for (int32_t i_4 = 1; (uintptr_t)i_4 != 0x3d; )
        {
                int32_t* $v1_76 = $v0_165;
                int32_t j_1 = i_4;
            if (!*$v0_165)
                i_4 += 1;
            else if (!$v0_165[0x3c])
                i_4 += 1;
            else
            {
                
                while ((uintptr_t)j_1 != 0x3c)
                {
                    j_1 += 1;
                    
                    if (*$v0_165 == $v1_76[1] && $v0_165[0x3c] == $v1_76[0x3d])
                    {
                        $v1_76[1] = 0;
                        $v1_76[0x3d] = 0;
                    }
                    
                    $v1_76 = &$v1_76[1];
                }
                
                i_4 += 1;
            }
            
            $v0_165 = &$v0_165[1];
        }
        
        int32_t* $a0_58 = &Cluster_rgbg_value1;
        int32_t Cluster_rgbg_value1_1 = Cluster_rgbg_value1;
        
        while (true)
        {
                int32_t $t6_3 = 0;
                    int32_t $a1_32 = 0;
                    int32_t $v0_167 = 0;
                    int32_t $v1_77 = 0;
                        int32_t $t7_1 = 0;
                        char* $t4_12 = (char*)(i_5 * 0x3c + arg1); // Fixed void pointer assignment
                            int32_t $s0_11 = *$t4_12;
                            int32_t $t2_8 = *$a0_58;
                            uint32_t $a3_21 = ($v0_47 + $s0_11) >> ($fp & 0x1f);
                            int32_t $a2_31 = $a3_21 - $t2_8;
                            int32_t $t9_2 = *($t4_12 + 0x384);
                            int32_t $t8_2 = $a0_58[0x3c];
                            uint32_t $a3_24 = ($v0_47 + $t9_2) >> ($fp & 0x1f);
                            int32_t $a3_25 = $a3_24 - $t8_2;
                            int32_t $lo_5;
                            int32_t $hi_5;
            if (!Cluster_rgbg_value1_1)
                $a0_58 = &$a0_58[1];
            else if (!$a0_58[0x3c])
                $a0_58 = &$a0_58[1];
            else
            {
                
                while (true)
                {
                    $t6_3 += 1;
                    
                    for (int32_t i_5 = 0; i_5 != arg9; i_5 += 1)
                    {
                        
                        while ($t7_1 != arg8)
                        {
                            
                            if ($a3_21 < $t2_8)
                                $a2_31 = $t2_8 - $a3_21;
                            
                            
                            if ($a3_24 < $t8_2)
                                $a3_25 = $t8_2 - $a3_24;
                            
                            $hi_5 = HIGHD($a3_25 * $a3_25 + $a2_31 * $a2_31);
                            $lo_5 = LOWD($a3_25 * $a3_25 + $a2_31 * $a2_31);
                            $t7_1 += 1;
                            
                            if ($v0_8 >= $lo_5)
                            {
                                $v1_77 += $s0_11;
                                $v0_167 += $t9_2;
                                $a1_32 += 1;
                            }
                            
                            $t4_12 += 4;
                        }
                    }
                    
                    uint32_t $a2_34 = $a1_32 >> 1;
                    
                    if (!$a1_32)
                    {
                        $a0_58 = &$a0_58[1];
                        break;
                    }
                    
                    int32_t $t2_10 = *$a0_58;
                    uint32_t $v1_80 = (($v1_77 + $a2_34) / $a1_32) >> ($fp & 0x1f);
                    int32_t $a2_35 = $v1_80 - $t2_10;
                    
                    if ($t2_10 >= $v1_80)
                        $a2_35 = $t2_10 - $v1_80;
                    
                    uint32_t $v0_170 = (($v0_167 + $a2_34) / $a1_32) >> ($fp & 0x1f);
                    
                    if ($v0_11 >= $a2_35)
                    {
                        int32_t $t2_12 = $a0_58[0x3c];
                        int32_t $a2_37 = $v0_170 - $t2_12;
                        
                        if ($t2_12 >= $v0_170)
                            $a2_37 = $t2_12 - $v0_170;
                        
                        if ($v0_11 >= $a2_37)
                        {
                            $a0_58 = &$a0_58[1];
                            break;
                        }
                    }
                    
                    if ($v0_12 < $t6_3)
                    {
                        $a0_58 = &$a0_58[1];
                        break;
                    }
                    
                    *$a0_58 = $v1_80;
                    $a0_58[0x3c] = $v0_170;
                    $a0_58[0x78] = $a1_32;
                }
            }
            
            if (0xb73a4 == $a0_58)
                break;
            
            Cluster_rgbg_value1_1 = *$a0_58;
        }
        
        int32_t* $v0_171 = &Cluster_rgbg_value2;
        
        for (int32_t i_6 = 0; (uintptr_t)i_6 != 0x3c; )
        {
            int32_t* $v1_81 = &Cluster_rgbg_value1;
            int32_t $a1_33 = data_b7494;
                    int32_t $a2_43 = *$v1_81;
            
            while (true)
            {
                if (!$a1_33)
                    $v1_81 = &$v1_81[1];
                else if ($v0_171[0x78] >= $a1_33)
                    $v1_81 = &$v1_81[1];
                else
                {
                    $v0_171[0x78] = $a1_33;
                    *$v0_171 = $a2_43;
                    $v0_171[0x3c] = $v1_81[0x3c];
                    $v1_81 = &$v1_81[1];
                }
                
                if ($(uintptr_t)v1_81 == 0xb73a4)
                    break;
                
                $a1_33 = $v1_81[0x78];
            }
            
            int32_t* $t2_14 = &Cluster_rgbg_value1;
            
            if (!$v0_171[0x78])
                break;
            
            int32_t $v1_83 = 0;
            int32_t $a1_34 = 0;
            int32_t $a2_45 = 0;
            int32_t $t6_4 = data_b7494_1;
            
            while (true)
            {
                    int32_t $t4_13 = *$v0_171;
                    int32_t $t8_3 = *$t2_14;
                    int32_t $a3_31 = $t4_13 - $t8_3;
                    int32_t $t7_3 = $t2_14[0x3c];
                    int32_t $t5_4 = $v0_171[0x3c];
                    int32_t $t5_5 = $t5_4 - $t7_3;
                    int32_t $lo_9;
                    int32_t $hi_8;
                if (!$t6_4)
                    $t2_14 = &$t2_14[1];
                else
                {
                    
                    if ($t4_13 < $t8_3)
                        $a3_31 = $t8_3 - $t4_13;
                    
                    
                    if ($t5_4 < $t7_3)
                        $t5_5 = $t7_3 - $t5_4;
                    
                    $hi_8 = HIGHD($t5_5 * $t5_5 + $a3_31 * $a3_31);
                    $lo_9 = LOWD($t5_5 * $t5_5 + $a3_31 * $a3_31);
                    
                    if ($v0_9 < $lo_9)
                        $t2_14 = &$t2_14[1];
                    else
                    {
                        $v1_83 += $t6_4;
                        $a2_45 += $t6_4 * $t8_3;
                        $t2_14[0x78] = 0;
                        $a1_34 += $t6_4 * $t7_3;
                        $t2_14 = &$t2_14[1];
                    }
                }
                
                if ($(uintptr_t)t2_14 == 0xb73a4)
                    break;
                
                $t6_4 = $t2_14[0x78];
            }
            
            i_6 += 1;
            
            if ($v1_83)
            {
                uint32_t $a3_36 = $v1_83 >> 1;
                $v0_171[0x78] = 0;
                *$v0_171 = ($a2_45 + $a3_36) / $v1_83;
                $v0_171[0x3c] = ($a1_34 + $a3_36) / $v1_83;
            }
            
            $v0_171 = &$v0_171[1];
        }
        
        int32_t* $v0_172 = &Cluster_rgbg_value2;
        int32_t Cluster_rgbg_value2_1 = Cluster_rgbg_value2;
        
        while (true)
        {
                int32_t $t5_6 = $v0_172[0x3c];
                    int32_t $t3_2 = 0;
                        int32_t $t7_4 = $t3_2 * 0x3c;
                        int32_t $t1_6 = 0;
                        char* $t2_15 = (char*)(arg1 + $t7_4); // Fixed void pointer assignment
                            int32_t $t8_4 = *(&rgbg_wght + ($t1_6 << 2) + $t7_4);
                                uint32_t $v1_92 = ($v0_47 + *$t2_15) >> ($fp & 0x1f);
                                int32_t $a1_38 = $v1_92 - Cluster_rgbg_value2_1;
                                uint32_t $v1_95 = ($v0_47 + *($t2_15 + 0x384)) >> ($fp & 0x1f);
                                int32_t $v1_96 = $v1_95 - $t5_6;
                                int32_t $lo_13;
                                int32_t $hi_11;
            if (Cluster_rgbg_value2_1)
            {
                
                if (!$t5_6)
                    $v0_172 = &$v0_172[1];
                else
                {
                    $v0_172[0x78] = 0;
                    $v0_172[0xb4] = 0;
                    
                    while (true)
                    {
                        
                        if ($t3_2 == arg9)
                            break;
                        
                        
                        while ($t1_6 != arg8)
                        {
                            $t1_6 += 1;
                            
                            if ($t8_4)
                            {
                                
                                if ($v1_92 < Cluster_rgbg_value2_1)
                                    $a1_38 = Cluster_rgbg_value2_1 - $v1_92;
                                
                                
                                if ($v1_95 < $t5_6)
                                    $v1_96 = $t5_6 - $v1_95;
                                
                                $hi_11 = HIGHD($v1_96 * $v1_96 + $a1_38 * $a1_38);
                                $lo_13 = LOWD($v1_96 * $v1_96 + $a1_38 * $a1_38);
                                
                                if ($v0_10 >= $lo_13)
                                {
                                    $v0_172[0x78] += 1;
                                    $v0_172[0xb4] += $t8_4;
                                }
                            }
                            
                            $t2_15 += 4;
                        }
                        
                        $t3_2 += 1;
                    }
                    
                    $v0_172 = &$v0_172[1];
                }
            }
            else
                $v0_172 = &$v0_172[1];
            
            if ($(uintptr_t)v0_172 == 0xb6fe4)
                break;
            
            Cluster_rgbg_value2_1 = *$v0_172;
        }
        
        int32_t* $v0_173 = &Cluster_rgbg_value2;
        int32_t $a2_51 = data_b70d4_1;
        
        while (true)
        {
            if (!$a2_51)
                $v0_173 = &$v0_173[1];
            else
            {
                $v0_173[0xb4] = (($a2_51 >> 1) + $v0_173[0xb4]) / $a2_51;
                $v0_173 = &$v0_173[1];
            }
            
            if ($(uintptr_t)v0_173 == 0xb6fe4)
                break;
            
            $a2_51 = $v0_173[0x78];
        }
        
        int32_t* $v0_174 = &Cluster_rgbg_value2;
        int32_t $a1_42 = 0;
        int32_t $v1_104 = data_b71c4_1;
        
        while (true)
        {
            $v0_174 = &$v0_174[1];
            
            if ($v1_104 && $a1_42 < $v1_104)
                $a1_42 = $v1_104;
            
            if ($(uintptr_t)v0_174 == 0xb6fe4)
                break;
            
            $v1_104 = $v0_174[0xb4];
        }
        
        int32_t* $v1_105 = &Cluster_rgbg_value2;
        int32_t $v0_175 = data_b70d4_2;
        
        while (true)
        {
            if (!$v0_175)
                $v1_105 = &$v1_105[1];
            else
            {
                $v1_105[0x78] = ($v0_175 * $v1_105[0xb4] + ($a1_42 >> 1)) / $a1_42;
                $v1_105 = &$v1_105[1];
            }
            
            if ($(uintptr_t)v1_105 == 0xb6fe4)
                break;
            
            $v0_175 = $v1_105[0x78];
        }
        
        int32_t* $v0_178 = &Cluster_rgbg_value2;
        int32_t $a2_54 = 0;
        int32_t $v1_106 = data_b70d4_3;
        
        while (true)
        {
            $v0_178 = &$v0_178[1];
            
            if ($v1_106 && $a2_54 < $v1_106)
                $a2_54 = $v1_106;
            
            if (0xb6fe4 == $v0_178)
                break;
            
            $v1_106 = $v0_178[0x78];
        }
        
        uint32_t $s0_14 = $a2_54 >> 1;
        int32_t $t5_7 = 0;
        
        while (true)
        {
            int32_t $a3_37 = $t5_7 * 0x3c;
            char* $t1_8 = (char*)(&rgbg_wght + $a3_37); // Fixed void pointer assignment
            int32_t $t7_5 = 0;
            char* $a3_38 = (char*)(arg1 + $a3_37); // Fixed void pointer assignment
                int32_t* $t4_15 = &Cluster_rgbg_value2;
                int32_t i_7 = 1;
                int32_t i_12 = 0;
                    int32_t $a1_44 = *$t4_15;
                        int32_t $t6_5 = $t4_15[0x3c];
                            uint32_t $v0_184 = ($v0_47 + *$a3_38) >> ($fp & 0x1f);
                            int32_t $v0_185 = $v0_184 - $a1_44;
                            uint32_t $a1_47 = ($v0_47 + *($a3_38 + 0x384)) >> ($fp & 0x1f);
                            int32_t $a1_48 = $a1_47 - $t6_5;
                            int32_t $lo_17;
                            int32_t $hi_15;
            
            if ($t5_7 == arg9)
                break;
            
            
            while (true)
            {
                
                if ($t7_5 == arg8)
                    break;
                
                
                do
                {
                    
                    if (!$a1_44)
                        i_7 += 1;
                    else
                    {
                        
                        if (!$t6_5)
                            i_7 += 1;
                        else
                        {
                            
                            if ($v0_184 < $a1_44)
                                $v0_185 = $a1_44 - $v0_184;
                            
                            
                            if ($a1_47 < $t6_5)
                                $a1_48 = $t6_5 - $a1_47;
                            
                            $hi_15 = HIGHD($a1_48 * $a1_48 + $v0_185 * $v0_185);
                            $lo_17 = LOWD($a1_48 * $a1_48 + $v0_185 * $v0_185);
                            
                            if ($v0_10 < $lo_17)
                                i_7 += 1;
                            else
                            {
                                if (!i_12)
                                    i_12 = i_7;
                                else if ((&Cluster_rgbg_value2)[i_12 + 0x77] < $t4_15[0x78])
                                    i_12 = i_7;
                                
                                i_7 += 1;
                            }
                        }
                    }
                    
                    $t4_15 = &$t4_15[1];
                } while ((uintptr_t)i_7 != 0x3d);
                
                int32_t $v0_195;
                
                if (!i_12)
                    $v0_195 = $s0_14 + *$t1_8;
                else
                    $v0_195 = (&Cluster_rgbg_value2)[i_12 + 0x77] * *$t1_8 + $s0_14;
                
                $t7_5 += 1;
                $t1_8 += 4;
                $a3_38 += 4;
                *($t1_8 - 4) = $v0_195 / $a2_54;
            }
            
            $t5_7 += 1;
        }
    }
    
    int32_t $a1_52 = 0;
    
    for (int32_t i_8 = 0; i_8 != arg9; i_8 += 1)
    {
        int32_t $v1_111 = 0;
            int32_t $v0_202 = *(&rgbg_wght + ($v1_111 << 2) + i_8 * 0x3c);
        
        while ($v1_111 != arg8)
        {
            $v1_111 += 1;
            $a1_52 += $v0_202;
        }
    }
    
    int32_t* $v1_112 = arg20;
    
    if (!$a1_52)
        goto label_2b2d4;
    
    *arg20 = 0;
    int32_t $v0_206 = -(arg8) << 2;
    char* $v0_208 = (char*)($v0_29 - arg1); // Fixed void pointer assignment
    char* $t1_9 = (char*)(arg1); // Fixed void pointer assignment
    char* $t4_16 = (char*)(nullptr); // Fixed void pointer assignment
    int32_t $s2_14 = 0;
    int32_t $s3_5 = 0;
    int32_t $s4_7 = 0;
    int32_t $t2_18 = 0;
    int32_t $t0_6 = 0;
    int32_t $s5_5 = 0;
    int32_t $v1_114 = 0;
    
    while ($v1_114 != arg9)
    {
        char* $s1_4 = (char*)($t1_9); // Fixed void pointer assignment
        int32_t $s7_6 = 0;
            int32_t $v0_214 = $v1_114 * 0x3c + ($s7_6 << 2);
            int32_t $v0_217 = (&data_20000 + 0x1018)($fp, *(&rgbg_wght + $v0_214), 
            int32_t $v0_219 = $t0_6 + fix_point_mult2_32($fp, *$s1_4, $v0_217);
            int32_t $v0_222;
            int32_t $v0_223 = $s4_7 + $v0_222;
            int32_t $s0_16 = $s2_14 + $v0_217;
            int32_t $a0_65 = $v0_223 < $s4_7 ? 1 : 0;
            int32_t $v0_224 = $s0_16 < $s2_14 ? 1 : 0;
        
        while ($s7_6 != arg8)
        {
                *(&zone_pix_wgh + $v0_214), *(arg13 + $v0_214) << ($fp & 0x1f), var_128, var_124, 
                var_120);
            int32_t (* var_a4_1)(int32_t arg1, int32_t arg2, int32_t arg3) = fix_point_mult2_32;
            $s5_5 += $v0_219 < $t0_6 ? 1 : 0;
            $v0_222 = var_a4_1($fp, *($t4_16 + $s1_4 + $v0_208 + $t1_9), $v0_217);
            $s4_7 = $v0_223;
            $t2_18 += $a0_65;
            $s2_14 = $s0_16;
            $s3_5 += $v0_224;
            $s7_6 += 1;
            $s1_4 += 4;
            $t0_6 = $v0_219;
        }
        
        $v1_114 += 1;
        $t1_9 += $v0_23;
        $t4_16 += $v0_206;
    }
    
    int32_t* result;
    
    if ($s2_14 | $s3_5)
    {
        int32_t i_19;
        int32_t $a1_56;
        int32_t i_18 = i_19;
        int32_t i_17 = fix_point_div_64($fp, $a1_56, $s4_7, $t2_18, $s2_14, $s3_5);
        int32_t $v0_304;
            char* var_a4_2 = (char*)(&rgbg_dis); // Fixed void pointer assignment
            char* $v1_121 = (char*)(-($v0_28) << 2); // Fixed void pointer assignment
            int32_t $t2_21 = 0;
            int32_t $s7_7 = 0;
            int32_t $t4_18 = 0;
            char* var_a0_2 = (char*)(&rgbg_wght); // Fixed void pointer assignment
                int32_t $v0_231 = $t4_18 * 0x3c;
                int32_t $s4_8 = 0;
                char* $s3_6 = (char*)(arg13 + $v0_231); // Fixed void pointer assignment
                char* $s5_6 = (char*)(arg1 + $v0_29 + $t2_21); // Fixed void pointer assignment
                    int32_t $v0_238 = *($v1_121 + $s5_6 + $t2_21);
                    int32_t $a2_60 = i_18 - $v0_238;
                    int32_t $v0_240 = *$s5_6;
                    int32_t $a3_45 = i_17 - $v0_240;
                    int32_t $a1_59 =
                    int32_t $a3_47 = ($s4_8 << 2) + $v0_231;
                    int32_t $a2_62 = *(var_a0_2 + $a3_47);
        i_19 = fix_point_div_64($fp, $a1_52, $t0_6, $s5_5, $s2_14, $s3_5);
        int32_t (* var_78_3)(int32_t arg1, int32_t arg2, int32_t arg3, int32_t arg4, int32_t arg5, 
            int32_t arg6) = fix_point_div_64;
        
        if ($v1_7 != 1)
            $v0_304 = arg10[0xe];
        else
        {
            void* void* var_a8_5 = (void*)&data_20000; // Fixed function pointer assignment
            
            while ($t4_18 != arg9)
            {
                
                while ($s4_8 != arg8)
                {
                    
                    if ($v0_238 >= i_18)
                        $a2_60 = $v0_238 - i_18;
                    
                    
                    if ($v0_240 >= i_17)
                        $a3_45 = $v0_240 - i_17;
                    
                        (var_a8_5 + 0xfd4)($fp, $a2_60) + (var_a8_5 + 0xfd4)($fp, $a3_45, $a3_45);
                    *((int32_t*)((char*)var_a4_2 + $a3_47)) = $a1_59; // Fixed void pointer dereference
                    
                    if (!$a2_62)
                        $s4_8 += 1;
                    else if (!*(&zone_pix_wgh + $a3_47))
                        $s4_8 += 1;
                    else if (!*$s3_6)
                        $s4_8 += 1;
                    else
                    {
                        $s7_7 += (var_a8_5 + 0xfd4)($fp, fix_point_mult3_32($fp, $a1_59, $a2_62), 
                            *$s3_6 << ($fp & 0x1f));
                        $s4_8 += 1;
                    }
                    
                    $s3_6 += 4;
                    $s5_6 += 4;
                }
                
                $t4_18 += 1;
                $t2_21 += $v0_23;
                $v1_121 += $v0_206;
            }
            
            int32_t $v0_258;
            int32_t $a1_62;
            $v0_258 = fix_point_div_32($fp, $s7_7, $s2_14);
            int32_t (* var_6c_3_1)(int32_t arg1, int32_t arg2, int32_t arg3) = fix_point_div_32;
            
            if (!$v0_258)
                $v0_304 = arg10[0xe];
            else
            {
                char* var_50_1 = (char*)(&rgbg_d_wght); // Fixed void pointer assignment
                char* var_4c_1 = (char*)(&rgbg_wght); // Fixed void pointer assignment
                char* var_a0_3 = (char*)(nullptr); // Fixed void pointer assignment
                int32_t $s2_15 = 0;
                int32_t $s3_7 = 0;
                int32_t var_a4_3 = 0;
                int32_t var_8c_4 = 0;
                int32_t var_a8_6 = 0;
                int32_t var_90_3 = 0;
                int32_t var_b0_4 = 0;
                char* var_48_1 = (char*)(&data_b0000); // Fixed void pointer assignment
                    int32_t $v0_261 = var_b0_4 * 0x3c;
                    int32_t var_ac_5 = 0;
                    char* $s5_7 = (char*)(var_50_1 + $v0_261); // Fixed void pointer assignment
                    char* $s4_9 = (char*)(var_4c_1 + $v0_261); // Fixed void pointer assignment
                    char* $s7_8 = (char*)(var_80_1); // Fixed void pointer assignment
                    char* var_54_1 = (char*)(&zone_pix_wgh); // Fixed void pointer assignment
                        int32_t $v1_128 = (var_ac_5 << 2) + $v0_261;
                        int32_t $v0_273 = var_6c_3($fp, *(var_48_1 + 0x6158 + $v1_128), $v0_258);
                        int32_t $v0_280 = fix_point_mult3_32($fp, *$s5_7, *(var_54_1 + $v1_128));
                        int32_t $v0_283 = var_a8_6 + (var_58_3 + 0xfd4)($fp, *$s7_8, $v0_280);
                        int32_t $a0_88 = $v0_283 < var_a8_6 ? 1 : 0;
                        int32_t $v0_290;
                        int32_t $v0_291 = var_a4_3 + $v0_290;
                        int32_t $a0_90 = $v0_291 < var_a4_3 ? 1 : 0;
                        int32_t $v1_141 = $s2_15 + $v0_280;
                
                while (var_b0_4 != arg9)
                {
                    void* void* var_58_3 = (void*)&data_20000; // Fixed function pointer assignment
                    
                    while (var_ac_5 != arg8)
                    {
                        
                        if ($v1_8 < $v0_273)
                            *$s5_7 = 0;
                        else if ($v0_273 >= $v0_1)
                            *$s5_7 = (var_58_3 + 0xfd4)($fp, *$s4_9, 
                                (1 << ($fp & 0x1f))
                                    - var_6c_3($fp, $v0_273 - $v0_1, $v1_8 - $v0_1));
                        else
                            *$s5_7 = *$s4_9;
                        
                        $s5_7 += 4;
                        var_a8_6 = $v0_283;
                        var_90_3 += $a0_88;
                        $v0_290 = (var_58_3 + 0xfd4)($fp, *(var_a0_3 + $s7_8 + $v0_208 + var_80_1), 
                            $v0_280);
                        $s7_8 += 4;
                        var_a4_3 = $v0_291;
                        var_8c_4 += $a0_90;
                        $s3_7 += $v1_141 < $s2_15 ? 1 : 0;
                        $s2_15 = $v1_141;
                        var_ac_5 += 1;
                        $s4_9 += 4;
                    }
                    
                    var_b0_4_1 += 1;
                    var_80_1_2 += $v0_23;
                    var_a0_3 += $v0_206;
                }
                
                if (!($s2_15 | $s3_7))
                    $v0_304 = arg10[0xe];
                else
                {
                    int32_t i_20;
                    int32_t $a1_69;
                    i_20 = var_78_3($fp, $a1_62, var_a8_6, var_90_3, $s2_15, $s3_7);
                    i_18 = i_20;
                    i_17 = var_78_3($fp, $a1_69, var_a4_3, var_8c_4, $s2_15, $s3_7);
                    $v0_304 = arg10[0xe];
                }
            }
        }
        
        int32_t i_11 = $v0_304 << ($fp & 0x1f);
        int32_t i_10 = i_11;
        
        if (i_11 >= i_18)
        {
            int32_t i_13 = *arg10 << ($fp & 0x1f);
            
            if (i_18 >= i_13)
                i_13 = i_18;
            
            i_10 = i_13;
        }
        
        int32_t i_15 = arg11[0xe] << ($fp & 0x1f);
        int32_t i_9 = i_15;
        
        if (i_15 >= i_17)
        {
            int32_t i_14 = *arg11 << ($fp & 0x1f);
            
            if (i_17 >= i_14)
                i_14 = i_17;
            
            i_9 = i_14;
        }
        
        int32_t* $v1_148 = arg10;
        int32_t var_128_10;
        int32_t var_124_10;
        int32_t (* $v0_324)(int32_t arg1, int32_t arg2, int32_t arg3, int32_t arg4, int32_t arg5, 
            int32_t arg6);
        int32_t $a0_101;
        int32_t i_16;
        int32_t $a2_75;
        int32_t $a3_53;
        int32_t $v0_316;
        int32_t $a0_95;
        int32_t $s0_17;
        
        if (i_10 == i_11)
        {
            int32_t $v0_315;
                int32_t* $a0_94 = arg11;
                int32_t $s3_8 = 0;
            $s0_17 = 0xf;
            
            if (i_9 != i_15)
            {
            label_2b894:
                
                while (i_9 < *$a0_94 << ($fp & 0x1f) || i_9 >= $a0_94[1] << ($fp & 0x1f))
                {
                    $a0_94 = &$a0_94[1];
                    
                    if ($(uintptr_t)s3_8 == 0xe)
                        break;
                    
                    $s3_8 += 1;
                }
                
                $a0_95 = $s3_8 + 1;
                
                if ($(uintptr_t)s0_17 == 0xf)
                {
                    goto label_2b908;
                    $v0_315 = $a0_95 << 2;
                }
                
                if ($(uintptr_t)s3_8 == 0xe)
                {
                    goto label_2b94c;
                    $v0_316 = $s0_17 << 2;
                }
                
                char* $s0_20 = (char*)(arg12 + ((($s3_8 + 1) * 0xf + $s0_17 + 0x3ffffff0) << 2)); // Fixed void pointer assignment
                char* $s5_8 = (char*)(&arg10[$s0_17 + 0x3fffffff]); // Fixed void pointer assignment
                char* $s4_13 = (char*)(arg10 + (($s0_17 + 0x3fffffff) << 2) + 4); // Fixed void pointer assignment
                int32_t $v0_330 =
                    ISPAWBInterpolation1($fp, i_10, *$s5_8, *$s4_13, *$s0_20, *($s0_20 + 4));
                int32_t $v0_333 = ISPAWBInterpolation1($fp, i_10, *$s5_8, *$s4_13, 
                    *($s0_20 + 0x3c), *($s0_20 + 0x40));
                char* $v1_161 = (char*)(&arg11[$s3_8]); // Fixed void pointer assignment
                $a3_53 = *($v1_161 + 4);
                $a2_75 = *$v1_161;
                var_124_10_1 = $v0_333;
                var_128_10_1 = $v0_330;
                i_16 = i_9;
                $a0_101 = $fp;
                $v0_324 = ISPAWBInterpolation2;
            }
            else
            {
                char* $v0_318 = (char*)(arg11 + $v0_315 - 4); // Fixed void pointer assignment
                char* $a0_96 = (char*)($a0_95 * 0x3c + arg12); // Fixed void pointer assignment
                $a0_95 = 0xf;
            label_2b8fc:
                $v0_315 = 0x3c;
            label_2b908:
                $a3_53 = *($v0_318 + 4);
                $a2_75 = *$v0_318;
                i_16 = i_9;
                var_124_10 = *($a0_96 + 0x38);
                var_128_10 = *($a0_96 - 4);
                $a0_101 = $fp;
                $v0_324 = ISPAWBInterpolation1;
            }
        }
        else
        {
            int32_t $a2_74 = 0;
            
            while (i_10 < *$v1_148 << ($fp & 0x1f) || i_10 >= $v1_148[1] << ($fp & 0x1f))
            {
                $v1_148 = &$v1_148[1];
                
                if ($(uintptr_t)a2_74 == 0xe)
                    break;
                
                $a2_74 += 1;
            }
            
            $s0_17 = $a2_74 + 1;
            
            if (i_9 != i_15)
                goto label_2b894;
            
            if ($(uintptr_t)a2_74 == 0xe)
            {
                goto label_2b8fc;
                $a0_95 = 0xf;
            }
            
            $v0_316 = $s0_17 << 2;
        label_2b94c:
            char* $v1_157 = (char*)(arg10 + $v0_316 - 4); // Fixed void pointer assignment
            $a3_53 = *($v1_157 + 4);
            $a2_75 = *$v1_157;
            i_16 = i_10;
            char* $v0_322 = (char*)(arg12 + $v0_316 - 4); // Fixed void pointer assignment
            var_124_10_2 = *($v0_322 + 0x34c);
            var_128_10_2 = *($v0_322 + 0x348);
            $a0_101 = $fp;
            $v0_324 = ISPAWBInterpolation1;
        }
        int32_t $v0_334 = $v0_324($a0_101, i_16, $a2_75, $a3_53, var_128_10_3, var_124_10_3);
        uint32_t $v0_335;
        
        if ($v0_334)
            $v0_335 =
                ($v0_47 + fix_point_div_32($fp, 0xf4240 << ($fp & 0x1f), $v0_334)) >> ($fp & 0x1f);
        else
            $v0_335 = 0x1388;
        
        *arg15 = $v0_335;
        *arg19 = (i_10 + $v0_47) >> ($fp & 0x1f);
        arg19[1] = (i_9 + $v0_47) >> ($fp & 0x1f);
        result = arg20;
        *result = 0;
    }
    else
    {
    label_2b2cc:
        $v1_112 = arg20;
    label_2b2d4:
        *$v1_112 = 1;
        *arg15 = 0x1388;
        result = 0x100;
        *arg19 = 0x100;
        arg19[1] = 0x100;
    }
    
    return result;
}

