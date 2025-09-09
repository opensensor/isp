#include "include/main.h"


  void* tiziano_bcsh_Tccm_RGBYUV(int32_t* arg1, int32_t* arg2, int32_t* arg3, int32_t* arg4, int32_t arg5 @ $hi)

{
    void* $s5 = &var_a0;
    void* i = &var_a0;
        int32_t $a0 = *arg2;
    void var_a0;
    int32_t var_58;
    
    do
    {
        
        if ($a0 < 0)
        {
            *i = 0xffffffff;
            $a0 = -($a0);
        }
        else
            *i = 1;
        
        *(((void**)((char*)i + 4))) = $a0; // Fixed void pointer dereference
        i += 8;
        arg2 = &arg2[1];
    } while (&var_58_5 != i);
    
    int32_t var_e8;
    int32_t* i_1 = &var_e8_1;
    
    do
    {
        int32_t $a0_1 = *arg3;
        
        if ($a0_1 < 0)
        {
            *i_1 = 0xffffffff;
            $a0_1 = -($a0_1);
        }
        else
            *i_1 = 1;
        
        i_1[1] = $a0_1;
        i_1 = &i_1[2];
        arg3 = &arg3[1];
    } while (&var_a0_6 != i_1);
    
    int32_t var_130;
    int32_t* i_2 = &var_130_1;
    
    do
    {
        int32_t $a0_2 = *arg4;
        
        if ($a0_2 < 0)
        {
            *i_2 = 0xffffffff;
            $a0_2 = -($a0_2);
        }
        else
            *i_2 = 1;
        
        i_2[1] = $a0_2;
        i_2 = &i_2[2];
        arg4 = &arg4[1];
    } while (&var_e8_2 != i_2);
    
    int32_t* $s3 = arg1;
    int32_t* i_3 = arg1;
    
    do
    {
        int32_t $t1_1 = *$s5;
        int32_t $t5_1 = *($s5 + 4);
        int32_t $s6_1 = $t1_1 * var_e8 * fix_point_mult2_32(0x10, $t5_1, var_e4 << 6);
        int32_t $fp_1 = *($s5 + 8);
        int32_t $s4_1 = *($s5 + 0xc);
        int32_t $v1_1 = $fp_1 * var_d0 * fix_point_mult2_32(0x10, $s4_1, var_cc << 6);
        int32_t $s0_5 = *($s5 + 0x10);
        int32_t $s6_2 = *($s5 + 0x14);
        int32_t $t6_6 = $t1_1 * var_e0 * fix_point_mult2_32(0x10, $t5_1, var_dc << 6);
        int32_t $t6_8 = $fp_1 * var_c8 * fix_point_mult2_32(0x10, $s4_1, var_c4 << 6) + $t6_6;
        int32_t j = 0;
        int32_t* $v0_16 = &var_58;
            int32_t $a0_4 = *(i_3 + j);
        int32_t var_e4;
        int32_t var_d0;
        int32_t var_cc;
        int32_t var_b8;
        int32_t var_b4;
        *i_3 = $s0_5 * var_b8 * fix_point_mult2_32(0x10, $s6_2, var_b4 << 6) + $v1_1 + $s6_1;
        int32_t var_e0;
        int32_t var_dc;
        int32_t var_c8;
        int32_t var_c4;
        int32_t var_b0;
        int32_t var_ac;
        i_3[1] = $s0_5 * var_b0 * fix_point_mult2_32(0x10, $s6_2, var_ac << 6) + $t6_8;
        int32_t var_d8;
        int32_t var_d4;
        int32_t var_c0;
        int32_t var_bc;
        int32_t $lo_2;
        int32_t $hi;
        $hi = HIGHD(COMBINE(arg5, $t1_1 * var_d8 * fix_point_mult2_32(0x10, $t5_1, var_d4 << 6))
            + $fp_1 * var_c0 * fix_point_mult2_32(0x10, $s4_1, var_bc << 6));
        $lo_2 = LOWD(COMBINE(arg5, $t1_1 * var_d8 * fix_point_mult2_32(0x10, $t5_1, var_d4 << 6))
            + $fp_1 * var_c0 * fix_point_mult2_32(0x10, $s4_1, var_bc << 6));
        int32_t var_a8;
        int32_t var_a4;
        i_3[2] = $s0_5 * var_a8 * fix_point_mult2_32(0x10, $s6_2, var_a4 << 6) + $lo_2;
        
        do
        {
            
            if ($a0_4 < 0)
            {
                *$v0_16 = 0xffffffff;
                $a0_4 = -($a0_4);
            }
            else
                *$v0_16 = 1;
            
            j += 4;
            $v0_16[1] = $a0_4;
            $v0_16 = &$v0_16[2];
        } while ((uintptr_t)j != 0xc);
        
        int32_t $t2_7 = var_58_6;
        int32_t var_12c;
        int32_t var_54;
        int32_t $s6_3 = $t2_7 * var_130_2 * fix_point_mult2_32(0x10, var_54_2, var_12c_1);
        int32_t var_118_3;
        int32_t var_114_4;
        int32_t var_50_3;
        int32_t var_4c_1;
        int32_t $t7_5 = var_50_4 * var_118_4 * fix_point_mult2_32(0x10, var_4c_2, var_114_5) + $s6_3;
        int32_t var_100;
        int32_t var_fc;
        int32_t var_48_15;
        int32_t var_44_7;
        *i_3 = var_48_16 * var_100_1 * fix_point_mult2_32(0x10, var_44_8, var_fc_2) + $t7_5;
        int32_t var_128_2;
        int32_t var_124_6;
        int32_t $t7_10 = $t2_7 * var_128_3 * fix_point_mult2_32(0x10, var_54_3, var_124_7);
        int32_t var_110;
        int32_t var_10c;
        int32_t $t7_12 = var_50_5 * var_110_2 * fix_point_mult2_32(0x10, var_4c_3, var_10c_1) + $t7_10;
        int32_t var_f8_6;
        int32_t var_f4;
        i_3[1] = var_48_17 * var_f8_7 * fix_point_mult2_32(0x10, var_44_9, var_f4_2) + $t7_12;
        int32_t var_120_3;
        int32_t var_11c_5;
        int32_t var_108;
        int32_t var_104;
        int32_t $lo_4;
        arg5 = HIGHD(COMBINE($hi, $t2_7 * var_120_4 * fix_point_mult2_32(0x10, var_54_4, var_11c_6))
            + var_50_6 * var_108_1 * fix_point_mult2_32(0x10, var_4c_4, var_104_1));
        $lo_4 = LOWD(COMBINE($hi, $t2_7 * var_120_5 * fix_point_mult2_32(0x10, var_54_5, var_11c_7))
            + var_50_7 * var_108_2 * fix_point_mult2_32(0x10, var_4c_5, var_104_2));
        i_3 = &i_3[3];
        int32_t var_f0;
        int32_t var_ec;
        *(i_3 - 4) = var_48_18 * var_f0_1 * fix_point_mult2_32(0x10, var_44_10, var_ec_2) + $lo_4;
        $s5 += 0x18;
    } while (&arg1[9] != i_3);
    
    uint32_t bcsh_hue_1 = bcsh_hue;
    int32_t $s1;
    int32_t $s2;
    int32_t $s6_5;
    
    if ((uintptr_t)bcsh_hue_1 == 0x3c)
    {
        $s1 = data_aa710;
        $s2 = data_aa704;
        $s6_5 = 1;
    }
    else if ((uintptr_t)bcsh_hue_1 < 0x3d)
    {
        int32_t CosValue_1 = CosValue;
        int32_t $a0_17 = data_aa710;
        int32_t HueIndex_2 = HueIndex;
            int32_t $a1_30 = HueIndex_2 - bcsh_hue_1;
            int32_t $a2_37 = data_aa71c;
            int32_t $s1_4 = HueIndex_2 - $a2_37;
        
        if ($a0_17 >= CosValue_1)
        {
            
            if (HueIndex_2 < bcsh_hue_1)
                $a1_30 = bcsh_hue_1 - HueIndex_2;
            
            
            if ($a2_37 >= HueIndex_2)
                $s1_4 = $a2_37 - HueIndex_2;
            
            $s1 = $a1_30 * ($a0_17 - CosValue_1) / $s1_4 + CosValue_1;
        }
        else
        {
            int32_t $a1_28 = HueIndex_2 - bcsh_hue_1;
            int32_t $a2_35 = data_aa71c;
            int32_t $s1_3 = HueIndex_2 - $a2_35;
            
            if (HueIndex_2 < bcsh_hue_1)
                $a1_28 = bcsh_hue_1 - HueIndex_2;
            
            
            if ($a2_35 >= HueIndex_2)
                $s1_3 = $a2_35 - HueIndex_2;
            
            $s1 = CosValue_1 - $a1_28 * (CosValue_1 - $a0_17) / $s1_3;
        }
        
        int32_t SinValue_1 = SinValue;
        int32_t $a0_24 = data_aa704_1;
        int32_t HueIndex_1 = HueIndex;
        
        if ($a0_24 >= SinValue_1)
        {
            int32_t $v0_42 = HueIndex_1 - bcsh_hue_1;
            int32_t $a0_28 = data_aa71c;
            int32_t $s2_5 = HueIndex_1 - $a0_28;
            
            if (HueIndex_1 < bcsh_hue_1)
                $v0_42 = bcsh_hue_1 - HueIndex_1;
            
            
            if ($a0_28 >= HueIndex_1)
                $s2_5 = $a0_28 - HueIndex_1;
            
            $s2 = $v0_42 * ($a0_24 - SinValue_1) / $s2_5 + SinValue_1;
        }
        else
        {
            int32_t $v0_39 = HueIndex_1 - bcsh_hue_1;
            int32_t $a0_26 = data_aa71c;
            int32_t $s2_3 = HueIndex_1 - $a0_26;
            
            if (HueIndex_1 < bcsh_hue_1)
                $v0_39 = bcsh_hue_1 - HueIndex_1;
            
            
            if ($a0_26 >= HueIndex_1)
                $s2_3 = $a0_26 - HueIndex_1;
            
            $s2 = SinValue_1 - $v0_39 * (SinValue_1 - $a0_24) / $s2_3;
        }
        
        $s6_5 = 0xffffffff;
    }
    else
    {
        int32_t $t3_7 = data_aa710;
        int32_t $a0_5 = data_aa714;
        int32_t $t2_15 = data_aa71c;
            int32_t $a1_22 = $t2_15 - bcsh_hue_1;
            int32_t $a2_30 = data_aa720;
            int32_t $s1_2 = $t2_15 - $a2_30;
        
        if ($a0_5 >= $t3_7)
        {
            
            if ($t2_15 < bcsh_hue_1)
                $a1_22 = bcsh_hue_1 - $t2_15;
            
            
            if ($a2_30 >= $t2_15)
                $s1_2 = $a2_30 - $t2_15;
            
            $s1 = $a1_22 * ($a0_5 - $t3_7) / $s1_2 + $t3_7;
        }
        else
        {
            int32_t $a1_20 = $t2_15 - bcsh_hue_1;
            int32_t $a2_28 = data_aa720;
            int32_t $s1_1 = $t2_15 - $a2_28;
            
            if ($t2_15 < bcsh_hue_1)
                $a1_20 = bcsh_hue_1 - $t2_15;
            
            
            if ($a2_28 >= $t2_15)
                $s1_1 = $a2_28 - $t2_15;
            
            $s1 = $t3_7 - $a1_20 * ($t3_7 - $a0_5) / $s1_1;
        }
        
        int32_t $t1_14 = data_aa704_2;
        int32_t $a0_12 = data_aa708_1;
        int32_t $a2_31 = data_aa71c_1;
        
        if ($a0_12 >= $t1_14)
        {
            int32_t $v0_36 = $a2_31 - bcsh_hue_1;
            int32_t $a0_16 = data_aa720;
            int32_t $s2_2 = $a2_31 - $a0_16;
            
            if ($a2_31 < bcsh_hue_1)
                $v0_36 = bcsh_hue_1 - $a2_31;
            
            
            if ($a0_16 >= $a2_31)
                $s2_2 = $a0_16 - $a2_31;
            
            $s2 = $v0_36 * ($a0_12 - $t1_14) / $s2_2 + $t1_14;
        }
        else
        {
            int32_t $v0_33 = $a2_31 - bcsh_hue_1;
            int32_t $a0_14 = data_aa720;
            int32_t $s2_1 = $a2_31 - $a0_14;
            
            if ($a2_31 < bcsh_hue_1)
                $v0_33 = bcsh_hue_1 - $a2_31;
            
            
            if ($a0_14 >= $a2_31)
                $s2_1 = $a0_14 - $a2_31;
            
            $s2 = $t1_14 - $v0_33 * ($t1_14 - $a0_12) / $s2_1;
        }
        
        $s6_5 = 1;
    }
    
    void var_178;
    void* $s5_1 = &var_178_1;
    void* i_4 = &var_178_3;
    int32_t* $a0_29 = arg1;
    
    do
    {
        int32_t $v1_17 = *$a0_29;
        
        if ($v1_17 < 0)
        {
            *i_4 = 0xffffffff;
            $v1_17 = -($v1_17);
        }
        else
            *i_4 = 1;
        
        *(((void**)((char*)i_4 + 4))) = $v1_17; // Fixed void pointer dereference
        i_4 += 8;
        $a0_29 = &$a0_29[1];
    } while (&var_130_3 != i_4);
    
    int32_t* $fp_5 = arg1;
    
    for (int32_t i_5 = 0; i_5 != 9; )
    {
        else if (i_5 >= 6)
            int32_t $a3_9 = -($s6_5) * *($s5_1 - 0x18);
            int32_t $v0_54 = fix_point_mult2_32(0x10, $s2, *($s5_1 - 0x14));
        int32_t $v0_49;
        
        if (i_5 < 3)
            $v0_49 = *$s5_1 * *($s5_1 + 4);
        {
            $v0_49 = *$s5_1 * fix_point_mult2_32(0x10, $s1, *($s5_1 + 4)) + $a3_9 * $v0_54;
        }
        else
        {
            int32_t $t1_17 = *$s5_1;
            int32_t $v0_50 = fix_point_mult2_32(0x10, $s1, *($s5_1 + 4));
            $v0_49 = $s6_5 * *($s5_1 + 0x18) * fix_point_mult2_32(0x10, $s2, *($s5_1 + 0x1c))
                + $t1_17 * $v0_50;
        }
        
        i_5 += 1;
        *$fp_5 = $v0_49;
        $s5_1 += 8;
        $fp_5 = &$fp_5[1];
    }
    
    int32_t $v0_56 = *$s3;
    
    while (true)
    {
        int32_t $v0_58;
        
        $v0_58 = $v0_56 < 0 ? -((-($v0_56) >> 6)) : $v0_56 >> 6;
        
        *$s3 = $v0_58;
        $s3 = &$s3[1];
        
        if (&arg1[9] == $s3)
            break;
        
        $v0_56 = *$s3;
    }
    
    return &arg1[9];
}

