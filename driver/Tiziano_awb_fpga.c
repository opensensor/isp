#include "include/main.h"


  uint32_t Tiziano_awb_fpga(void* arg1, void* arg2, void* arg3, void* arg4, int32_t* arg5, int32_t* arg6, void* arg7, int32_t arg8, int32_t* arg9, int32_t* arg10, void* arg11, int32_t* arg12, int32_t arg13)

{
    void* arg_4 = arg2;
    void* arg_0 = arg1;
    int32_t $s2 = *arg10;
    int32_t j = *(arg7 + 0xc);
    void* arg_c = arg4;
    void* arg_8 = arg3;
    int32_t $s5 = *(arg7 + 4);
    int32_t $a0_1 = *arg6;
    int32_t $s1 = *arg5;
    int32_t $v1_1 = arg5[1];
    int32_t $v1_2 = *(arg11 + 0x20);
    int32_t $v0_1 = *(arg11 + 0x24);
    int32_t $a0_2 = arg6[1];
    int32_t $v0_2 = arg6[2];
    int32_t $v0_3 = arg6[3];
    int32_t $s3 = 0x100 << ($s2 & 0x1f);
    int32_t $v0_4 = arg6[4];
    int32_t $v0_5 = arg6[5];
    int32_t $v0_7 = fix_point_mult2_32($s2, $s3, *arg9);
    int32_t $v0_9 = fix_point_mult2_32($s2, $s3, arg9[1]);
    int32_t $s3_1 = j << 2;
    int32_t $t8 = j * $s5;
    void* $t2 = nullptr;
    int32_t $t9 = $t8 << 2;
    int32_t $s4 = 0;
    int32_t $v1_3 = 0;
    void* $t1 = &zone_rgbg;
        void* $s6_1 = $t1 + $s4;
        int32_t* $t6_1 = arg1 + $s4;
        void* $t0_2 = &zone_pix_cnt + $s4;
        int32_t* $fp_1 = arg3 + $s4;
        int32_t $t5_1 = $s4 + $t9;
        int32_t* $s7_1 = arg2 + $s4;
        int32_t $t3_1 = 0;
        int32_t* $t4_1 = arg4 + $s4;
            int32_t $a0_5 = *$s7_1;
    int32_t (* var_74)(int32_t arg1, int32_t arg2, int32_t arg3) = fix_point_mult2_32;
    
    while ($v1_3 != $s5)
    {
        
        while ($t3_1 != j)
        {
            
            if ($a0_5)
            {
                *$s6_1 = var_74($s2, (*$t6_1 << ($s2 & 0x1f)) / $a0_5, $v0_7);
                *(((void**)((char*)$t2 + $s6_1 + $t5_1))) = var_74($s2, (*$fp_1 << ($s2 & 0x1f)) / *$s7_1, $v0_9); // Fixed void pointer dereference
                *$t0_2 = *$t4_1;
            }
            else
            {
                *$s6_1 = 0;
                *$t0_2 = 0;
                *(((int32_t*)((char*)$t2 + $s6_1 + $t5_1))) = 0; // Fixed void pointer dereference
            }
            
            $t3_1 += 1;
            $s6_1 += 4;
            $t0_2 += 4;
            $t6_1 = &$t6_1[1];
            $fp_1 = &$fp_1[1];
            $s7_1 = &$s7_1[1];
            $t4_1 = &$t4_1[1];
        }
        
        $v1_3 += 1;
        $s4 += $s3_1;
        $t2 += -(j) << 2;
    }
    
    awb_rg_global = 0;
    awb_bg_global = 0;
    *awb_pix_cnt = $t8;
    *(((void**)((char*)awb_pix_cnt + 4))) = $t8; // Fixed void pointer dereference
    void* $fp_2 = $t1;
    void* $v1_4 = $t1;
    int32_t $a0_10 = $t8;
    int32_t $a1_10 = $t8;
    int32_t $t3_2 = 0;
    int32_t $t4_2 = 0;
    int32_t $t5_2 = 0;
    uint32_t $a2_4 = 0;
    int32_t $t6_2 = 0;
    uint32_t $a3_1 = 0;
    int32_t $t2_1 = 0;
    
    while ($t2_1 != $s5)
    {
        int32_t $v0_29 = 0;
            int32_t $t6_3 = *($v1_4 + $v0_29);
            int32_t $t5_5 = *($v1_4 + $t9 + $v0_29);
        
        while ($v0_29 != $s3_1)
        {
            $a3_1 += $t6_3;
            $a2_4 += $t5_5;
            
            if (!$t6_3)
            {
                $a1_10 -= 1;
                $t4_2 = 1;
            }
            
            $v0_29 += 4;
            
            if (!$t5_5)
            {
                $a0_10 -= 1;
                $t3_2 = 1;
            }
            
            $t6_2 = 1;
            $t5_2 = 1;
        }
        
        $t2_1 += 1;
        $v1_4 += $s3_1;
    }
    
    if ($t6_2)
        awb_rg_global = $a3_1;
    
    if ($t5_2)
        awb_bg_global = $a2_4;
    
    if ($t4_2)
        *awb_pix_cnt = $a1_10;
    
    if ($t3_2)
        *(((void**)((char*)awb_pix_cnt + 4))) = $a0_10; // Fixed void pointer dereference
    
    uint32_t awb_rg_global_1 = awb_rg_global;
    int32_t var_f8_3 = *arg9;
    uint64_t* var_50_2 = &awb_pix_cnt;
    int32_t (* var_60_1_1)(int32_t arg1, int32_t arg2, int32_t arg3, int32_t arg4, int32_t arg5, 
        int32_t arg6) = fix_point_div;
    int32_t $v0_32;
    int32_t $v1_5;
    $v0_32 = fix_point_div($s2, $a1_10, awb_rg_global_1, 0, var_f8_4, 0);
    void* const var_64_1_2 = __lshrdi3;
    awb_rg_global = __lshrdi3($v0_32, $v1_5, $s2);
    uint32_t awb_bg_global_1 = awb_bg_global;
    var_f8_5 = arg9[1];
    int32_t var_f4_1 = 0;
    int32_t $v0_37;
    int32_t $v1_6;
    $v0_37 = var_60_1_2($s2);
    uint32_t $v0_38 = var_64_1_3($v0_37, $v1_6, $s2);
    int32_t awb_pix_cnt_1 = *awb_pix_cnt;
    awb_bg_global = $v0_38;
    uint32_t $a0_15 = 0x100;
    
    if (awb_pix_cnt_1)
        $a0_15 = awb_rg_global / awb_pix_cnt_1;
    
    data_b5a40_1 = $a0_15;
    int32_t $a0_17 = *(var_50_2_1 + 4);
    uint32_t $v0_39;
    
    $v0_39 = $a0_17 ? $v0_38 / $a0_17 : 0x100;
    
    data_b5a44_1 = $v0_39;
    int32_t $a2_8 = 0;
    int32_t i = 0;
    
    while (i != $s5 * 0xf)
    {
        void* $v0_40 = &tisp_wb_zone_attr + i;
        void* $a0_18 = arg4 + $a2_8;
        int32_t $t2_3 = 0;
        void* $v1_8 = $v0_40;
        void* $t5_6 = $v0_40;
            int32_t $t6_4 = *$a0_18;
                int32_t $a1_15 = $a2_8 + $t2_3;
        
        while ($v0_40 - $t5_6 < j)
        {
            
            if ($t6_4)
            {
                *$v1_8 = *(arg1 + $a1_15) / $t6_4;
                *(((void**)((char*)$v1_8 + 0xe1))) = *(arg2 + $a1_15) / *$a0_18; // Fixed void pointer dereference
                *(((void**)((char*)$v1_8 + 0x1c2))) = *(arg3 + $a1_15) / *$a0_18; // Fixed void pointer dereference
            }
            else
            {
                *$v0_40 = 0;
                *(((int32_t*)((char*)$v0_40 + 0xe1))) = 0; // Fixed void pointer dereference
                *(((int32_t*)((char*)$v0_40 + 0x1c2))) = 0; // Fixed void pointer dereference
            }
            
            $v0_40 += 1;
            $t2_3 += 4;
            $v1_8 += 1;
            $a0_18 += 4;
        }
        
        i += 0xf;
        $a2_8 += $s3_1;
    }
    
    int32_t $a0_19 = 0;
    int32_t $a2_9 = 0;
    
    while ($a2_9 != $s5)
    {
        int32_t $v1_9 = 0;
        int32_t $a1_19 = 0;
            int32_t $v0_42 = $a0_19 + $v1_9;
        
        while ($a1_19 != j)
        {
            $a1_19 += 1;
            
            if (arg8 >= *(arg4 + $v0_42))
            {
                *(((int32_t*)((char*)$t1 + $v0_42))) = 0; // Fixed void pointer dereference
                *(((int32_t*)((char*)$t1 + $v1_9 + $a0_19 + $t9))) = 0; // Fixed void pointer dereference
                *(((int32_t*)((char*)&zone_pix_cnt + $v0_42))) = 0; // Fixed void pointer dereference
            }
            
            $v1_9 += 4;
        }
        
        $a2_9 += 1;
        $a0_19 += $s3_1;
    }
    
    int32_t $v0_44 = data_98468_1;
    void* $v1_10 = &zone_rgbg_last;
    uint32_t awb_moa_1;
    int32_t $a0_21;
    
    if ($v0_44)
    {
    label_283a8:
        $a0_21 = 0;
    label_28448:
        
        while ($a0_21 != $s5)
        {
            for (int32_t i_1 = 0; $s3_1 != i_1; i_1 += 4)
            {
                *(((void**)((char*)$v1_10 + i_1))) = *($fp_2 + i_1); // Fixed void pointer dereference
                *(((void**)((char*)$v1_10 + $t9 + i_1))) = *($fp_2 + $t9 + i_1); // Fixed void pointer dereference
            }
            
            $a0_21 += 1;
            $fp_2 += $s3_1;
            $v1_10 += $s3_1;
        }
        
        int32_t var_b0_16;
        int32_t* var_bc_1 = &var_b0_17;
        uint32_t var_b8_5;
        int32_t* var_c0_1 = &var_b8_6;
        
        for (int32_t i_2 = 0; (uintptr_t)i_2 < 0x38; i_2 += 1)
        {
            void arg_34;
            *(((void**)((char*)&var_f8 + i_2))) = *(&arg_34 + i_2); // Fixed void pointer dereference
        }
        
        Tiziano_Awb_Ct_Detect(&zone_rgbg, &zone_pix_cnt, arg12, arg13);
        uint32_t var_b4;
        
        if (data_98468_2)
        {
            uint32_t $t2_7 = var_b8;
            uint32_t _awb_ct_1 = _awb_ct;
                uint32_t* $a0_26 = &data_983b4 + i_3;
            
            for (int32_t i_3 = 0; (uintptr_t)i_3 != 0x3c; )
            {
                *(((void**)((char*)&data_9842c + i_3))) = $t2_7; // Fixed void pointer dereference
                *(((void**)((char*)&data_983f0 + i_3))) = var_b4; // Fixed void pointer dereference
                i_3 += 4;
                *$a0_26 = _awb_ct_1;
            }
            
            data_98468_3 = 0;
            $s1 = 1;
            goto label_286c4;
        }
        
        void* i_4 = &data_9842c;
        void* $a0_22 = &data_983f0;
        void* $v1_13 = &data_983b4;
        
        do
        {
            int32_t $t1_21 = *(i_4 + 4);
        int32_t i_5 = 0xf - $s1;
        int32_t $t9_2 = i_5 << 2;
        int32_t $t6_8 = 0;
        int32_t $t0_10 = 0;
        int32_t $t3_7 = 0;
        int32_t $v1_14 = 0;
        int32_t $t4_6 = 0;
        int32_t $a0_23 = 0;
        int32_t $t5_8 = 0;
            int32_t $t1_24 = $t9_2 + $t6_8;
        uint32_t $lo_9 = (($a0_23 >> 1) + $t5_8) / $a0_23;
        uint32_t $lo_10 = (($v1_14 >> 1) + $t4_6) / $v1_14;
        int32_t awb_gain_original_1 = *awb_gain_original;
            int32_t $a0_28 = 1 << (($s2 + 0x10) & 0x1f);
            int32_t $a1_32 = 1 << (($s2 - 1) & 0x1f);
            uint32_t $v1_20 = ($a0_28 / $v1_15 + $a1_32) >> ($s2 & 0x1f);
            uint32_t $v0_65 = ($a0_28 / $v0_60 + $a1_32) >> ($s2 & 0x1f);
            int32_t $a1_34 = $v0_2 - $v1_20;
            int32_t $a0_32 = $v0_65 - $v0_3;
            int32_t $a0_33 = $a1_34 + $a0_32;
            int32_t $a1_35 = $v0_1;
            int32_t $a2_16 = var_b0;
            i_4 += 4;
            *(i_4 - 4) = $t1_21;
            $v1_13 += 4;
            *$a0_22 = *($a0_22 + 4);
            $a0_22 += 4;
            *($v1_13 - 4) = *$v1_13;
        } while (i_4 != &data_98464);
        
        data_98464 = var_b8;
        data_98428 = var_b4;
        data_983ec = _awb_ct;
        
        if ($(uintptr_t)s1 >= 0x10)
            $s1 = 0xf;
        else if (!$s1)
            $s1 = 1;
        
        
        do
        {
            i_5 += 1;
            $t5_8 += i_5 * *(&data_9842c + $t1_24);
            $a0_23 += i_5;
            $t4_6 += i_5 * *(&data_983f0 + $t1_24);
            $v1_14 += i_5;
            $t3_7 += i_5 * *(&data_983b4 + $t1_24);
            $t0_10 += i_5;
            $t6_8 += 4;
        } while ((uintptr_t)i_5 != 0xf);
        
        var_b8 = $lo_9;
        var_b4 = $lo_10;
        _awb_ct = (($t0_10 >> 1) + $t3_7) / $t0_10;
        uint32_t $v0_60;
        uint32_t $v1_15;
        
        if ($lo_9 != awb_gain_original_1)
        {
        label_286c4:
            $v1_15 = var_b8;
        label_286c8:
            $v0_60 = var_b4;
        label_286d0:
            *awb_gain_original = $v1_15;
            *(((void**)((char*)awb_gain_original + 4))) = $v0_60; // Fixed void pointer dereference
            
            if ($v0_2 < $v1_20)
                $a1_34 = $v1_20 - $v0_2;
            
            
            if ($v0_3 >= $v0_65)
                $a0_32 = $v0_3 - $v0_65;
            
            
            if (!$v1_2)
                $a1_35 = $v1_1;
            
            
            if ($a2_16 == 1)
            {
                arg6[2] = 0x100;
                arg6[3] = 0x100;
                arg6[4] = 0x100;
                arg6[5] = 0x100;
            }
            
            if ($a0_1)
            {
                int32_t $a0_38 = $a0_2;
                
                if ($v1_1 >= $a0_33)
                    goto label_28810;
                
                if ($a2_16 != 1)
                {
                    *arg6 = 1;
                    arg6[1] = 1;
                    arg6[2] = $v1_20;
                    arg6[3] = $v0_65;
                    arg6[4] = ($v1_20 - $v0_4) / $s1 + $v0_4;
                    arg6[5] = ($v0_65 - $v0_5) / $s1 + $v0_5;
                }
                else
                {
                        int32_t $v0_68 = arg6[2];
                    $a0_38 = $a0_2;
                label_28810:
                    
                    if ($s1 == $a0_38 + 1 || $s1 == 1)
                    {
                        *arg6 = 0;
                        arg6[4] = $v0_68;
                        arg6[5] = arg6[3];
                    }
                    else
                    {
                        int32_t $s1_1 = $s1 - $a0_2;
                        *arg6 = 1;
                        arg6[4] = ($v1_20 - $v0_4) / $s1_1 + $v0_4;
                        arg6[5] = ($v0_65 - $v0_5) / $s1_1 + $v0_5;
                    }
                    
                    arg6[1] += 1;
                }
            }
            else if ($a1_35 >= $a0_33)
            {
                *arg6 = 0;
                arg6[1] = 0;
            }
            else if ($a2_16 != 1)
            {
                *arg6 = 1;
                arg6[1] = 1;
                arg6[2] = $v1_20;
                arg6[3] = $v0_65;
                arg6[4] = ($v1_20 - $v0_2) / $s1 + $v0_4;
                arg6[5] = ($v0_65 - $v0_3) / $s1 + $v0_5;
            }
            else
            {
                *arg6 = 0;
                arg6[1] = 0;
            }
            
            return Tiziano_awb_set_gain(arg6, $s2, arg9);
        }
        
        $v1_15 = var_b8_7;
        
        if ($lo_10 != *(awb_gain_original + 4))
            goto label_286c8;
        
        $v0_60 = var_b4_1;
        
        if (tisp_wb_attr)
            goto label_286d0;
        
        $v0_60 = var_b4_2;
        
        if (awb_moa)
            goto label_286d0;
        
        $v1_15 = var_b8_8;
        
        if (arg6[2] != arg6[4])
            goto label_286c8;
        
        awb_moa_1 = arg6[5];
        
        if (arg6[3] != awb_moa_1)
            goto label_286c4;
    }
    else
    {
        void* $a2_10 = &zone_rgbg_last;
        void* $a3_2 = $fp_2;
        int32_t $t0_4 = 0;
        int32_t $t1_20 = 0;
            int32_t $a0_20 = 0;
                int32_t $t3_5 = *($a3_2 + $a0_20);
                int32_t $a1_22 = *($a2_10 + $a0_20);
                int32_t $a1_23 = $a1_22 - $t3_5;
                int32_t $t3_6 = *($a3_2 + $t9 + $a0_20);
                int32_t $a1_26 = *($a2_10 + $t9 + $a0_20);
                int32_t $a1_27 = $a1_26 - $t3_6;
        
        while ($t1_20 != $s5)
        {
            
            while ($s3_1 != $a0_20)
            {
                
                if ($a1_22 < $t3_5)
                    $a1_23 = $t3_5 - $a1_22;
                
                $t0_4 += $a1_23;
                $a0_20 += 4;
                
                if ($a1_26 < $t3_6)
                    $a1_27 = $t3_6 - $a1_26;
                
                $v0_44 += $a1_27;
            }
            
            $t1_20 += 1;
            $a3_2 += $s3_1;
            $a2_10 += $s3_1;
        }
        
        if ((($v0_44 + $t0_4) >> ($s2 & 0x1f)) / $t8 >= *(_AwbPointPos + 4))
            goto label_283a8;
        
        $a0_21 = 0;
        
        if (tisp_wb_attr)
            goto label_28448;
        
        awb_moa_1 = awb_moa;
        
        if (awb_moa_1)
            goto label_28448;
    }
    return awb_moa_1;
}

