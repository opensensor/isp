#include "include/main.h"


  int32_t tiziano_adr_5x5_param()

{
    void var_c0;
    void* var_38_1;
        char* $t4_8 = (char*)(&adr_lut11_value_sum + i); // Fixed void pointer assignment
    memcpy(&var_c0, 0x7d380, 0x7c);
    
    for (int32_t i = 0; (uintptr_t)i != 0x80; )
    {
        *(&adr_lut7_counter + i) = 0;
        *(&adr_lut8_counter + i) = 0;
        *(&adr_lut12_counter + i) = 0;
        *(&adr_lut1_value_sum + i) = 0;
        *(&adr_lut2_value_sum + i) = 0;
        *(&adr_lut3_value_sum + i) = 0;
        *(&adr_lut6_value_sum + i) = 0;
        i += 4;
        *$t4_8 = 0;
        var_38_1 = &adr_lut7_counter;
    }
    
    uint32_t $lo = (width_def + 3) / 6;
    uint32_t $t2_2 = (height_def + 2) >> 2;
    uint32_t $t1 = $t2_2 << 1;
    uint32_t $v0 = $t1 + $t2_2;
    int32_t $t9 = $t2_2 + (($t2_2 + 1) >> 1);
    int32_t i_1 = 0;
    uint32_t i_4 = $lo << 1;
    int32_t $v0_2 = data_ace54_1;
    int32_t $lo_1;
    int32_t $hi;
    $hi = HIGHD($v0_2 * $v0_2);
    $lo_1 = LOWD($v0_2 * $v0_2);
    int32_t $s0 = $lo + (($lo + 1) >> 1);
    void* $t4_13;
    
    do
    {
        int32_t $v0_5 = *(&var_c0 + i_1);
        char* $t3_1 = (char*)(&param_adr_centre_w_dis_array_tmp + i_1); // Fixed void pointer assignment
        int32_t $lo_3;
        int32_t $hi_2;
        int32_t $lo_4;
        int32_t $hi_3;
        uint32_t i_2 = i_4;
            int32_t $v0_14;
            int32_t $t1_1;
            int32_t $t3_2;
            int32_t $t4_14;
            int32_t $t5_2;
            int32_t $t7_2;
            int32_t $t8_2;
            char* $a0_2 = (char*)(var_38_1 + ($v0_14 << 2)); // Fixed void pointer assignment
            int32_t $v0_18;
            int32_t $t1_2;
            int32_t $t3_3;
            int32_t $t4_15;
            int32_t $t5_3;
            int32_t $t6_2;
            int32_t $t8_3;
            int32_t $t9_1;
            int32_t $v0_20;
            int32_t $t1_3;
            int32_t $t3_4;
            int32_t $t4_16;
            int32_t $t5_4;
            int32_t $t6_3;
            int32_t $t7_3;
            int32_t $v0_22;
            int32_t $t1_4;
            int32_t $t2_4;
            int32_t $t3_5;
            int32_t $t4_17;
            int32_t $t5_5;
            int32_t $t6_4;
            int32_t $v0_24;
            int32_t $t1_5;
            int32_t $t2_5;
            int32_t $t3_6;
            int32_t $t4_18;
            int32_t $t5_6;
            int32_t $t8_4;
            int32_t $v0_27;
            int32_t $t0_4;
            int32_t $t1_6;
            int32_t $t3_7;
            int32_t $t4_19;
            int32_t $t5_7;
            int32_t $t6_7;
            int32_t $t9_2;
            int32_t $v0_29;
            int32_t $t0_5;
            int32_t $t1_7;
            int32_t $t3_8;
            int32_t $t4_20;
            int32_t $t5_8;
            int32_t $t7_4;
            int32_t $v0_32;
            int32_t $t3_9;
            int32_t $t6_10;
        $hi_2 = HIGHD($hi * $v0_5 + ($v0_5 >> 0x1f) * $lo_1);
        $lo_3 = LOWD($hi * $v0_5 + ($v0_5 >> 0x1f) * $lo_1);
        $hi_3 = HIGHD($v0_5 * $lo_1);
        $lo_4 = LOWD($v0_5 * $lo_1);
        i_1 += 4;
        *$t3_1 = (($lo_4 + &data_20000 < $lo_4 ? 1 : 0) + $lo_3 + $hi_3) << 0xe
            | ($lo_4 + &data_20000) >> 0x12;
        $t4_13 = &param_adr_centre_w_dis_array_tmp;
    } while ((uintptr_t)i_1 != 0x7c);
    
    while (true)
    {
        
        if ($v0 < $t1)
            break;
        
        while (i_4 + $lo >= i_2)
        {
            $v0_14 = (0x60000 - 0x53ac)($t1, i_2, $t9, $s0, $t4_13);
            *$a0_2 += 1;
            $v0_18 = $t5_2($t1_1, $t3_2, $t7_2, $t8_2, $t4_14);
            *(&adr_lut1_value_sum + $t6_2) += $v0_18;
            $v0_20 = $t5_3($t1_2, $t3_3, $t9_1, $t8_3, $t4_15);
            *(&adr_lut2_value_sum + $t6_3) += $v0_20;
            $v0_22 = $t5_4($t1_3, $t3_4, $t7_3, $s0, $t4_16);
            *(&adr_lut6_value_sum + $t6_4) += $v0_22;
            $v0_24 = $t5_5($t1_4, $t3_5, $t2_4, $s0, $t4_17);
            *(&adr_lut8_counter + ($v0_24 << 2)) += 1;
            $v0_27 = $t5_6($t1_5, $t3_6, $t2_5, $t8_4, $t4_18);
            *(&adr_lut3_value_sum + $t6_7) += $v0_27;
            $v0_29 = $t5_7($t1_6, $t3_7, $t9_2, $t0_4, $t4_19);
            *(&adr_lut12_counter + ($v0_29 << 2)) += 1;
            $v0_32 = $t5_8($t1_7, $t3_8, $t7_4, $t0_5, $t4_20);
            i_2 = $t3_9 + 1;
            *(&adr_lut11_value_sum + $t6_10) += $v0_32;
        }
        
        $t1 += 1;
    }
    
    int32_t* $t3_10 = &param_adr_weight_20_lut_array_tmp;
    int32_t* $t2_6 = &param_adr_weight_02_lut_array_tmp;
    int32_t* $t1_8 = &param_adr_weight_22_lut_array_tmp;
    int32_t* $t0_6 = &param_adr_weight_12_lut_array_tmp;
    int32_t* $a3_9 = &param_adr_weight_21_lut_array_tmp;
    int32_t $v1_8 = 0;
    
    for (int32_t i_3 = 0; (uintptr_t)i_3 != 0x20; )
    {
        int32_t $v0_36 = *(var_38_1 + $v1_8);
        
        if (!($v0_36 | i_3))
        {
            param_adr_weight_22_lut_array_tmp = 0;
            param_adr_weight_12_lut_array_tmp = 0;
            param_adr_weight_21_lut_array_tmp = 0;
        }
        else if ($v0_36)
        {
            int32_t $lo_5 = $v0_36 / 2;
            *((int32_t*)$t1_8) = ($lo_5 + *(&adr_lut1_value_sum + $v1_8)) / $v0_36; // Fixed void pointer dereference
            *((int32_t*)$t0_6) = ($lo_5 + *(&adr_lut2_value_sum + $v1_8)) / $v0_36; // Fixed void pointer dereference
            *((int32_t*)$a3_9) = ($lo_5 + *(&adr_lut6_value_sum + $v1_8)) / $v0_36; // Fixed void pointer dereference
        }
        else
        {
            *((int32_t*)$t1_8) = *($t1_8 - 4); // Fixed void pointer dereference
            *((int32_t*)$t0_6) = *($t0_6 - 4); // Fixed void pointer dereference
            *((int32_t*)$a3_9) = *($a3_9 - 4); // Fixed void pointer dereference
        }
        
        int32_t $v0_41 = *(&adr_lut8_counter + $v1_8);
        
        if ($v0_41 | i_3)
        {
            int32_t $v0_42;
            
            if ($v0_41)
                $v0_42 = ($v0_41 / 2 + *(&adr_lut3_value_sum + $v1_8)) / $v0_41;
            else
                $v0_42 = *($t2_6 - 4);
            
            *$t2_6 = $v0_42;
        }
        else
            param_adr_weight_02_lut_array_tmp = 0;
        
        int32_t $v0_44 = *(&adr_lut12_counter + $v1_8);
        
        if ($v0_44 | i_3)
        {
            int32_t $v0_45;
            
            if ($v0_44)
                $v0_45 = ($v0_44 / 2 + *(&adr_lut11_value_sum + $v1_8)) / $v0_44;
            else
                $v0_45 = *($t3_10 - 4);
            
            *$t3_10 = $v0_45;
        }
        else
            param_adr_weight_20_lut_array_tmp = 0;
        
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

