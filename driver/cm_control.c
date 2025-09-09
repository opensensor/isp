#include "include/main.h"


  int32_t cm_control(int32_t* arg1, int32_t arg2, int32_t* arg3)

{
    int32_t var_50_16 = ((arg2 * 0xb375) >> 8) + 0x4c8b;
    int32_t var_40_25 = ((arg2 * 0x69ba) >> 8) + 0x9646;
    int32_t var_4c_10 = 0x9646 - ((arg2 * 0x9646) >> 8);
    int32_t var_34_17 = 0x9646 - ((arg2 * 0x9646) >> 8);
    void* $s0 = &cm_in;
    int32_t var_44_19 = 0x4c8b - ((arg2 * 0x4c8b) >> 8);
    int32_t var_38_25 = 0x4c8b - ((arg2 * 0x4c8b) >> 8);
    int32_t var_48_27 = 0x1d2f - ((arg2 * 0x1d2f) >> 8);
    int32_t var_3c_12 = 0x1d2f - ((arg2 * 0x1d2f) >> 8);
    int32_t var_30_9 = ((arg2 * 0xe2d1) >> 8) + 0x1d2f;
    void* i = &cm_in;
    
    do
    {
        int32_t $a1_4;
        
        if (*arg1 < 0)
        {
            *i = 0xffffffff;
            $a1_4 = -(*arg1);
        }
        else
        {
            *i = 1;
            $a1_4 = *arg1;
        }
        
        *(i + 4) = $a1_4;
        i += 8;
        arg1 = &arg1[1];
    } while (&mapG_before != i);
    
    int32_t* $v0_6 = &s_in;
    
    for (int32_t i_1 = 0; i_1 != 0x24; )
    {
        int32_t $v1_4 = *(&var_50_17 + i_1);
        
        if ($v1_4 < 0)
        {
            *$v0_6 = 0xffffffff;
            $v1_4 = -($v1_4);
        }
        else
            *$v0_6 = 1;
        
        i_1 += 4;
        $v0_6[1] = $v1_4;
        $v0_6 = &$v0_6[2];
    }
    
    int32_t* i_2 = arg3;
    int32_t result;
    
    do
    {
        int32_t $fp_2 = *$s0 * s_in;
        int32_t $v0_8 = fix_point_mult2_32(0x10, *($s0 + 4) << 6, data_b88c4_1);
        int32_t $v1_6 =
            *($s0 + 8) * data_b88d8_1 * fix_point_mult2_32(0x10, *($s0 + 0xc) << 6, data_b88dc_1);
        *i_2 = *($s0 + 0x10) * data_b88f0_1 * fix_point_mult2_32(0x10, *($s0 + 0x14) << 6, data_b88f4_1)
            + $v1_6 + $fp_2 * $v0_8;
        int32_t $fp_6 = *$s0 * data_b88c8_1;
        int32_t $v0_13 = fix_point_mult2_32(0x10, *($s0 + 4) << 6, data_b88cc_1);
        int32_t $v1_10 =
            *($s0 + 8) * data_b88e0_1 * fix_point_mult2_32(0x10, *($s0 + 0xc) << 6, data_b88e4_1);
        i_2[1] =
            *($s0 + 0x10) * data_b88f8_1 * fix_point_mult2_32(0x10, *($s0 + 0x14) << 6, data_b88fc_1)
            + $v1_10 + $fp_6 * $v0_13;
        int32_t $fp_10 = *$s0 * data_b88d0_1;
        int32_t $v0_18 = fix_point_mult2_32(0x10, *($s0 + 4) << 6, data_b88d4_1);
        int32_t $v1_14 =
            *($s0 + 8) * data_b88e8_1 * fix_point_mult2_32(0x10, *($s0 + 0xc) << 6, data_b88ec_1);
        int32_t $v0_22 =
            *($s0 + 0x10) * data_b8900_1 * fix_point_mult2_32(0x10, *($s0 + 0x14) << 6, data_b8904_1)
            + $v1_14 + $fp_10 * $v0_18;
        int32_t $v1_17 = *i_2;
        i_2[2] = $v0_22;
        int32_t $v1_18;
        
        $v1_18 = $v1_17 < 0 ? -((-($v1_17) >> 6)) : $v1_17 >> 6;
        
        *i_2 = $v1_18;
        int32_t $v1_21 = i_2[1];
        int32_t $v1_23;
        
        $v1_23 = $v1_21 < 0 ? -((-($v1_21) >> 6)) : $v1_21 >> 6;
        
        i_2[1] = $v1_23;
        
        result = $v0_22 < 0 ? -((-($v0_22) >> 6)) : $v0_22 >> 6;
        
        i_2[2] = result;
        i_2 = &i_2[3];
        $s0 += 0x18;
    } while (i_2 != &arg3[9]);
    
    return result;
}

