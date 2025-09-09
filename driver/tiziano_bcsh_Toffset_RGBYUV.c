#include "include/main.h"


  int32_t tiziano_bcsh_Toffset_RGBYUV(int32_t* arg1, int32_t* arg2, int32_t* arg3)

{
    int32_t var_78_30;
    int32_t* i = &var_78_31;
    int32_t var_30_12;
    
    do
    {
        int32_t $v1_1 = *arg2;
        
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
    } while (&var_30_13 != i);
    
    int32_t var_c0_4;
    int32_t* i_1 = &var_c0_5;
    
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
    } while (&var_78_32 != i_1);
    
    int32_t var_bc_4;
    int32_t $s5_1 = var_bc_5 << 6;
    int32_t $s4 = var_c0_6;
    int32_t var_b4_14;
    int32_t $s7_1 = var_b4_15 << 6;
    int32_t var_74_10;
    int32_t $fp_2 = $s4 * var_78_33 * fix_point_mult2_32(0x10, var_74_11, $s5_1);
    int32_t var_ac_17;
    int32_t $s6_1 = var_ac_18 << 6;
    int32_t var_b8_37;
    int32_t var_70_16;
    int32_t var_6c_12;
    var_30_14 = var_b8_38 * var_70_17 * fix_point_mult2_32(0x10, var_6c_13, $s7_1) + $fp_2;
    int32_t var_b0_57;
    int32_t var_68_14;
    int32_t var_64_14;
    *arg1 = var_b0_58 * var_68_15 * fix_point_mult2_32(0x10, var_64_15, $s6_1) + var_30_15;
    int32_t var_60_8;
    int32_t var_5c_3;
    var_30_16 = $s4 * var_60_9 * fix_point_mult2_32(0x10, var_5c_4, $s5_1);
    int32_t var_58_12;
    int32_t var_54_9;
    var_30_17 += var_b8_39 * var_58_13 * fix_point_mult2_32(0x10, var_54_10, $s7_1);
    int32_t var_50_23;
    int32_t var_4c_16;
    arg1[1] = var_b0_59 * var_50_24 * fix_point_mult2_32(0x10, var_4c_17, $s6_1) + var_30_18;
    int32_t var_48_33;
    int32_t var_44_24;
    int32_t $s5_2 = $s4 * var_48_34 * fix_point_mult2_32(0x10, var_44_25, $s5_1);
    int32_t var_40_26;
    int32_t var_3c_13;
    int32_t $s3_1 = var_b8_40 * var_40_27 * fix_point_mult2_32(0x10, var_3c_14, $s7_1) + $s5_2;
    int32_t* $a0_4 = arg1;
    int32_t var_38_30;
    int32_t var_34_23;
    arg1[2] = var_b0_60 * var_38_31 * fix_point_mult2_32(0x10, var_34_24, $s6_1) + $s3_1;
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

