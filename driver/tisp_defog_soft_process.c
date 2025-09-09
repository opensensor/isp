#include "include/main.h"


  int32_t tisp_defog_soft_process(void* arg1, void* arg2, void* arg3, void* arg4, void* arg5, int32_t* arg6, int32_t* arg7, int32_t* arg8, int32_t* arg9, void* arg10, int32_t* arg11, void* arg12, void* arg13, void* arg14, int32_t* arg15, int32_t* arg16, int32_t* arg17, int32_t* arg18, int32_t* arg19, int32_t* arg20, int32_t* arg21, int32_t* arg22, int32_t* arg23, int32_t* arg24)

{
    void* arg_0 = arg1;
    void* arg_4 = arg2;
    void* arg_8 = arg3;
    void* arg_c = arg4;
    int32_t $a0 = *(arg10 + 0xc);
    int32_t $a0_1 = *(arg10 + 0x10);
    int32_t $a0_2 = *(arg10 + 0x14);
    int32_t $t0 = *(arg10 + 0x18);
    int32_t $t1 = *(arg10 + 0x1c);
    int32_t $a0_3 = *arg24;
    int32_t $a0_4 = arg24[1];
    int32_t $a0_5 = arg24[2];
    int32_t $a0_6 = arg24[3];
    int32_t $v1_10 = arg24[4];
    int32_t $v1_11 = 0x100 - *(arg10 + 8);
    int32_t $v0_7 = 1;
    
    if ($a0_5 < $a0_6)
        $v0_7 = $a0_6 - $a0_5;
    
    int32_t $a3_1 = *(arg10 + 4) + 1;
    void var_f0_2;
    memcpy(&var_f0_3, U"8H8H]H8H8", 0x24, $a3_1);
    void* $v0_8 = kmem_cache_alloc(0, 0x20);
    void* $v0_9 = kmem_cache_alloc(0, 0x20);
    void* $v0_10 = kmem_cache_alloc(0, 0x20);
    memset($v0_10, 0, 0x2d0);
    void* $v0_11 = kmem_cache_alloc(0, 0x20);
    memset($v0_11, 0, 0x2d0);
    int32_t* i_4 = kmem_cache_alloc(0, 0x20);
    memset(i_4, 0, 0x3c);
    int32_t* $v0_12 = kmem_cache_alloc(0, 0x20);
    memset($v0_12, 0, 0x3c);
    char* $v0_13 = kmem_cache_alloc(0, 0x20);
    memset($v0_13, 0, 0xb4);
    int32_t $t0_1 = $t0;
    int32_t $a0_14 = 0xff;
    
    if ($t1 < 0x100)
        $a0_14 = $t1;
    
    int32_t $a3_2 = $a3_1;
    
    if ($a0_14 < $t0_1)
        $t0_1 = $a0_14;
    
    int32_t $v0_15 = 2;
    
    if ($a3_2 < 2)
        $a3_2 = 2;
    
    if ($t0_1 < 0)
        $t0_1 = 0;
    
    if ($v1_11 >= 2)
        $v0_15 = $v1_11;
    
    int32_t $a0_15 = $a0_14 - $t0_1;
    int32_t $t0_2 = 0;
    int32_t var_b8_1_1 = 0;
    int32_t var_bc_1_1 = 0;
    int32_t var_c0_1_1 = 0;
    int32_t var_c4_1_1 = 0;
    int32_t var_90_24 = 0;
    
    for (int32_t i = 0; i != 0xa; )
    {
        int32_t j = 0;
        int32_t* $t6_1 = arg5 + $t0_2;
        int32_t* $t5_1 = arg12 + $t0_2;
        int32_t* $t4_1 = arg13 + $t0_2;
        int32_t* $t3_1 = arg14 + $t0_2;
        
        do
        {
            int32_t $v0_24 = j < arg9[1] ? 1 : 0;
            int32_t $v0_26;
            
            if (i >= *arg9)
            {
                if (!$v0_24)
                    $v0_26 = arg8[3];
                else
                    $v0_26 = arg8[2];
            }
            else if (!$v0_24)
                $v0_26 = arg8[1];
            else
                $v0_26 = *arg8;
            
            int32_t $s7_1 = j << 2;
            int32_t $v1_16 = $t0_2 + $s7_1;
            int32_t $lo_1;
            int32_t $hi_1;
            $hi_1 = HIGHD($v0_26 * *(arg2 + $v1_16));
            $lo_1 = LOWD($v0_26 * *(arg2 + $v1_16));
            int32_t $t9_1 = $hi_1 >> 4;
            int32_t $lo_2;
            int32_t $hi_2;
            $hi_2 = HIGHD($v0_26 * *(arg3 + $v1_16));
            $lo_2 = LOWD($v0_26 * *(arg3 + $v1_16));
            int32_t $ra_1 = $hi_2 >> 4;
            int32_t $lo_3;
            int32_t $hi_3;
            $hi_3 = HIGHD($v0_26 * *(arg4 + $v1_16));
            $lo_3 = LOWD($v0_26 * *(arg4 + $v1_16));
            int32_t $lo_5;
            int32_t $hi_5;
            $hi_5 = HIGHD($ra_1 * 0x26 + $t9_1 * 0x13);
            $lo_5 = LOWD($ra_1 * 0x26 + $t9_1 * 0x13);
            int32_t $a2_8 = $hi_3 >> 4;
            int32_t $v1_20 = ($lo_5 + $a2_8 * 7) >> 6;
            
            if ($v1_20 < 0)
                $v1_20 = 0;
            
            if ($v1_20 >= 0x100)
                $v1_20 = 0xff;
            
            int32_t $a3_14;
            
            if ($v1_10 != 1)
            {
                int32_t $a3_15 = $a0 + $t9_1 - 0x40;
                
                if ($a3_15 < 0)
                    $a3_15 = 0;
                
                int32_t $s6_3 = $a3_15;
                
                if ($a3_15 >= 0x100)
                    $s6_3 = 0xff;
                
                *$t5_1 = $t0_1 + (($s6_3 * $a0_15) >> 8);
                int32_t $a3_21 = $a0_1 + $ra_1 - 0x40;
                
                if ($a3_21 < 0)
                    $a3_21 = 0;
                
                int32_t $s6_6 = $a3_21;
                
                if ($a3_21 >= 0x100)
                    $s6_6 = 0xff;
                
                *$t4_1 = $t0_1 + (($s6_6 * $a0_15) >> 8);
                int32_t $a3_27 = $a0_2 + $a2_8 - 0x40;
                
                if ($a3_27 < 0)
                    $a3_27 = 0;
                
                int32_t $s6_9 = $a3_27;
                
                if ($a3_27 >= 0x100)
                    $s6_9 = 0xff;
                
                $a3_14 = $t0_1 + (($s6_9 * $a0_15) >> 8);
            }
            else
            {
                $a3_14 = (($a0_15 * $v1_20) >> 8) + $t0_1;
                *$t5_1 = $a3_14;
                *$t4_1 = $a3_14;
            }
            
            *$t3_1 = $a3_14;
            *($v0_8 + j + i * 0x12) = $v1_20;
            *($v0_9 + (j << 1) + i * 0x24) = $v1_20 - *$t6_1;
            *$t6_1 = $v1_20;
            
            if (*arg23)
                j += 1;
            else
            {
                int32_t $s6_15 = *arg21;
                int32_t $t9_2;
                
                if ($t9_1 < $s6_15)
                    $t9_2 = arg21[5];
                else if ($t9_1 < arg21[1])
                    $t9_2 = arg21[6];
                else if ($t9_1 < arg21[2])
                    $t9_2 = arg21[7];
                else if ($t9_1 < arg21[3])
                    $t9_2 = arg21[8];
                else if ($t9_1 >= arg21[4])
                    $t9_2 = arg21[0xa];
                else
                    $t9_2 = arg21[9];
                
                int32_t $a3_48;
                
                if ($ra_1 < $s6_15)
                    $a3_48 = arg21[5];
                else if ($ra_1 < arg21[1])
                    $a3_48 = arg21[6];
                else if ($ra_1 < arg21[2])
                    $a3_48 = arg21[7];
                else if ($ra_1 < arg21[3])
                    $a3_48 = arg21[8];
                else if ($ra_1 >= arg21[4])
                    $a3_48 = arg21[0xa];
                else
                    $a3_48 = arg21[9];
                
                int32_t $a2_9;
                
                if ($a2_8 < $s6_15)
                    $a2_9 = arg21[5];
                else if ($a2_8 < arg21[1])
                    $a2_9 = arg21[6];
                else if ($a2_8 < arg21[2])
                    $a2_9 = arg21[7];
                else if ($a2_8 < arg21[3])
                    $a2_9 = arg21[8];
                else if ($a2_8 >= arg21[4])
                    $a2_9 = arg21[0xa];
                else
                    $a2_9 = arg21[9];
                
                int32_t $v1_21;
                
                if ($v1_20 < $s6_15)
                    $v1_21 = arg21[5];
                else if ($v1_20 < arg21[1])
                    $v1_21 = arg21[6];
                else if ($v1_20 < arg21[2])
                    $v1_21 = arg21[7];
                else if ($v1_20 < arg21[3])
                    $v1_21 = arg21[8];
                else if ($v1_20 >= arg21[4])
                    $v1_21 = arg21[0xa];
                else
                    $v1_21 = arg21[9];
                
                int32_t $s7_2 = $t0_2 + $s7_1;
                var_c4_1_2 += $t9_2;
                var_c0_1_2 += $a3_48;
                var_bc_1_2 += $a2_9;
                var_b8_1_2 += $v1_21;
                int32_t* $t9_7 = arg1 + (var_90_25 << 2);
                int32_t $lo_6;
                int32_t $hi_6;
                $hi_6 = HIGHD($v0_26 * *$t9_7);
                $lo_6 = LOWD($v0_26 * *$t9_7);
                *($v0_10 + $s7_2) = (($hi_6 << 0x10 | $lo_6 >> 0x10) + ($lo_6 >> 0xf & 1)) / $a3_2;
                int32_t $lo_8;
                int32_t $hi_7;
                $hi_7 = HIGHD($v0_26 * $t9_7[2]);
                $lo_8 = LOWD($v0_26 * $t9_7[2]);
                *($v0_11 + $s7_2) = (($hi_7 << 0x10 | $lo_8 >> 0x10) + ($lo_8 >> 0xf & 1)) / $v0_15;
                var_90_26 += 3;
                j += 1;
            }
            
            $t6_1 = &$t6_1[1];
            $t5_1 = &$t5_1[1];
            $t4_1 = &$t4_1[1];
            $t3_1 = &$t3_1[1];
        } while (j != 0x12);
        
        i += 1;
        $t0_2 += 0x48;
    }
    
    int32_t $v1_34;
    
    if (*arg23)
        $v1_34 = arg23[1];
    else
    {
        int32_t i_1 = 0;
        void* $v0_36 = $v0_10;
        
        do
        {
            int32_t j_1 = 0;
            int32_t* $v1_35 = $v0_36 + i_1;
            void* $v0_38 = $v0_11 + i_1;
            
            do
            {
                j_1 += 1;
                *i_4 += *$v1_35;
                $v1_35 = &$v1_35[1];
                *$v0_12 += *$v0_38;
                $v0_38 += 4;
                i_4[1] += $v1_35[3];
                $v0_12[1] += *($v0_38 + 0xc);
                i_4[2] += $v1_35[6];
                $v0_12[2] += *($v0_38 + 0x18);
                i_4[3] += $v1_35[9];
                $v0_12[3] += *($v0_38 + 0x24);
                i_4[4] += $v1_35[0xd];
                $v0_12[4] += *($v0_38 + 0x34);
                i_4[5] += $v1_35[0x35];
                $v0_12[5] += *($v0_38 + 0xd4);
                i_4[6] += $v1_35[0x39];
                $v0_12[6] += *($v0_38 + 0xe4);
                i_4[7] += $v1_35[0x3c];
                $v0_12[7] += *($v0_38 + 0xf0);
                i_4[8] += $v1_35[0x3f];
                $v0_12[8] += *($v0_38 + 0xfc);
                i_4[9] += $v1_35[0x43];
                $v0_12[9] += *($v0_38 + 0x10c);
                i_4[0xa] += $v1_35[0x6b];
                $v0_12[0xa] += *($v0_38 + 0x1ac);
                i_4[0xb] += $v1_35[0x6f];
                $v0_12[0xb] += *($v0_38 + 0x1bc);
                i_4[0xc] += $v1_35[0x72];
                $v0_12[0xc] += *($v0_38 + 0x1c8);
                i_4[0xd] += $v1_35[0x75];
                $v0_12[0xd] += *($v0_38 + 0x1d4);
                i_4[0xe] += $v1_35[0x79];
                $v0_12[0xe] += *($v0_38 + 0x1e4);
            } while (j_1 != 4);
            
            i_1 += 0x48;
            $v0_36 = $v0_10;
        } while (i_1 != 0x120);
        
        int32_t $t4_2 = arg22[3];
        int32_t* i_2 = i_4;
        int32_t* $t3_2 = $v0_12;
        int32_t $v0_39 = 0;
        
        do
        {
            int32_t $lo_10 = ((*$t3_2 >> 6) + 1) / 3;
            int32_t $lo_11 = ((*i_2 >> 6) + 1) / 3;
            int32_t $t0_3 = $t4_2 < $lo_10 ? 1 : 0;
            int32_t $v1_40;
            
            if ($t4_2 >= $lo_11)
            {
                int32_t $a0_22 = arg22[2];
                
                if ($a0_22 >= $lo_11)
                {
                    int32_t $a2_82 = arg22[1];
                    
                    if ($a2_82 >= $lo_11)
                    {
                        int32_t $a3_62 = *arg22;
                        
                        if ($a3_62 >= $lo_11)
                        {
                            if ($t0_3)
                                $v1_40 = arg22[0x18];
                            else if ($a0_22 < $lo_10)
                                $v1_40 = arg22[0x19];
                            else if ($a2_82 < $lo_10)
                                $v1_40 = arg22[0x1a];
                            else if ($a3_62 >= $lo_10)
                                $v1_40 = arg22[0x1c];
                            else
                                $v1_40 = arg22[0x1b];
                        }
                        else if ($t0_3)
                            $v1_40 = arg22[0x13];
                        else if ($a0_22 < $lo_10)
                            $v1_40 = arg22[0x14];
                        else if ($a2_82 < $lo_10)
                            $v1_40 = arg22[0x15];
                        else if ($a3_62 >= $lo_10)
                            $v1_40 = arg22[0x17];
                        else
                            $v1_40 = arg22[0x16];
                    }
                    else if ($t0_3)
                        $v1_40 = arg22[0xe];
                    else if ($a0_22 < $lo_10)
                        $v1_40 = arg22[0xf];
                    else if ($a2_82 < $lo_10)
                        $v1_40 = arg22[0x10];
                    else if (*arg22 >= $lo_10)
                        $v1_40 = arg22[0x12];
                    else
                        $v1_40 = arg22[0x11];
                }
                else if ($t0_3)
                    $v1_40 = arg22[9];
                else if ($a0_22 < $lo_10)
                    $v1_40 = arg22[0xa];
                else if (arg22[1] < $lo_10)
                    $v1_40 = arg22[0xb];
                else if (*arg22 >= $lo_10)
                    $v1_40 = arg22[0xd];
                else
                    $v1_40 = arg22[0xc];
            }
            else if ($t0_3)
                $v1_40 = arg22[4];
            else if (arg22[2] < $lo_10)
                $v1_40 = arg22[5];
            else if (arg22[1] < $lo_10)
                $v1_40 = arg22[6];
            else if (*arg22 >= $lo_10)
                $v1_40 = arg22[8];
            else
                $v1_40 = arg22[7];
            
            i_2 = &i_2[1];
            $v0_39 += $v1_40;
            $t3_2 = &$t3_2[1];
        } while (&i_4[0xf] != i_2);
        
        arg23[6] = $v0_39;
        $v1_34 = var_c4_1_3 + var_c0_1_3 + var_bc_1_3 + var_b8_1_3 + $v0_39;
        arg23[2] = var_c4_1_4;
        arg23[1] = $v1_34;
        arg23[3] = var_c0_1_4;
        arg23[4] = var_bc_1_4;
        arg23[5] = var_b8_1_4;
    }
    
    int32_t* $a2_86 = arg15;
    int32_t $v0_40 = 0;
    
    while (true)
    {
        int32_t $a0_38 = *$a2_86;
        int32_t $v0_50;
        
        if ($v1_34 >= $a0_38)
        {
            $a2_86 = &$a2_86[1];
            
            if ($v0_40 != 8)
            {
                $v0_40 += 1;
                continue;
            }
            else
            {
                *arg7 = arg16[8];
                arg7[1] = arg17[8];
                arg7[2] = arg18[8];
                arg7[3] = arg19[8];
                $v0_50 = arg20[8];
            }
        }
        else
        {
            if ($v0_40)
            {
                int32_t $a3_65 = $v1_34 - *(arg15 + ($v0_40 << 2) - 4);
                int32_t $v1_50 = $a0_38 - $v1_34;
                int32_t $a2_89 = $a3_65 + $v1_50;
                int32_t $t1_35 = $a2_89 >> 1;
                int32_t $lo_13;
                int32_t $hi_11;
                $hi_11 = HIGHD($a3_65 * arg16[$v0_40] + $v1_50 * *(arg16 + ($v0_40 << 2) - 4));
                $lo_13 = LOWD($a3_65 * arg16[$v0_40] + $v1_50 * *(arg16 + ($v0_40 << 2) - 4));
                *arg7 = ($lo_13 + $t1_35) / $a2_89;
                int32_t $lo_16;
                int32_t $hi_13;
                $hi_13 = HIGHD($a3_65 * arg17[$v0_40] + $v1_50 * *(arg17 + ($v0_40 << 2) - 4));
                $lo_16 = LOWD($a3_65 * arg17[$v0_40] + $v1_50 * *(arg17 + ($v0_40 << 2) - 4));
                arg7[1] = ($lo_16 + $t1_35) / $a2_89;
                int32_t $lo_19;
                int32_t $hi_15;
                $hi_15 = HIGHD($a3_65 * arg18[$v0_40] + $v1_50 * *(arg18 + ($v0_40 << 2) - 4));
                $lo_19 = LOWD($a3_65 * arg18[$v0_40] + $v1_50 * *(arg18 + ($v0_40 << 2) - 4));
                arg7[2] = ($lo_19 + $t1_35) / $a2_89;
                int32_t $lo_22;
                int32_t $hi_17;
                $hi_17 = HIGHD($a3_65 * arg19[$v0_40] + $v1_50 * *(arg19 + ($v0_40 << 2) - 4));
                $lo_22 = LOWD($a3_65 * arg19[$v0_40] + $v1_50 * *(arg19 + ($v0_40 << 2) - 4));
                arg7[3] = ($lo_22 + $t1_35) / $a2_89;
                int32_t $lo_25;
                int32_t $hi_19;
                $hi_19 = HIGHD($a3_65 * arg20[$v0_40] + $v1_50 * *(arg20 + ($v0_40 << 2) - 4));
                $lo_25 = LOWD($a3_65 * arg20[$v0_40] + $v1_50 * *(arg20 + ($v0_40 << 2) - 4));
                arg7[4] = ($lo_25 + $t1_35) / $a2_89;
                break;
            }
            
            *arg7 = *arg16;
            arg7[1] = *arg17;
            arg7[2] = *arg18;
            arg7[3] = *arg19;
            $v0_50 = *arg20;
        }
        
        arg7[4] = $v0_50;
        break;
    }
    
    int32_t $v0_64 = arg6[1];
    int32_t $s7_4 = arg7[1];
    int32_t $s6_17 = arg7[2];
    int32_t $v0_66 = *arg6;
    int32_t $s5_1 = arg7[3];
    int32_t $s4_1 = arg6[2];
    int32_t $s3_1 = arg6[3];
    int32_t $v0_68 = arg6[4];
    int32_t $v0_69 = *arg7;
    int32_t $v0_70 = arg7[4];
    int32_t $v0_71 = $v0_70;
    
    if ($a0_3 == 1)
    {
        int32_t i_3 = 0;
        int32_t $v0_72 = 1;
        
        do
        {
            int32_t $t1_38;
            int32_t $t3_3;
            
            if ($v0_72)
            {
                $t1_38 = 0;
                $t3_3 = i_3 + 2;
            }
            else
            {
                $t1_38 = i_3 - 2;
                $t3_3 = 9;
                
                if (i_3 < 7)
                    $t3_3 = i_3 + 2;
            }
            
            int32_t j_2 = 0;
            char* $t0_11 = i_3 * 0x12 + $v0_13;
            
            do
            {
                int32_t $v1_53;
                int32_t $t2;
                int32_t $t4_3;
                
                if (j_2 < 3)
                {
                    $t2 = 0;
                    $t4_3 = j_2 + 2;
                    $v1_53 = $t3_3 - $t1_38 + 1;
                }
                else
                {
                    $t2 = j_2 - 2;
                    $t4_3 = 0x11;
                    
                    if (j_2 < 0xf)
                    {
                        $t4_3 = j_2 + 2;
                        $v1_53 = $t3_3 - $t1_38 + 1;
                    }
                    else
                        $v1_53 = $t3_3 - $t1_38 + 1;
                }
                
                int32_t $v0_82 = ($t4_3 - $t2 + 1) * $v1_53;
                int32_t $t7_1 = $t1_38;
                int16_t* $a1_12 = $v0_9 + (($t1_38 * 0x12 + $t2) << 1);
                int16_t* $t8_2 = $a1_12;
                int32_t $v1_56 = 0;
                int32_t var_c4_2_1 = 0;
                
                while (true)
                {
                    int16_t* $ra_19 = $t8_2;
                    
                    if ($t3_3 < $t7_1)
                        break;
                    
                    int32_t $t9_8 = $t2;
                    
                    while ($t4_3 >= $t9_8)
                    {
                        int32_t $t5_5 = *$ra_19;
                        $t9_8 += 1;
                        var_c4_2_2 += $t5_5;
                        int32_t $a3_68 = $v1_56 + $t5_5;
                        $v1_56 -= $t5_5;
                        
                        if ($t5_5 >= 1)
                            $v1_56 = $a3_68;
                        
                        $ra_19 = &$ra_19[1];
                    }
                    
                    $t7_1 += 1;
                    $t8_2 = &$t8_2[0x12];
                }
                
                int32_t $t7_2 = $t1_38;
                int32_t $t5_7 = 0;
                
                while (true)
                {
                    int16_t* $t9_9 = $a1_12;
                    
                    if ($t3_3 < $t7_2)
                        break;
                    
                    int32_t $t8_4 = $t2;
                    
                    while (true)
                    {
                        int32_t $ra_20 = $t4_3 < $t8_4 ? 1 : 0;
                        $t8_4 += 1;
                        
                        if ($ra_20)
                            break;
                        
                        int32_t $t5_9 = *$t9_9 - var_c4_2_3 / $v0_82;
                        $t5_7 = $t5_9 * $t5_9;
                        $t9_9 = &$t9_9[1];
                    }
                    
                    $t7_2 += 1;
                    $a1_12 = &$a1_12[0x12];
                }
                
                int32_t $lo_30 = ($t5_7 / $v0_82 + $a0_4) * ($v1_56 << 6) / $v0_82 / $a0_4;
                
                if ($a0_6 < $lo_30)
                    *$t0_11 = -1;
                else if ($lo_30 >= $a0_5)
                    *$t0_11 = (($lo_30 - $a0_5) << 8) / $v0_7;
                else
                    *$t0_11 = 0;
                
                j_2 += 1;
                $t0_11 = &$t0_11[1];
            } while (j_2 != 0x12);
            
            i_3 += 1;
            $v0_72 = i_3 < 3 ? 1 : 0;
        } while (i_3 != 0xa);
        
        tisp_defog_max_filter3($v0_13, $v0_13);
        tisp_defog_img_filter5($v0_13, $v0_13, &var_f0_4);
        $v0_71 = $v0_70;
    }
    
    void* $a1_18 = $v0_8;
    int32_t* $a2_91 = arg11;
    char* $a3_71 = $v0_13;
    void* $t1_39 = $a1_18 + 0xb4;
    
    do
    {
        uint32_t $v0_93 = *$a1_18;
        int32_t $v1_66 = *arg6;
        int32_t $v0_94;
        
        if ($v0_93 >= $v1_66)
        {
            int32_t $a0_72 = arg6[1];
            
            if ($v0_93 >= $a0_72)
            {
                int32_t $v1_68 = arg6[2];
                
                if ($v0_93 >= $v1_68)
                {
                    int32_t $a0_74 = arg6[3];
                    
                    if ($v0_93 < $a0_74)
                        $v0_94 = ($s5_1 - $s6_17) * ($v0_93 - $v1_68) / ($s3_1 - $s4_1) + arg7[2];
                    else if ($v0_93 >= arg6[4])
                        $v0_94 = arg7[4];
                    else
                        $v0_94 = ($v0_71 - $s5_1) * ($v0_93 - $a0_74) / ($v0_68 - $s3_1) + arg7[3];
                }
                else
                    $v0_94 = ($s6_17 - $s7_4) * ($v0_93 - $a0_72) / ($s4_1 - $v0_64) + arg7[1];
            }
            else
                $v0_94 = ($s7_4 - $v0_69) * ($v0_93 - $v1_66) / ($v0_64 - $v0_66) + *arg7;
        }
        else
            $v0_94 = *arg7;
        
        int32_t $v1_72 = ($v0_94 * 0x28f) >> 8;
        
        if ($v1_72 < 0)
            $v1_72 = 0;
        
        int32_t $v0_106 = $v1_72;
        
        if ($v1_72 >= 0x100)
            $v0_106 = 0xff;
        
        if ($a0_3 != 1)
            *$a2_91 = $v0_106;
        else
            *$a2_91 = (((0xff - $v0_106) * *$a3_71) >> 8) + $v0_106;
        
        $a1_18 += 1;
        $a3_71 = &$a3_71[1];
        $a2_91 = &$a2_91[1];
    } while ($t1_39 != $a1_18);
    
    kfree($v0_8, $a1_18, $a2_91, $a3_71);
    kfree($v0_9);
    kfree($v0_10);
    kfree($v0_11);
    kfree(i_4);
    kfree($v0_12);
    return kfree($v0_13);
}

