#include "include/main.h"


  void* tiziano_bcsh_Tccm_RGBYUV(int32_t* arg1, int32_t* arg2, int32_t* arg3, int32_t* arg4, int32_t arg5 @ $hi)

{
    void var_a0_28;
    void* $s5 = &var_a0_29;
    void* i = &var_a0_30;
    int32_t var_58_8;
    
    do
    {
        int32_t $a0 = *arg2;
        
        if ($a0 < 0)
        {
            *i = 0xffffffff;
            $a0 = -($a0);
        }
        else
            *i = 1;
        
        *(i + 4) = $a0;
        i += 8;
        arg2 = &arg2[1];
    } while (&var_58_9 != i);
    
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
    } while (&var_a0_31 != i_1);
    
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
        int32_t var_e4;
        int32_t $s6_1 = $t1_1 * var_e8_4 * fix_point_mult2_32(0x10, $t5_1, var_e4_1 << 6);
        int32_t $fp_1 = *($s5 + 8);
        int32_t $s4_1 = *($s5 + 0xc);
        int32_t var_d0;
        int32_t var_cc_5;
        int32_t $v1_1 = $fp_1 * var_d0_1 * fix_point_mult2_32(0x10, $s4_1, var_cc_6 << 6);
        int32_t $s0_5 = *($s5 + 0x10);
        int32_t $s6_2 = *($s5 + 0x14);
        int32_t var_b8_37;
        int32_t var_b4_14;
        *i_3 = $s0_5 * var_b8_38 * fix_point_mult2_32(0x10, $s6_2, var_b4_15 << 6) + $v1_1 + $s6_1;
        int32_t var_e0;
        int32_t var_dc;
        int32_t $t6_6 = $t1_1 * var_e0_2 * fix_point_mult2_32(0x10, $t5_1, var_dc_1 << 6);
        int32_t var_c8;
        int32_t var_c4_6;
        int32_t $t6_8 = $fp_1 * var_c8_1 * fix_point_mult2_32(0x10, $s4_1, var_c4_7 << 6) + $t6_6;
        int32_t var_b0_57;
        int32_t var_ac_17;
        i_3[1] = $s0_5 * var_b0_58 * fix_point_mult2_32(0x10, $s6_2, var_ac_18 << 6) + $t6_8;
        int32_t var_d8;
        int32_t var_d4;
        int32_t var_c0;
        int32_t var_bc;
        int32_t $lo_2;
        int32_t $hi;
        $hi = HIGHD(COMBINE(arg5, $t1_1 * var_d8_1 * fix_point_mult2_32(0x10, $t5_1, var_d4_1 << 6))
            + $fp_1 * var_c0_2 * fix_point_mult2_32(0x10, $s4_1, var_bc_2 << 6));
        $lo_2 = LOWD(COMBINE(arg5, $t1_1 * var_d8_2 * fix_point_mult2_32(0x10, $t5_1, var_d4_2 << 6))
            + $fp_1 * var_c0_3 * fix_point_mult2_32(0x10, $s4_1, var_bc_3 << 6));
        int32_t j = 0;
        int32_t var_a8_24;
        int32_t var_a4_19;
        i_3[2] = $s0_5 * var_a8_25 * fix_point_mult2_32(0x10, $s6_2, var_a4_20 << 6) + $lo_2;
        int32_t* $v0_16 = &var_58_10;
        
        do
        {
            int32_t $a0_4 = *(i_3 + j);
            
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
        } while (j != 0xc);
        
        int32_t $t2_7 = var_58_11;
        int32_t var_12c;
        int32_t var_54_4;
        int32_t $s6_3 = $t2_7 * var_130_2 * fix_point_mult2_32(0x10, var_54_5, var_12c_1);
        int32_t var_118_9;
        int32_t var_114_7;
        int32_t var_50_16;
        int32_t var_4c_11;
        int32_t $t7_5 = var_50_17 * var_118_10 * fix_point_mult2_32(0x10, var_4c_12, var_114_8) + $s6_3;
        int32_t var_100_2;
        int32_t var_fc;
        int32_t var_48_29;
        int32_t var_44_20;
        *i_3 = var_48_30 * var_100_3 * fix_point_mult2_32(0x10, var_44_21, var_fc_2) + $t7_5;
        int32_t var_128_20;
        int32_t var_124_19;
        int32_t $t7_10 = $t2_7 * var_128_21 * fix_point_mult2_32(0x10, var_54_6, var_124_20);
        int32_t var_110;
        int32_t var_10c_3;
        int32_t $t7_12 = var_50_18 * var_110_2 * fix_point_mult2_32(0x10, var_4c_13, var_10c_4) + $t7_10;
        int32_t var_f8_23;
        int32_t var_f4;
        i_3[1] = var_48_31 * var_f8_24 * fix_point_mult2_32(0x10, var_44_22, var_f4_1) + $t7_12;
        int32_t var_120_11;
        int32_t var_11c_10;
        int32_t var_108_3;
        int32_t var_104;
        int32_t $lo_4;
        arg5 = HIGHD(COMBINE($hi, $t2_7 * var_120_12 * fix_point_mult2_32(0x10, var_54_7, var_11c_11))
            + var_50_19 * var_108_4 * fix_point_mult2_32(0x10, var_4c_14, var_104_1));
        $lo_4 = LOWD(COMBINE($hi, $t2_7 * var_120_13 * fix_point_mult2_32(0x10, var_54_8, var_11c_12))
            + var_50_20 * var_108_5 * fix_point_mult2_32(0x10, var_4c_15, var_104_2));
        i_3 = &i_3[3];
        int32_t var_f0;
        int32_t var_ec;
        *(i_3 - 4) = var_48_32 * var_f0_1 * fix_point_mult2_32(0x10, var_44_23, var_ec_2) + $lo_4;
        $s5 += 0x18;
    } while (&arg1[9] != i_3);
    
    uint32_t bcsh_hue_1 = bcsh_hue;
    int32_t $s1;
    int32_t $s2;
    int32_t $s6_5;
    
    if (bcsh_hue_1 == 0x3c)
    {
        $s1 = data_aa710_1;
        $s2 = data_aa704_1;
        $s6_5 = 1;
    }
    else if (bcsh_hue_1 < 0x3d)
    {
        int32_t CosValue_1 = CosValue;
        int32_t $a0_17 = data_aa710_2;
        int32_t HueIndex_2 = HueIndex;
        
        if ($a0_17 >= CosValue_1)
        {
            int32_t $a1_30 = HueIndex_2 - bcsh_hue_1;
            
            if (HueIndex_2 < bcsh_hue_1)
                $a1_30 = bcsh_hue_1 - HueIndex_2;
            
            int32_t $a2_37 = data_aa71c_1;
            int32_t $s1_4 = HueIndex_2 - $a2_37;
            
            if ($a2_37 >= HueIndex_2)
                $s1_4 = $a2_37 - HueIndex_2;
            
            $s1 = $a1_30 * ($a0_17 - CosValue_1) / $s1_4 + CosValue_1;
        }
        else
        {
            int32_t $a1_28 = HueIndex_2 - bcsh_hue_1;
            
            if (HueIndex_2 < bcsh_hue_1)
                $a1_28 = bcsh_hue_1 - HueIndex_2;
            
            int32_t $a2_35 = data_aa71c_2;
            int32_t $s1_3 = HueIndex_2 - $a2_35;
            
            if ($a2_35 >= HueIndex_2)
                $s1_3 = $a2_35 - HueIndex_2;
            
            $s1 = CosValue_1 - $a1_28 * (CosValue_1 - $a0_17) / $s1_3;
        }
        
        int32_t SinValue_1 = SinValue;
        int32_t $a0_24 = data_aa704_2;
        int32_t HueIndex_1 = HueIndex;
        
        if ($a0_24 >= SinValue_1)
        {
            int32_t $v0_42 = HueIndex_1 - bcsh_hue_1;
            
            if (HueIndex_1 < bcsh_hue_1)
                $v0_42 = bcsh_hue_1 - HueIndex_1;
            
            int32_t $a0_28 = data_aa71c_3;
            int32_t $s2_5 = HueIndex_1 - $a0_28;
            
            if ($a0_28 >= HueIndex_1)
                $s2_5 = $a0_28 - HueIndex_1;
            
            $s2 = $v0_42 * ($a0_24 - SinValue_1) / $s2_5 + SinValue_1;
        }
        else
        {
            int32_t $v0_39 = HueIndex_1 - bcsh_hue_1;
            
            if (HueIndex_1 < bcsh_hue_1)
                $v0_39 = bcsh_hue_1 - HueIndex_1;
            
            int32_t $a0_26 = data_aa71c_4;
            int32_t $s2_3 = HueIndex_1 - $a0_26;
            
            if ($a0_26 >= HueIndex_1)
                $s2_3 = $a0_26 - HueIndex_1;
            
            $s2 = SinValue_1 - $v0_39 * (SinValue_1 - $a0_24) / $s2_3;
        }
        
        $s6_5 = 0xffffffff;
    }
    else
    {
        int32_t $t3_7 = data_aa710_3;
        int32_t $a0_5 = data_aa714_1;
        int32_t $t2_15 = data_aa71c_5;
        
        if ($a0_5 >= $t3_7)
        {
            int32_t $a1_22 = $t2_15 - bcsh_hue_1;
            
            if ($t2_15 < bcsh_hue_1)
                $a1_22 = bcsh_hue_1 - $t2_15;
            
            int32_t $a2_30 = data_aa720_1;
            int32_t $s1_2 = $t2_15 - $a2_30;
            
            if ($a2_30 >= $t2_15)
                $s1_2 = $a2_30 - $t2_15;
            
            $s1 = $a1_22 * ($a0_5 - $t3_7) / $s1_2 + $t3_7;
        }
        else
        {
            int32_t $a1_20 = $t2_15 - bcsh_hue_1;
            
            if ($t2_15 < bcsh_hue_1)
                $a1_20 = bcsh_hue_1 - $t2_15;
            
            int32_t $a2_28 = data_aa720_2;
            int32_t $s1_1 = $t2_15 - $a2_28;
            
            if ($a2_28 >= $t2_15)
                $s1_1 = $a2_28 - $t2_15;
            
            $s1 = $t3_7 - $a1_20 * ($t3_7 - $a0_5) / $s1_1;
        }
        
        int32_t $t1_14 = data_aa704_3;
        int32_t $a0_12 = data_aa708_1;
        int32_t $a2_31 = data_aa71c_6;
        
        if ($a0_12 >= $t1_14)
        {
            int32_t $v0_36 = $a2_31 - bcsh_hue_1;
            
            if ($a2_31 < bcsh_hue_1)
                $v0_36 = bcsh_hue_1 - $a2_31;
            
            int32_t $a0_16 = data_aa720_3;
            int32_t $s2_2 = $a2_31 - $a0_16;
            
            if ($a0_16 >= $a2_31)
                $s2_2 = $a0_16 - $a2_31;
            
            $s2 = $v0_36 * ($a0_12 - $t1_14) / $s2_2 + $t1_14;
        }
        else
        {
            int32_t $v0_33 = $a2_31 - bcsh_hue_1;
            
            if ($a2_31 < bcsh_hue_1)
                $v0_33 = bcsh_hue_1 - $a2_31;
            
            int32_t $a0_14 = data_aa720_4;
            int32_t $s2_1 = $a2_31 - $a0_14;
            
            if ($a0_14 >= $a2_31)
                $s2_1 = $a0_14 - $a2_31;
            
            $s2 = $t1_14 - $v0_33 * ($t1_14 - $a0_12) / $s2_1;
        }
        
        $s6_5 = 1;
    }
    
    void var_178;
    void* $s5_1 = &var_178_3;
    void* i_4 = &var_178_4;
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
        
        *(i_4 + 4) = $v1_17;
        i_4 += 8;
        $a0_29 = &$a0_29[1];
    } while (&var_130_3 != i_4);
    
    int32_t* $fp_5 = arg1;
    
    for (int32_t i_5 = 0; i_5 != 9; )
    {
        int32_t $v0_49;
        
        if (i_5 < 3)
            $v0_49 = *$s5_1 * *($s5_1 + 4);
        else if (i_5 >= 6)
        {
            int32_t $a3_9 = -($s6_5) * *($s5_1 - 0x18);
            int32_t $v0_54 = fix_point_mult2_32(0x10, $s2, *($s5_1 - 0x14));
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

