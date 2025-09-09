#include "include/main.h"


  int32_t tiziano_wdr_5x5_param()

{
    void var_c0_10;
    memcpy(&var_c0_11, 0x7d9c0, 0x7c);
    void* var_38_1_9;
    
    for (int32_t i = 0; i != 0x80; )
    {
        *(&wdr_lut7_counter + i) = 0;
        *(&wdr_lut8_counter + i) = 0;
        *(&wdr_lut12_counter + i) = 0;
        *(&wdr_lut1_value_sum + i) = 0;
        *(&wdr_lut2_value_sum + i) = 0;
        *(&wdr_lut3_value_sum + i) = 0;
        *(&wdr_lut6_value_sum + i) = 0;
        void* $t4_8 = &wdr_lut11_value_sum + i;
        i += 4;
        *$t4_8 = 0;
        var_38_1_10 = &wdr_lut7_counter;
    }
    
    uint32_t $lo = (height_wdr_def + 5) / 0xa;
    uint32_t $t2_2 = (width_wdr_def + 8) >> 4;
    uint32_t i_4 = $t2_2 << 1;
    int32_t $s0 = $t2_2 + (($t2_2 + 1) >> 1);
    int32_t i_1 = 0;
    uint32_t $t1 = $lo << 1;
    uint32_t $v0 = $t1 + $lo;
    int32_t $v0_2 = data_b1580_1;
    int32_t $lo_1;
    int32_t $hi;
    $hi = HIGHD($v0_2 * $v0_2);
    $lo_1 = LOWD($v0_2 * $v0_2);
    int32_t $t9 = $lo + (($lo + 1) >> 1);
    int16_t* $t4_13;
    
    do
    {
        int32_t $v0_5 = *(&var_c0_12 + i_1);
        void* $t3_1 = &param_centre5x5_w_distance_array_def + i_1;
        int32_t $lo_3;
        int32_t $hi_2;
        $hi_2 = HIGHD($hi * $v0_5 + ($v0_5 >> 0x1f) * $lo_1);
        $lo_3 = LOWD($hi * $v0_5 + ($v0_5 >> 0x1f) * $lo_1);
        int32_t $lo_4;
        int32_t $hi_3;
        $hi_3 = HIGHD($v0_5 * $lo_1);
        $lo_4 = LOWD($v0_5 * $lo_1);
        i_1 += 4;
        *$t3_1 =
            (($lo_4 + 0x8000 < $lo_4 ? 1 : 0) + $lo_3 + $hi_3) << 0x10 | ($lo_4 + 0x8000) >> 0x10;
        $t4_13 = &param_centre5x5_w_distance_array_def;
    } while (i_1 != 0x7c);
    
    while (true)
    {
        uint32_t i_2 = i_4;
        
        if ($v0 < $t1)
            break;
        
        while (i_4 + $t2_2 >= i_2)
        {
            int32_t $v0_14;
            int32_t $t1_1;
            int32_t $t3_2;
            int32_t $t4_14;
            int32_t $t5_2;
            int32_t $t7_2;
            int32_t $t8_2;
            $v0_14 = (0x70000 - 0xce8)($t1, i_2, $t9, $s0, $t4_13);
            void* $a0_2 = var_38_1_11 + ($v0_14 << 2);
            *$a0_2 += 1;
            int32_t $v0_18;
            int32_t $t1_2;
            int32_t $t3_3;
            int32_t $t4_15;
            int32_t $t5_3;
            int32_t $t6_2;
            int32_t $t8_3;
            int32_t $t9_1;
            $v0_18 = $t5_2($t1_1, $t3_2, $t7_2, $t8_2, $t4_14);
            *(&wdr_lut1_value_sum + $t6_2) += $v0_18;
            int32_t $v0_20;
            int32_t $t1_3;
            int32_t $t3_4;
            int32_t $t4_16;
            int32_t $t5_4;
            int32_t $t6_3;
            int32_t $t7_3;
            $v0_20 = $t5_3($t1_2, $t3_3, $t9_1, $t8_3, $t4_15);
            *(&wdr_lut2_value_sum + $t6_3) += $v0_20;
            int32_t $v0_22;
            int32_t $t0_4;
            int32_t $t1_4;
            int32_t $t3_5;
            int32_t $t4_17;
            int32_t $t5_5;
            int32_t $t6_4;
            $v0_22 = $t5_4($t1_3, $t3_4, $t7_3, $s0, $t4_16);
            *(&wdr_lut6_value_sum + $t6_4) += $v0_22;
            int32_t $v0_24;
            int32_t $t0_5;
            int32_t $t1_5;
            int32_t $t3_6;
            int32_t $t4_18;
            int32_t $t5_6;
            int32_t $t8_4;
            $v0_24 = $t5_5($t1_4, $t3_5, $t0_4, $s0, $t4_17);
            *(&wdr_lut8_counter + ($v0_24 << 2)) += 1;
            int32_t $v0_27;
            int32_t $t1_6;
            int32_t $t2_4;
            int32_t $t3_7;
            int32_t $t4_19;
            int32_t $t5_7;
            int32_t $t6_7;
            int32_t $t9_2;
            $v0_27 = $t5_6($t1_5, $t3_6, $t0_5, $t8_4, $t4_18);
            *(&wdr_lut3_value_sum + $t6_7) += $v0_27;
            int32_t $v0_29;
            int32_t $t1_7;
            int32_t $t2_5;
            int32_t $t3_8;
            int32_t $t4_20;
            int32_t $t5_8;
            int32_t $t7_4;
            $v0_29 = $t5_7($t1_6, $t3_7, $t9_2, $t2_4, $t4_19);
            *(&wdr_lut12_counter + ($v0_29 << 2)) += 1;
            int32_t $v0_32;
            int32_t $t3_9;
            int32_t $t6_10;
            $v0_32 = $t5_8($t1_7, $t3_8, $t7_4, $t2_5, $t4_20);
            i_2 = $t3_9 + 1;
            *(&wdr_lut11_value_sum + $t6_10) += $v0_32;
        }
        
        $t1 += 1;
    }
    
    int32_t* $t3_10 = &param_wdr_weightLUT20_array_def;
    int32_t* $t2_6 = &param_wdr_weightLUT02_array_def;
    int32_t* $t1_8 = &param_wdr_weightLUT22_array_def;
    int32_t* $t0_6 = &param_wdr_weightLUT12_array_def;
    int32_t* $a3_9 = &param_wdr_weightLUT21_array_def;
    int32_t $v1_8 = 0;
    
    for (int32_t i_3 = 0; i_3 != 0x20; )
    {
        int32_t $v0_36 = *(var_38_1_12 + $v1_8);
        
        if (!($v0_36 | i_3))
        {
            param_wdr_weightLUT22_array_def = 0;
            param_wdr_weightLUT12_array_def = 0;
            param_wdr_weightLUT21_array_def = 0;
        }
        else if ($v0_36)
        {
            int32_t $lo_5 = $v0_36 / 2;
            *$t1_8 = ($lo_5 + *(&wdr_lut1_value_sum + $v1_8)) / $v0_36;
            *$t0_6 = ($lo_5 + *(&wdr_lut2_value_sum + $v1_8)) / $v0_36;
            *$a3_9 = ($lo_5 + *(&wdr_lut6_value_sum + $v1_8)) / $v0_36;
        }
        else
        {
            *$t1_8 = *($t1_8 - 4);
            *$t0_6 = *($t0_6 - 4);
            *$a3_9 = *($a3_9 - 4);
        }
        
        int32_t $v0_41 = *(&wdr_lut8_counter + $v1_8);
        
        if ($v0_41 | i_3)
        {
            int32_t $v0_42;
            
            if ($v0_41)
                $v0_42 = ($v0_41 / 2 + *(&wdr_lut3_value_sum + $v1_8)) / $v0_41;
            else
                $v0_42 = *($t2_6 - 4);
            
            *$t2_6 = $v0_42;
        }
        else
            param_wdr_weightLUT02_array_def = 0;
        
        int32_t $v0_44 = *(&wdr_lut12_counter + $v1_8);
        
        if ($v0_44 | i_3)
        {
            int32_t $v0_45;
            
            if ($v0_44)
                $v0_45 = ($v0_44 / 2 + *(&wdr_lut11_value_sum + $v1_8)) / $v0_44;
            else
                $v0_45 = *($t3_10 - 4);
            
            *$t3_10 = $v0_45;
        }
        else
            param_wdr_weightLUT20_array_def = 0;
        
        i_3 += 1;
        $v1_8 += 4;
        $t3_10 = &$t3_10[1];
        $t2_6 = &$t2_6[1];
        $t1_8 = &$t1_8[1];
        $t0_6 = &$t0_6[1];
        $a3_9 = &$a3_9[1];
    }
    
    return 0;
}

