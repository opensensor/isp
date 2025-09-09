#include "include/main.h"


  int32_t tiziano_bcsh_Toffset_RGBYUV(int32_t* arg1, int32_t* arg2, int32_t* arg3)

{
    int32_t var_78;
    int32_t* i = &var_78;
    int32_t var_30;
        int32_t $v1_1 = *arg2;
    
    do
    {
        
        if ($v1_1 < 0)
        {
            *i = 0xffffffff;
            $v1_1 = -($v1_1);
        }
        else
            *i = 1;
        
        i[1] = $v1_1;
        i = &i[2];
        arg2 = &arg2[1];
    } while (&var_30_3 != i);
    
    int32_t var_c0;
    int32_t* i_1 = &var_c0_2;
    
    do
    {
        int32_t $v1_2 = *arg3;
        
        if ($v1_2 < 0)
        {
            *i_1 = 0xffffffff;
            $v1_2 = -($v1_2);
        }
        else
            *i_1 = 1;
        
        i_1[1] = $v1_2;
        i_1 = &i_1[2];
        arg3 = &arg3[1];
    } while (&var_78_17 != i_1);
    
    int32_t var_bc;
    int32_t $s5_1 = var_bc_2 << 6;
    int32_t $s4 = var_c0_3;
    int32_t var_b4_3;
    int32_t $s7_1 = var_b4_4 << 6;
    int32_t var_74;
    int32_t $fp_2 = $s4 * var_78_18 * fix_point_mult2_32(0x10, var_74_2, $s5_1);
    int32_t var_ac;
    int32_t $s6_1 = var_ac_3 << 6;
    int32_t var_b8_9;
    int32_t var_70_8;
    int32_t var_6c_5;
    var_30_4 = var_b8_10 * var_70_9 * fix_point_mult2_32(0x10, var_6c_6, $s7_1) + $fp_2;
    int32_t var_b0_18;
    int32_t var_68_7;
    int32_t var_64_7;
    *arg1 = var_b0_19 * var_68_8 * fix_point_mult2_32(0x10, var_64_8, $s6_1) + var_30_5;
    int32_t var_60_3;
    int32_t var_5c_1;
    var_30_6 = $s4 * var_60_4 * fix_point_mult2_32(0x10, var_5c_2, $s5_1);
    int32_t var_58_6;
    int32_t var_54_5;
    var_30_7 += var_b8_11 * var_58_7 * fix_point_mult2_32(0x10, var_54_6, $s7_1);
    int32_t var_50_8;
    int32_t var_4c_6;
    arg1[1] = var_b0_20 * var_50_9 * fix_point_mult2_32(0x10, var_4c_7, $s6_1) + var_30_8;
    int32_t var_48_19;
    int32_t var_44_11;
    int32_t $s5_2 = $s4 * var_48_20 * fix_point_mult2_32(0x10, var_44_12, $s5_1);
    int32_t var_40_14;
    int32_t var_3c_4;
    int32_t $s3_1 = var_b8_12 * var_40_15 * fix_point_mult2_32(0x10, var_3c_5, $s7_1) + $s5_2;
    int32_t* $a0_4 = arg1;
    int32_t var_38_6;
    int32_t var_34_15;
    arg1[2] = var_b0_21 * var_38_7 * fix_point_mult2_32(0x10, var_34_16, $s6_1) + $s3_1;
    int32_t $v0_15 = *$a0_4;
    int32_t result;
    
    while (true)
    {
        result = $v0_15 < 0 ? -((-($v0_15) >> 6)) : $v0_15 >> 6;
        
        *$a0_4 = result;
        $a0_4 = &$a0_4[1];
        
        if (&arg1[3] == $a0_4)
            break;
        
        $v0_15 = *$a0_4;
    }
    
    return result;
}

