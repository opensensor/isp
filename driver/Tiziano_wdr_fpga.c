#include "include/main.h"


  int32_t Tiziano_wdr_fpga(int32_t* arg1, int32_t* arg2, int32_t* arg3, int32_t* arg4, int32_t* arg5, int32_t* arg6, int32_t* arg7, int32_t* arg8, int32_t* arg9, int32_t* arg10, int32_t* arg11, int32_t* arg12, int32_t* arg13, int32_t* arg14, int32_t* arg15, int32_t* arg16, int32_t* arg17, int32_t* arg18, int32_t* arg19, int32_t* arg20, int32_t* arg21, int32_t* arg22, int32_t* arg23, int32_t* arg24, void* arg25, int32_t* arg26, int32_t* arg27, int32_t* arg28, int32_t arg29, int32_t* arg30)

{
    int32_t* arg_c = arg4;
    int32_t* arg_0 = arg1;
    int32_t* arg_4 = arg2;
    int32_t* arg_8 = arg3;
    int32_t* i_20 = arg15;
    int32_t $a3_14 = *arg1;
    int32_t $a3_15 = arg1[1];
    int32_t var_124 = *arg2;
    int32_t $a3_17 = arg2[1];
    int32_t $a3_18 = arg2[2];
    int32_t $s6 = *arg3;
    int32_t $a3_19 = arg2[3];
    int32_t $a1 = arg2[4];
    int32_t $a3_20 = arg3[1];
    int32_t $fp = arg8[3];
    int32_t $a3_21 = arg3[2];
    int32_t $a3_22 = arg3[3];
    int32_t $a3_23 = arg1[2];
    int32_t $a3_24 = *arg8;
    int32_t $a3_25 = arg8[1];
    int32_t $a3_26 = arg8[2];
    int32_t $a3_27 = arg8[4];
    int32_t $a3_28 = arg3[4];
    int32_t $a3_29 = arg3[5];
    int32_t $a2 = arg3[6];
    int32_t $a2_1 = arg8[5];
    int32_t $a1_1 = *arg6;
    int32_t $a1_2 = arg6[1];
    int32_t $a1_3 = arg6[2];
    int32_t $a1_4 = arg6[3];
    int32_t $a1_5 = arg6[4];
    int32_t $a1_6 = arg6[5];
    int32_t $a1_7 = arg6[6];
    int32_t $a1_8 = arg6[7];
    int32_t $a1_9 = arg6[0xb];
    int32_t $a1_10 = arg6[0xd];
    int32_t $a1_11 = arg6[0xe];
    int32_t $a1_12 = arg6[0xf];
    int32_t var_120 = arg6[0x10];
    int32_t $a1_14 = arg6[0x11];
    int32_t $a1_15 = arg6[0x12];
    int32_t $v1_1 = arg6[0x13];
    int32_t $v1_2 = arg8[6];
    int32_t* $a1_16 = &claHistG1;
    int32_t $v1_3 = arg8[7];
    int32_t* $t2 = &claHistG1;
    int32_t $v1_4 = arg8[8];
    int32_t $v1_5 = arg8[9];
    int32_t $v1_6 = arg8[0xa];
    int32_t $v1_7 = arg8[0xb];
    int32_t $v1_8 = arg8[0xc];
    int32_t $v1_9 = arg8[0xd];
    int32_t $v1_10 = arg8[0xe];
    int32_t $v1_11 = arg1[3];
    int32_t $v1_12 = arg8[0xf];
    int32_t $v0_3 = arg8[0x10];
    int32_t $v0_4 = arg7[0xe];
    int32_t $v0_5 = arg7[0xf];
    int32_t* $t6 = &claHistR0;
    int32_t* $v1_13 = &claHistR1;
    int32_t* $v0_14 = &data_aa51c;
    int32_t* var_114 = &claHistG0;
    int32_t* $t5 = &claHistG0;
    int32_t* $t1 = &data_aa51c;
    int32_t* $t3 = &claHistR1;
    int32_t* $t4 = &claHistB0;
        int32_t $a2_2 = *$t6;
        int32_t $t8_18 = *(arg21 + i + 4);
    claHistR0 = *arg16;
    claHistG0 = *arg17;
    claHistB0 = *arg18;
    claHistR1 = *arg19;
    claHistG1 = *arg20;
    data_aa51c = *arg21;
    
    for (int32_t i = 0; (uintptr_t)i != 0x3fc; )
    {
        $t6 = &$t6[1];
        *$t6 = *(arg16 + i + 4) + $a2_2;
        $t4 = &$t4[1];
        $t5[1] = *(arg17 + i + 4) + *$t5;
        $t5 = &$t5[1];
        *$t4 = *(arg18 + i + 4) + *($t4 - 4);
        $t2 = &$t2[1];
        $t3[1] = *(arg19 + i + 4) + *$t3;
        $t3 = &$t3[1];
        *$t2 = *(arg20 + i + 4) + *($t2 - 4);
        i += 4;
        $t1[1] = $t8_18 + *$t1;
        $t1 = &$t1[1];
    }
    
    int32_t claHistG1_1 = claHistG1;
    int32_t $a0_1 = data_ba920_1;
    int32_t $a0_2 = data_ba924_1;
    int32_t $a0_3 = data_ba928_1;
    void* var_34_10 = &data_b0000_3;
    void* var_ec_1 = &data_a9d1c;
    int32_t* var_30 = &data_a911c_1;
    int32_t $a0_4 = $a3_14;
    void* $t4_1 = &data_a9d1c_1;
    int32_t* j_2;
    
    if ($a0_4)
    {
        int32_t* $t3_2 = &data_a911c;
        int32_t* $t7_1 = &claHistR0;
            int32_t $t1_7 = *$t7_1;
            int32_t $t0_10 = *$v1_13;
            int32_t $t5_1 = $t1_7 - $t0_10;
            int32_t* $t8_20 = &claHistR0;
                int32_t $t2_9 = $t8_20[1];
                int32_t $t2_10 = $t0_10 - $t2_9;
                    int32_t $t6_5 = j;
        
        for (int32_t i_1 = 0; (uintptr_t)i_1 != 0x100; )
        {
            i_1 += 1;
            *$t3_2 = (i_1 << 4) - 1;
            
            if ($t1_7 < $t0_10)
                $t5_1 = $t0_10 - $t1_7;
            
            *$t4_1 = i_1;
            
            for (int32_t j = 2; (uintptr_t)j != 0x101; )
            {
                
                if ($t0_10 < $t2_9)
                    $t2_10 = $t2_9 - $t0_10;
                
                if ($t2_10 >= $t5_1)
                    j += 1;
                else
                {
                    
                    if (j < i_1)
                        $t6_5 = i_1;
                    
                    *$t3_2 = ($t6_5 << 4) - 1;
                    *$t4_1 = $t6_5;
                    $t5_1 = $t2_10;
                    j += 1;
                }
                
                $t8_20 = &$t8_20[1];
            }
            
            $v1_13 = &$v1_13[1];
            $t7_1 = &$t7_1[1];
            $t4_1 += 4;
            $t3_2 = &$t3_2[1];
        }
        
        int32_t* $t5_5 = var_114_2;
        void* $a3_32 = &data_a991c;
        void* $a0_10 = &data_a8d1c;
        
        for (int32_t i_2 = 0; (uintptr_t)i_2 != 0x100; )
        {
            int32_t $t1_10 = *$t5_5;
            int32_t $t0_11 = *$a1_16;
            int32_t $t3_3 = $t1_10 - $t0_11;
            int32_t* $t6_6 = var_114;
                int32_t $t2_12 = $t6_6[1];
                int32_t $t2_13 = $t0_11 - $t2_12;
                    int32_t $t4_10 = j_1;
            i_2 += 1;
            *$a0_10 = (i_2 << 4) - 1;
            
            if ($t1_10 < $t0_11)
                $t3_3 = $t0_11 - $t1_10;
            
            *$a3_32 = i_2;
            
            for (int32_t j_1 = 2; (uintptr_t)j_1 != 0x101; )
            {
                
                if ($t0_11 < $t2_12)
                    $t2_13 = $t2_12 - $t0_11;
                
                if ($t2_13 >= $t3_3)
                    j_1 += 1;
                else
                {
                    
                    if (j_1 < i_2)
                        $t4_10 = i_2;
                    
                    *$a0_10 = ($t4_10 << 4) - 1;
                    *$a3_32 = $t4_10;
                    $t3_3 = $t2_13;
                    j_1 += 1;
                }
                
                $t6_6 = &$t6_6[1];
            }
            
            $a1_16 = &$a1_16[1];
            $t5_5 = &$t5_5[1];
            $a3_32 += 4;
            $a0_10 += 4;
        }
        
        int32_t** $a1_21 = &data_aa11c;
        void* $a0_11 = &data_a951c;
        int32_t* $t3_7 = &claHistB0;
        
        for (int32_t* i_3 = nullptr; (uintptr_t)i_3 != 0x100; )
        {
            int32_t $a3_33 = *$t3_7;
            int32_t $a2_15 = *$v0_14;
            int32_t $t1_13 = $a3_33 - $a2_15;
            int32_t* $t4_11 = &claHistB0;
                int32_t $t0_13 = $t4_11[1];
                int32_t $t0_14 = $a2_15 - $t0_13;
                    int32_t* $t2_17 = j_2;
            i_3 += 1;
            *$a0_11 = (i_3 << 4) - 1;
            
            if ($a3_33 < $a2_15)
                $t1_13 = $a2_15 - $a3_33;
            
            *$a1_21 = i_3;
            
            for (j_2 = 2; (uintptr_t)j_2 != 0x101; )
            {
                
                if ($a2_15 < $t0_13)
                    $t0_14 = $t0_13 - $a2_15;
                
                if ($t0_14 >= $t1_13)
                    j_2 += 1;
                else
                {
                    
                    if (j_2 < i_3)
                        $t2_17 = i_3;
                    
                    *$a0_11 = ($t2_17 << 4) - 1;
                    *$a1_21 = $t2_17;
                    $t1_13 = $t0_14;
                    j_2 += 1;
                }
                
                $t4_11 = &$t4_11[1];
            }
            
            $v0_14 = &$v0_14[1];
            $t3_7 = &$t3_7[1];
            $a1_21 = &$a1_21[1];
            $a0_11 += 4;
        }
    }
    else
    {
        void* $t0_1 = &data_a9d1c;
        int32_t* $t3_1 = &data_a911c;
            int32_t $t6_1 = *$v1_13;
            int32_t* $t2_1 = &claHistR0;
            int32_t $t1_1 = 1;
        
        while (true)
        {
            $a0_4 += 1;
            *$t0_1 = $a0_4;
            int32_t $t1_2;
            
            while (true)
            {
                if (*$t2_1 >= $t6_1)
                {
                    $t1_1 += 1;
                label_2fcf4:
                    $t2_1 = &$t2_1[1];
                    
                    if ($(uintptr_t)t1_1 != 0x100)
                        continue;
                }
                else
                {
                    if ($t2_1[1] < $t6_1)
                    {
                        $t1_1 += 1;
                        goto label_2fcf4;
                    }
                    
                    if ($t1_1 < $a0_4)
                    {
                        $t1_2 = *$t0_1;
                        break;
                    }
                    
                    *$t0_1 = $t1_1;
                }
                
                $t1_2 = *$t0_1;
                break;
            }
            
            $v1_13 = &$v1_13[1];
            *$t3_1 = ($t1_2 << 4) - 1;
            $t0_1 += 4;
            $t3_1 = &$t3_1[1];
            
            if ($(uintptr_t)a0_4 == 0x100)
                break;
        }
        
        void* $v1_14 = &data_a991c_1;
        void* $a3_30 = &data_a8d1c_1;
        int32_t $a0_5 = 0;
        
        while (true)
        {
            int32_t $t4_6 = *$a1_16;
            int32_t* $t1_5 = var_114;
            int32_t $t0_2 = 1;
            $a0_5 += 1;
            *$v1_14 = $a0_5;
            int32_t $t0_3;
            
            while (true)
            {
                if (*$t1_5 >= $t4_6)
                {
                    $t0_2 += 1;
                label_2fd7c:
                    $t1_5 = &$t1_5[1];
                    
                    if ($(uintptr_t)t0_2 != 0x100)
                        continue;
                }
                else
                {
                    if ($t1_5[1] < $t4_6)
                    {
                        $t0_2 += 1;
                        goto label_2fd7c;
                    }
                    
                    if ($t0_2 < $a0_5)
                    {
                        $t0_3 = *$v1_14;
                        break;
                    }
                    
                    *$v1_14 = $t0_2;
                }
                
                $t0_3 = *$v1_14;
                break;
            }
            
            $a1_16 = &$a1_16[1];
            *$a3_30 = ($t0_3 << 4) - 1;
            $v1_14 += 4;
            $a3_30 += 4;
            
            if ($(uintptr_t)a0_5 == 0x100)
                break;
        }
        
        void* $v1_15 = &data_aa11c_1;
        void* $a1_17 = &data_a951c_1;
        int32_t $a0_6 = 0;
        
        while (true)
        {
            int32_t $t2_7 = *$v0_14;
            int32_t $a2_10 = 1;
            $a0_6 += 1;
            *$v1_15 = $a0_6;
            j_2 = &claHistB0;
            int32_t $a2_11;
            
            while (true)
            {
                if (*j_2 >= $t2_7)
                {
                    $a2_10 += 1;
                label_2fe04:
                    j_2 = &j_2[1];
                    
                    if ($(uintptr_t)a2_10 != 0x100)
                        continue;
                }
                else
                {
                    if (j_2[1] < $t2_7)
                    {
                        $a2_10 += 1;
                        goto label_2fe04;
                    }
                    
                    j_2 = $a2_10 < $a0_6 ? 1 : 0;
                    
                    if (j_2)
                    {
                        $a2_11 = *$v1_15;
                        break;
                    }
                    
                    *$v1_15 = $a2_10;
                }
                
                $a2_11 = *$v1_15;
                break;
            }
            
            $v0_14 = &$v0_14[1];
            *$a1_17 = ($a2_11 << 4) - 1;
            $v1_15 += 4;
            $a1_17 += 4;
            
            if ($(uintptr_t)a0_6 == 0x100)
                break;
        }
    }
    
    int32_t var_158;
    tx_isp_module_deinit(&var_158_1, arg29, &data_a911c_2, j_2);
    void* var_a0_1_2 = &data_b0000_4;
    int32_t var_164_4;
    tx_isp_module_deinit(&var_164_5, arg29, &data_a8d1c_2);
    void* var_114_1_1 = &data_b0000_5;
    int32_t var_170;
    tx_isp_module_deinit(&var_170_2, arg29, &data_a951c_2, &data_b0000_6);
    *arg30 = var_158_2;
    int32_t var_154;
    arg30[1] = var_154_1;
    int32_t var_150;
    arg30[2] = var_150_1;
    arg30[3] = var_164_6;
    int32_t i_4 = 0;
    int32_t var_160_4;
    arg30[4] = var_160_5;
    int32_t var_15c_2;
    arg30[5] = var_15c_3;
    arg30[6] = var_170_3;
    int32_t var_16c_11;
    arg30[7] = var_16c_12;
    int32_t var_168_4;
    arg30[8] = var_168_5;
    void* var_114_2_1;
    void* var_fc_1;
    void* var_a0_2_2;
    
    do
    {
        int32_t $t2_20 = *(arg23 + i_4);
        void* $t1_23 = &data_a8a94 + i_4;
    int32_t* $s2_1 = &data_a911c;
    void* $v1_26 = var_a0_1 - 0x72e4;
    void* $v0_17 = var_114_1 - 0x6ae4;
    void* $a3_36 = &arg22[2];
    void* $a2_16 = &arg24[2];
    void* $a1_23 = &arg23[2];
    void* $t1_24 = $v0_17;
    void* $t2_21 = $v1_26;
    int32_t* $t3_8 = &data_a911c;
    int32_t $t0_15 = 0;
    int32_t i_5 = 1;
    void* $t6_7 = $v1_26;
    void* $ra = $v0_17;
        var_114_2 = &data_a8bd8;
        *(((void**)((char*)&data_a8bd8 + i_4))) = *(arg22 + i_4); // Fixed void pointer dereference
        *(((void**)((char*)&data_a8950 + i_4))) = *(arg24 + i_4); // Fixed void pointer dereference
        i_4 += 4;
        *$t1_23 = $t2_20;
        var_fc_1 = &data_a8950;
        var_a0_2 = &data_a8a94;
    } while ((uintptr_t)i_4 != 0x144);
    
    arg22[1] = data_a911c;
    arg24[1] = *(var_a0_1 - 0x72e4);
    arg23[1] = *(var_114_1 - 0x6ae4);
    
    do
    {
        int32_t $t4_20;
        
        if ((uintptr_t)i_5 < 0x10)
        {
            *$a3_36 = *(var_30 + $t0_15 + 4);
            *$a2_16 = *($t6_7 + $t0_15 + 4);
            $t4_20 = *($ra + $t0_15 + 4);
        }
        else if (i_5 - 0x10 >= 8)
        {
            *$a3_36 = *($s2_1 - 0xe4);
            *$a2_16 = *($v1_26 - 0xe4);
            $t4_20 = *($v0_17 - 0xe4);
        }
        else
        {
            *$a3_36 = *($t3_8 - 0x34);
            *$a2_16 = *($t2_21 - 0x34);
            $t4_20 = *($t1_24 - 0x34);
        }
        
        i_5 += 1;
        *$a1_23 = $t4_20;
        $t0_15 += 4;
        $a3_36 += 4;
        $a2_16 += 4;
        $a1_23 += 4;
        $t3_8 = &$t3_8[2];
        $t2_21 += 8;
        $t1_24 += 8;
        $s2_1 = &$s2_1[4];
        $v1_26 += 0x10;
        $v0_17 += 0x10;
    } while ((uintptr_t)i_5 != 0x50);
    
    int32_t $lo = (getVar(var_34_11 - 0x62e4) + getVar(&data_a991c_2) + getVar(&data_aa11c_2)) / 3;
    int32_t $a3_37 = 0;
    int32_t $a0_17;
    
    if (arg7[0xc] != 1)
        $a0_17 = arg7[0xd];
    else
    {
        int32_t $a1_24 = arg7[4];
            int32_t $v1_28 = arg7[5];
                int32_t $a0_19 = $v1_28 - $a1_24;
        
        if ($lo < $a1_24)
            $a0_17 = arg7[0xd];
        else
        {
            $a3_37 = arg7[7];
            
            if ($lo < $v1_28)
            {
                int32_t $lo_2;
                int32_t $hi_2;
                $hi_2 = HIGHD($v1_28 * $a0_19 + ($a3_37 - arg7[6]) * ($lo - $a1_24));
                $lo_2 = LOWD($v1_28 * $a0_19 + ($a3_37 - arg7[6]) * ($lo - $a1_24));
                $a3_37 = ($lo_2 + $a0_19 / 2) / $a0_19;
            }
            
            int32_t* $t1_25 = &arg22[1];
            void* $t0_17 = &arg24[1];
            void* $a2_18 = &arg23[1];
            
            for (int32_t i_6 = 0; (uintptr_t)i_6 != 0x140; )
            {
                int32_t $v1_44 = *(var_a0_2 + i_6 + 4);
                int32_t $lo_5;
                int32_t $hi_4;
                $hi_4 = HIGHD((0x10 - $a3_37) * *$t1_25);
                $lo_5 = LOWD((0x10 - $a3_37) * *$t1_25);
                $t1_25 = &$t1_25[1];
                int32_t $lo_6;
                int32_t $hi_5;
                $hi_5 = HIGHD(COMBINE($hi_4, $lo_5) + $a3_37 * *(var_114_2 + i_6 + 4));
                $lo_6 = LOWD(COMBINE($hi_4, $lo_5) + $a3_37 * *(var_114_2 + i_6 + 4));
                $t0_17 += 4;
                $a2_18 += 4;
                *($t1_25 - 4) = ($lo_6 + 8) / 0x10;
                int32_t $lo_9;
                int32_t $hi_7;
                $hi_7 = HIGHD((0x10 - $a3_37) * *($t0_17 - 4) + $a3_37 * *(var_fc_1 + i_6 + 4));
                $lo_9 = LOWD((0x10 - $a3_37) * *($t0_17 - 4) + $a3_37 * *(var_fc_1 + i_6 + 4));
                *($t0_17 - 4) = ($lo_9 + 8) / 0x10;
                i_6 += 4;
                int32_t $lo_12;
                int32_t $hi_9;
                $hi_9 = HIGHD((0x10 - $a3_37) * *($a2_18 - 4) + $a3_37 * $v1_44);
                $lo_12 = LOWD((0x10 - $a3_37) * *($a2_18 - 4) + $a3_37 * $v1_44);
                *($a2_18 - 4) = ($lo_12 + 8) / 0x10;
            }
            
            $a0_17 = arg7[0xd];
        }
    }
    
    int32_t* $v0_24;
    
    if ($a0_17 != 1)
        $v0_24 = arg22;
    else
    {
        int32_t $a1_26 = arg7[8];
            int32_t $v1_49 = arg7[9];
            int32_t $a2_19 = arg7[0xb];
            int32_t var_110_1 = $a2_19;
                int32_t $a0_21 = arg7[0xa];
                int32_t $v1_50 = $v1_49 - $a1_26;
        int32_t $v0_25;
        
        if ($lo < $a1_26)
            $v0_25 = $v0_4;
        else
        {
            
            if ($lo < $v1_49)
            {
                int32_t $lo_15;
                int32_t $hi_12;
                $hi_12 = HIGHD($a0_21 * $v1_50 + ($a2_19 - $a0_21) * ($lo - $a1_26));
                $lo_15 = LOWD($a0_21 * $v1_50 + ($a2_19 - $a0_21) * ($lo - $a1_26));
                var_110_1 = ($lo_15 + $v1_50 / 2) / $v1_50;
            }
            
            $v0_25 = var_110_1;
        }
        
        arg7[0xe] = $v0_25;
        $v0_24 = arg22;
    }
    
    int32_t $a1_27 = $v0_24[1];
    void* $t4_29 = &data_aa120;
    void* $t3_9 = &data_a9920;
    void* $t2_23 = &data_a9d20;
    int32_t $t0_19 = 0;
    int32_t $t1_26 = 1;
    int32_t $v0_32 = (($v0_24[2] - $a1_27) * 0xfffffff1 + ($a1_27 << 4) + 8) / 0x10;
    
    if ($v0_32 < 0)
        $v0_32 = 0;
    
    *arg22 = $v0_32;
    int32_t $a1_32 = arg24[1];
    int32_t $a1_36 = ((arg24[2] - $a1_32) * 0xfffffff1 + ($a1_32 << 4) + 8) / 0x10;
    
    if ($a1_36 < 0)
        $a1_36 = 0;
    
    *arg24 = $a1_36;
    int32_t $a1_37 = arg23[1];
    int32_t $s3_2 = $a3_17 << 2;
    void* var_114_3 = &data_a991c_3;
    int32_t $v1_53 = ((arg23[2] - $a1_37) * 0xfffffff1 + ($a1_37 << 4) + 8) / 0x10;
    
    if ($v1_53 < 0)
        $v1_53 = 0;
    
    *arg23 = $v1_53;
    int32_t $a2_25 = 0;
    
    while ($t1_26 < $a3_18)
    {
        int32_t $s2_2 = *$t2_23;
        int32_t $s1_3 = *$t3_9;
        int32_t $t9_2 = *$t4_29;
        int32_t $t7_3 = $t1_26 - $a3_17;
        int32_t $a0_24 = $t1_26 + $a3_17;
        int32_t $t8_21 = 0;
        
        if ($v0_5 < $s2_2)
            break;
        
        
        if ($v0_5 < $s1_3)
            break;
        
        
        if ($v0_5 < $t9_2)
            break;
        
        
        if ($t7_3 < 0)
            $t7_3 = 0;
        
        int32_t $v0_63;
        int32_t $v1_56;
        int32_t $a1_41;
        
        while (true)
        {
            $v1_56 = *($s3_2 + $t2_23 + $t8_21);
            
            if ($v0_5 >= $v1_56)
            {
                $a1_41 = *($s3_2 + $t3_9 + $t8_21);
                
                if ($v0_5 < $a1_41)
                    $a0_24 -= 1;
                else
                {
                    $v0_63 = *($s3_2 + $t4_29 + $t8_21);
                    
                    if ($v0_5 >= $v0_63)
                        break;
                    
                    $a0_24 -= 1;
                }
            }
            else
                $a0_24 -= 1;
            
            $t8_21 -= 4;
        }
        
        int32_t $t8_22 = $t7_3 << 2;
        int32_t $a0_25 = $a0_24 - $t7_3;
        int32_t $t5_8 = $t1_26 - $t7_3;
        int32_t $lo_23 = $a0_25 / 2;
        int32_t $s5_4 = *($t8_22 + var_ec_1_1);
        $t1_26 += 1;
        $t4_29 += 4;
        $t3_9 += 4;
        $t2_23 += 4;
        int32_t $lo_25;
        int32_t $hi_19;
        $hi_19 = HIGHD($s5_4 * $a0_25 + ($v1_56 - $s5_4) * $t5_8);
        $lo_25 = LOWD($s5_4 * $a0_25 + ($v1_56 - $s5_4) * $t5_8);
        int32_t $lo_26 = ($lo_25 + $lo_23) / $a0_25;
        int32_t $s2_3 = $s2_2 - $lo_26;
        
        if ($s2_2 < $lo_26)
            $s2_3 = $lo_26 - $s2_2;
        
        if ($a3_19 < $s2_3)
            $a2_25 = 1;
        
        int32_t $v1_65 = *(var_114_3_1 + $t8_22);
        int32_t $lo_28;
        int32_t $hi_22;
        $hi_22 = HIGHD($a0_25 * $v1_65 + ($a1_41 - $v1_65) * $t5_8);
        $lo_28 = LOWD($a0_25 * $v1_65 + ($a1_41 - $v1_65) * $t5_8);
        int32_t $lo_29 = ($lo_28 + $lo_23) / $a0_25;
        int32_t $s1_4 = $s1_3 - $lo_29;
        
        if ($s1_3 < $lo_29)
            $s1_4 = $lo_29 - $s1_3;
        
        if ($s1_4 >= $s2_3)
            $s2_3 = $s1_4;
        
        if ($a3_19 < $s1_4)
            $a2_25 = 1;
        
        int32_t $v1_68 = *(&data_aa11c_3 + $t8_22);
        int32_t $lo_31;
        int32_t $hi_25;
        $hi_25 = HIGHD($a0_25 * $v1_68 + ($v0_63 - $v1_68) * $t5_8);
        $lo_31 = LOWD($a0_25 * $v1_68 + ($v0_63 - $v1_68) * $t5_8);
        
        if ($t0_19 >= $s2_3)
            $s2_3 = $t0_19;
        
        int32_t $lo_32 = ($lo_31 + $lo_23) / $a0_25;
        int32_t $t9_3 = $t9_2 - $lo_32;
        
        if ($t9_2 < $lo_32)
            $t9_3 = $lo_32 - $t9_2;
        
        if ($a3_19 < $t9_3)
            $a2_25 = 1;
        
        if ($t9_3 >= $s2_3)
            $s2_3 = $t9_3;
        
        int32_t $v1_69 = var_124_3;
        
        if (var_124_4 < $s2_3)
            $v1_69 = $s2_3;
        
        $t0_19 = $s2_3;
        var_124_5 = $v1_69;
    }
    
    int32_t $lo_22 = (($a3_20 << 7) + $s6 / 2) / $s6;
    int32_t $v0_46 = 0xffffffff;
    
    if ($lo_22 >= $a3_21)
        $v0_46 = 1;
    
    if ($lo_22 < $a3_22)
        $a2_25 = 2;
    
    if (arg7[0xc] == 1 && $a3_37 >= 9)
        $a2_25 = 0;
    
    int32_t $a0_27 = arg4[1];
    int32_t $a0_42;
    int32_t $lo_40;
    
    if ($a0_27 < $s6)
    {
        int32_t $a1_56 = arg4[2];
            int32_t $a3_53 = arg5[1];
            int32_t $a1_57 = $a1_56 - $a0_27;
        
        if ($a1_56 >= $s6)
        {
            int32_t $lo_42;
            int32_t $hi_33;
            $hi_33 = HIGHD($a3_53 * $a1_57 + (arg5[2] - $a3_53) * ($s6 - $a0_27));
            $lo_42 = LOWD($a3_53 * $a1_57 + (arg5[2] - $a3_53) * ($s6 - $a0_27));
            int32_t $lo_46;
            int32_t $hi_35;
            $hi_35 = HIGHD(COMBINE($a1_4 % 2, $a1_4 / 2)
                + ($a0_27 * 0xfffffffd + ($a1_3 << 4) + $s6) * ($lo_22 << 2));
            $lo_46 = LOWD(COMBINE($a1_4 % 2, $a1_4 / 2)
                + ($a0_27 * 0xfffffffd + ($a1_3 << 4) + $s6) * ($lo_22 << 2));
            $lo_40 = (((($lo_42 + $a1_57 / 2) / $a1_57) << 0xd) + $lo_46 / $a1_4 + 8) / 0x10;
            goto label_30870;
        }
        
        if ($a1_11 >= $s6)
        {
            int32_t $a0_39 = arg5[2];
            int32_t $a3_56 = arg4[3] - $a1_56;
            int32_t $lo_49;
            int32_t $hi_38;
            $hi_38 = HIGHD($a0_39 * $a3_56 + (arg5[3] - $a0_39) * ($s6 - $a1_56));
            $lo_49 = LOWD($a0_39 * $a3_56 + (arg5[3] - $a0_39) * ($s6 - $a1_56));
            var_120 = ($lo_22 << 2) * $a1_5 + ((($lo_49 + $a3_56 / 2) / $a3_56) << 9);
            $a0_42 = $a1_10;
        }
        else if ($a1_12 < $s6)
            $a0_42 = $a1_10;
        else
        {
            var_120 = ($a1_9 << 2) * $lo_22 + ((($s6 - $a1_7 + $a1_8 / 2) / $a1_8 + $a1_6) << 9);
            $a0_42 = $a1_10;
        }
    }
    else
    {
        int32_t $a3_43 = *arg4;
        int32_t $a1_50 = *arg5;
        int32_t $a0_28 = $a0_27 - $a3_43;
        int32_t $lo_34;
        int32_t $hi_28;
        $hi_28 = HIGHD($a1_50 * $a0_28 + (arg5[1] - $a1_50) * ($s6 - $a3_43));
        $lo_34 = LOWD($a1_50 * $a0_28 + (arg5[1] - $a1_50) * ($s6 - $a3_43));
        int32_t $lo_38;
        int32_t $hi_30;
        $hi_30 = HIGHD(COMBINE($a1_2 % 2, $a1_2 / 2) + ((($a1_1 << 3) - $s6) << 2) * $lo_22);
        $lo_38 = LOWD(COMBINE($a1_2 % 2, $a1_2 / 2) + ((($a1_1 << 3) - $s6) << 2) * $lo_22);
        $lo_40 = (((($lo_34 + $a0_28 / 2) / $a0_28) << 0xc) + $lo_38 / $a1_2 + 4) / 8;
    label_30870:
        var_120 = $lo_40;
        $a0_42 = $a1_10;
    }
    int32_t $a0_43 = $a0_42 << 7;
    int32_t $t4_31 = $lo_22 - $a0_43;
    
    if ($lo_22 < $a0_43)
        $t4_31 = $a0_43 - $lo_22;
    
    int32_t result;
    
    if ($v1_11 >= 2)
    {
        int32_t* $a1_77 = arg27;
        int32_t $a3_77 = result + 0x6c;
            int32_t $a0_93 = *$a1_77;
        result = arg28;
        
        do
        {
            
            if ($a0_93)
                *result = ($a0_93 / 2 + ($a3_25 << 0xe)) / $a0_93;
            else
                *result = isp_printf;
            
            result += 4;
            $a1_77 = &$a1_77[1];
        } while (result != $a3_77);
    }
    else
    {
        void* $a1_68 = &var_1d8;
        int32_t* i_19 = arg25 + 4;
        void* $a3_59 = &var_1d8;
        int32_t* i_7 = i_19;
            int32_t $t0_37 = *i_7;
            int32_t $t2_29 = *(i_7 - 4);
        int32_t i_8 = 0;
                int32_t $a3_63 = *(&var_1d8 + i_8);
                void* $t2_34 = i_20 + i_8;
            void* $t2_35 = &i_20[2];
            void* $t0_40 = &var_1d8;
            int32_t $a3_66 = 2;
        void var_1d8;
        
        do
        {
            i_7 = &i_7[1];
            *$a3_59 = $t0_37 - $t2_29;
            $a3_59 += 4;
        } while (i_7 != arg25 + 0x6c);
        
        int32_t $a1_69;
        
        if ($v1_11 == 1)
        {
            do
            {
                *(((void**)((char*)arg9 + i_8))) = $a3_63; // Fixed void pointer dereference
                *(((void**)((char*)arg12 + i_8))) = $a3_63; // Fixed void pointer dereference
                *(((void**)((char*)arg14 + i_8))) = $a3_63; // Fixed void pointer dereference
                i_8 += 4;
                *$t2_34 = $a3_63;
            } while ((uintptr_t)i_8 != 0x68);
            
            *arg14 = $v1_2;
            arg14[1] = $v1_2;
            *i_20 = $v1_3;
            i_20[1] = $v1_3;
            
            while (true)
            {
                $t0_40 += 4;
                
                if ($a3_66 >= $v1_8)
                    break;
                
                $a3_66 += 1;
                $t2_35 += 4;
                *($t2_35 - 4) = ($v1_4 * *($t0_40 + 4) + $v1_5 / 2) / $v1_5;
            }
            
            int32_t* i_9 = arg13;
            
            do
            {
                int32_t $lo_56 = ($v1_6 * *$a1_68 + $v1_7 / 2) / $v1_7;
                i_9 = &i_9[1];
                $a1_68 += 4;
                *(i_9 - 4) = $lo_56;
            } while (&i_9[0x1a] != i_9);
            
            $a1_69 = $a3_15;
        }
        else
            $a1_69 = $a3_15;
        
        int32_t $a0_66;
        
        if ($a1_69 != 1)
        {
                int32_t* i_10 = arg12;
                int32_t* $a1_72 = arg9;
                    int32_t $t0_42 = *i_10;
            $a0_66 = $a1_10;
            
            if ($a3_15 == 2)
            {
                
                do
                {
                    i_10 = &i_10[1];
                    *$a1_72 = $t0_42;
                    $a1_72 = &$a1_72[1];
                } while (i_10 != &i_10[0x1a]);
                
                $a0_66 = $a1_10;
            }
        }
        else
        {
            int32_t $t0_39 = $v0_3 - $v1_12;
            int32_t* i_11 = arg10;
            int32_t* $a3_61 = arg9;
            int32_t* $t2_30 = arg11;
                    int32_t $t3_12 = *$t2_30;
            
            do
            {
                int32_t $a0_61;
                
                if ($v1_12 >= $s6)
                    $a0_61 = *i_11;
                else if ($v0_3 < $s6)
                    $a0_61 = *$t2_30;
                else
                {
                    int32_t $lo_58;
                    int32_t $hi_46;
                    $hi_46 = HIGHD($t3_12 * $t0_39 + ($t3_12 - *i_11) * ($s6 - $v0_3));
                    $lo_58 = LOWD($t3_12 * $t0_39 + ($t3_12 - *i_11) * ($s6 - $v0_3));
                    $a0_61 = ($lo_58 + $t0_39 / 2) / $t0_39;
                }
                
                i_11 = &i_11[1];
                *$a3_61 = $a0_61;
                $t2_30 = &$t2_30[1];
                $a3_61 = &$a3_61[1];
            } while (i_11 != &i_11[0x1a]);
            
            $a0_66 = $a1_10;
        }
        
        int32_t $t2_37 = arg7[2];
        int32_t $lo_61;
        int32_t $hi_49;
        $hi_49 = HIGHD(COMBINE($a0_66 % 2, $a0_66 / 2) + ($v0_46 << 2) * $t4_31);
        $lo_61 = LOWD(COMBINE($a0_66 % 2, $a0_66 / 2) + ($v0_46 << 2) * $t4_31);
        int32_t $t4_32 = *arg7;
        int32_t $a1_73 = ($lo_61 / $a1_10 + var_120_2 + 0x40) / 0x80;
        
        if ($a1_73 < 0)
            $a1_73 = 0;
        
        int32_t $t0_43 = arg7[1] - $t4_32;
        int32_t $lo_66;
        int32_t $hi_52;
        $hi_52 = HIGHD($t2_37 * $t0_43
            + ((claHistG1_1 + $a0_1 + $a0_2 + $a0_3 + 0x400) / 0x800 - $t4_32)
            * (arg7[3] - $t2_37));
        $lo_66 = LOWD($t2_37 * $t0_43
            + ((claHistG1_1 + $a0_1 + $a0_2 + $a0_3 + 0x400) / 0x800 - $t4_32)
            * (arg7[3] - $t2_37));
        int32_t $a3_68 = $a3_27;
        
        if ($a1_73 < $a3_26)
            $a3_68 = $fp;
        
        int32_t $a0_75 = ($lo_66 + $t0_43 / 2) / $t0_43;
        int32_t var_134_1;
        
        if ($a3_29 >= $lo_22)
        {
            int32_t $a3_69 = $a2_1;
            
            if ($a3_28 < $lo_22)
                $a3_69 = $a2_1 + 2;
            
            var_134_1 = $a3_69;
        }
        else
            var_134_1_1 = $a2_1 + (($a2 < $lo_22 ? 1 : 0) < 1 ? 1 : 0);
        
        int32_t* $v0_91 = &arg27[1];
        int32_t $v0_92;
        
        if ($a3_23)
        {
            int32_t $t3_13 = $a1_14 + 1;
            int32_t $a3_71 = (1 - var_134_1) * $v1_1;
            int32_t $t0_47 = $a3_24;
            int32_t $t2_40 = 0;
            
            if ($a0_75 < $t0_47)
                $t0_47 = $a0_75;
            
            
            for (int32_t i_12 = 0; (uintptr_t)i_12 != 0x1a; )
            {
                int32_t $t0_51;
                
                if (i_12 < 2)
                {
                    $t0_51 = $a1_73 - (3 - i_12) * $t0_47;
                    
                    if ($(uintptr_t)t0_51 >= 0x4b1)
                        $t0_51 = 0x4b0;
                    
                    if ($t0_51 < 0)
                        $t0_51 = 0;
                }
                else
                {
                    int32_t $s0_3;
                    
                    if ($v1_9 >= i_12)
                    {
                        $s0_3 = $t3_13 * i_12 + $a1_73;
                        
                        if ($(uintptr_t)s0_3 >= 0x4b1)
                            $s0_3 = 0x4b0;
                        
                        $t0_51 = $s0_3;
                    }
                    else if ($v1_10 < i_12)
                    {
                        $t0_51 = ($a3_71 + 4 + $t2_40) * ($a3_71 - 1 + $t2_40) + $a1_73;
                        
                        if ($t0_51 >= $a3_68)
                            $t0_51 = $a3_68;
                    }
                    else
                    {
                        $s0_3 = $a1_15 - $a1_14 + $a1_73 + $t3_13;
                        
                        if ($a3_68 < $s0_3)
                            $s0_3 = $a3_68;
                        
                        $t0_51 = $s0_3;
                    }
                }
                
                *$v0_91 = $t0_51;
                i_12 += 1;
                $v0_91 = &$v0_91[1];
                $t3_13 += 1;
                $t2_40 += $v1_1;
            }
            
            $v0_92 = arg27[1];
        }
        else
        {
            int32_t $v1_74 = $a3_24;
            int32_t* $t0_44 = arg9;
                    int32_t $t7_11 = *($v0_91 - 4);
            
            if ($a0_75 < $v1_74)
                $v1_74 = $a0_75;
            
            for (int32_t i_13 = 0; (uintptr_t)i_13 != 0x1a; )
            {
                int32_t $v1_78;
                
                if (i_13 >= 2)
                {
                    
                    if (i_13 - 2 >= 2)
                    {
                        $v1_78 = $t7_11 + *$t0_44;
                        
                        if ($v1_78 >= $a3_68)
                            $v1_78 = $a3_68;
                    }
                    else
                    {
                        $v1_78 = $t7_11 + *$t0_44;
                        
                        if ($(uintptr_t)v1_78 >= 0x4b1)
                            $v1_78 = 0x4b0;
                    }
                }
                else
                {
                    $v1_78 = $a1_73 - (3 - i_13) * $v1_74;
                    
                    if ($(uintptr_t)v1_78 >= 0x4b1)
                        $v1_78 = 0x4b0;
                    
                    if ($v1_78 < 0)
                        $v1_78 = 0;
                }
                
                i_13 += 1;
                *$v0_91 = $v1_78;
                $t0_44 = &$t0_44[1];
                $v0_91 = &$v0_91[1];
            }
            
            $v0_92 = arg27[1];
        }
        
        *arg9 = $v0_92;
        void* i_14 = &arg27[2];
        void* $v1_83 = &arg9[1];
        
        do
        {
            int32_t $a3_73 = *i_14;
            int32_t $t2_41 = *(i_14 - 4);
            int32_t* $t2_42 = arg9;
            int32_t i_15 = 0;
            void* $v0_95 = &arg27[1];
            i_14 += 4;
            *$v1_83 = $a3_73 - $t2_41;
            $v1_83 += 4;
        } while (i_14 != &arg27[0x1b]);
        
        int32_t* $s0_6;
        
        if (!$a2_25)
        {
            int32_t (** $t0_60)(int32_t arg1, int32_t arg2, int32_t arg3) = &arg28[1];
            
            if ($a0_75 >= $a3_24)
                $a0_75 = $a3_24;
            
            do
            {
                int32_t $v1_92;
                
                if (i_15 >= 2)
                {
                    $v1_92 = *($v0_95 - 4) + *$t2_42;
                    
                    if ($v1_92 >= $a3_68)
                        $v1_92 = $a3_68;
                }
                else
                {
                    int32_t $a2_28 = $a1_73 - $a0_75 * (3 - i_15);
                    
                    if ($(uintptr_t)a2_28 >= 0x4b1)
                        $a2_28 = 0x4b0;
                    
                    $v1_92 = $a2_28;
                    
                    if ($a2_28 < 0)
                        $v1_92 = 0;
                }
                
                *$v0_95 = $v1_92;
                int32_t $a2_31 = *$v0_95;
                
                if ($a2_31)
                    *$t0_60 = ($a2_31 / 2 + ($a3_25 << 0xe)) / $a2_31;
                else
                    *$t0_60 = isp_printf;
                
                i_15 += 1;
                $t0_60 = &$t0_60[1];
                $v0_95 += 4;
                $t2_42 = &$t2_42[1];
            } while ((uintptr_t)i_15 != 0x1a);
            
            $s0_6 = arg26;
        }
        else if ($a2_25 != 1)
        {
            int32_t* $t2_44 = arg13;
            void* $t0_63 = &arg28[1];
            int32_t i_16 = 0;
            void* $v0_109 = &arg27[1];
            
            if ($a0_75 >= $a3_24)
                $a0_75 = $a3_24;
            
            do
            {
                int32_t $v1_107;
                
                if (i_16 >= 2)
                {
                    $v1_107 = *($v0_109 - 4) + *$t2_44;
                    
                    if ($v1_107 >= $a3_68)
                        $v1_107 = $a3_68;
                }
                else
                {
                    int32_t $a2_40 = $a1_73 - $a0_75 * (3 - i_16);
                    
                    if ($(uintptr_t)a2_40 >= 0x4b1)
                        $a2_40 = 0x4b0;
                    
                    $v1_107 = $a2_40;
                    
                    if ($a2_40 < 0)
                        $v1_107 = 0;
                }
                
                *$v0_109 = $v1_107;
                int32_t $a2_43 = *$v0_109;
                
                if ($a2_43)
                    *$t0_63 = ($a2_43 / 2 + ($a3_25 << 0xe)) / $a2_43;
                else
                    *$t0_63 = isp_printf;
                
                i_16 += 1;
                $t0_63 += 4;
                $v0_109 += 4;
                $t2_44 = &$t2_44[1];
            } while ((uintptr_t)i_16 != 0x1a);
            
            $s0_6 = arg26;
        }
        else
        {
            int32_t $v0_99 = $a3_25;
                int32_t* i_17 = i_20;
                int32_t $a2_32 = $a1 - $a3_19;
                int32_t* $t0_61 = arg14;
                int32_t* $a3_75 = arg12;
                    int32_t $v0_102 = *$t0_61;
            
            if (var_124 < $a1)
            {
                
                do
                {
                    int32_t $lo_73;
                    int32_t $hi_59;
                    $hi_59 = HIGHD($v0_102 * $a2_32 + (*i_17 - $v0_102) * (var_124 - $a3_19));
                    $lo_73 = LOWD($v0_102 * $a2_32 + (*i_17 - $v0_102) * (var_124 - $a3_19));
                    i_17 = &i_17[1];
                    $t0_61 = &$t0_61[1];
                    $a3_75 = &$a3_75[1];
                    *($a3_75 - 4) = ($lo_73 + $a2_32 / 2) / $a2_32;
                } while (i_17 != &i_17[0x1a]);
                
                i_20 = arg12;
                $v0_99 = $a3_25;
            }
            
            int32_t* i_21 = i_20;
            void* $v1_101 = &arg27[1];
            void* $t0_62 = &arg28[1];
            
            for (int32_t i_18 = 0; (uintptr_t)i_18 != 0x1a; )
            {
                int32_t $a0_76 = i_18 < 2 ? 1 : 0;
                i_18 += 1;
                int32_t $a0_78;
                
                if (!$a0_76)
                {
                    $a0_78 = *($v1_101 - 4) + *i_21;
                    
                    if ($a0_78 >= $a3_68)
                        $a0_78 = $a3_68;
                }
                else
                {
                    $a0_78 = i_18 * *i_21 + $a1_73;
                    
                    if ($(uintptr_t)a0_78 >= 0x4b1)
                        $a0_78 = 0x4b0;
                }
                
                *$v1_101 = $a0_78;
                int32_t $a2_37 = *$v1_101;
                
                if ($a2_37)
                    *$t0_62 = ($a2_37 / 2 + ($v0_99 << 0xe)) / $a2_37;
                else
                    *$t0_62 = isp_printf;
                
                $t0_62 += 4;
                $v1_101 += 4;
                i_21 = &i_21[1];
            }
            
            $s0_6 = arg26;
        }
        
        do
        {
            int32_t $a0_84 = *i_19 - *(i_19 - 4);
        int32_t $a0_85 = arg27[1];
        int32_t $v0_114 = arg28[2] - arg28[1];
        int32_t $a3_76 = *(arg25 + 4);
        int32_t $a2_45 = *(arg25 + 8) - $a3_76;
        int32_t $a0_89 = ($lo_80 + $a2_45 / 2) / $a2_45;
        int32_t $v1_118 = *(arg25 + 4);
        int32_t $a1_76 = *(arg25 + 8) - $v1_118;
            i_19 = &i_19[1];
            *$s0_6 = Log2($a0_84);
            $s0_6 = &$s0_6[1];
        } while (arg25 + 0x6c != i_19);
        
        int32_t $lo_79;
        int32_t $hi_65;
        $hi_65 = HIGHD($a0_85 * $a2_45);
        $lo_79 = LOWD($a0_85 * $a2_45);
        int32_t $lo_80;
        int32_t $hi_66;
        $hi_66 = HIGHD(($hi_65 << 0x20 | $lo_79) - (arg27[2] - $a0_85) * $a3_76);
        $lo_80 = LOWD(($hi_65 << 0x20 | $lo_79) - (arg27[2] - $a0_85) * $a3_76);
        
        if ($a0_89 < 0)
            $a0_89 = 0;
        
        *arg27 = $a0_89;
        int32_t $lo_83;
        int32_t $hi_68;
        $hi_68 = HIGHD($a1_76 * arg28[1]);
        $lo_83 = LOWD($a1_76 * arg28[1]);
        int32_t $lo_84;
        int32_t $hi_69;
        $hi_69 = HIGHD(($hi_68 << 0x20 | $lo_83) - $v0_114 * $v1_118);
        $lo_84 = LOWD(($hi_68 << 0x20 | $lo_83) - $v0_114 * $v1_118);
        result = ($lo_84 + $a1_76 / 2) / $a1_76;
        
        if ((uintptr_t)result >= 0x10001)
            result = isp_printf;
        
        *arg28 = result;
    }
    
    return result;
}

