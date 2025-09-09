#include "include/main.h"


  int32_t subsection(int32_t* arg1, int32_t arg2, int16_t* arg3, void* arg4, int32_t* arg5, int32_t arg6, int32_t arg7, int32_t arg8, int32_t arg9)

{
    int32_t $v0 = 2 << (arg7 & 0x1f);
    arg1[8] = 0xfff;
    *arg1 = 0;
    int32_t $v0_3 = subsection_map(0x1388, 
        (fix_point_div_32(arg7, 0xfff << (arg7 & 0x1f), $v0) + 0x200) >> (arg7 & 0x1f), arg2, arg3, 
        arg4, arg5, arg6, arg7, arg8, arg9);
    int16_t* var_3c_8 = arg4;
    void* $a1_2 = arg4;
    int32_t $v0_4 = 0;
    int32_t $a1_9;
    
    while (true)
    {
        int32_t $a0_1 = *$a1_2;
        
        if ($v0_3 < $a0_1)
        {
            void* $s7_2 = &arg3[$v0_4];
            int32_t $v0_8 = fix_point_div_32(arg7, 
                (*$s7_2 - *(arg3 + ($v0_4 << 1) - 2)) << (arg7 & 0x1f), 
                ($a0_1 - *(arg4 + ($v0_4 << 1) - 2)) << (arg7 & 0x1f));
            int32_t $v0_10 =
                fix_point_mult2_32(arg7, $v0_8, ($a0_1 - $v0_3) << (arg7 & 0x1f)) + 0x200;
            $a1_9 = *$s7_2 - ($v0_10 >> (arg7 & 0x1f));
            break;
        }
        
        $v0_4 += 1;
        $a1_2 += 2;
        
        if ($v0_4 == 0x81)
        {
            $a1_9 = arg3[0x80];
            break;
        }
    }
    
    arg1[4] = $a1_9;
    int32_t $fp_2 = arg6 << (arg7 & 0x1f);
    int32_t $v0_20 = subsection_map(
        arg5[(fix_point_div_32(arg7, $a1_9 << (arg7 & 0x1f), $fp_2) + 0x200) >> (arg7 & 0x1f)] / 2, 
        (fix_point_div_32(arg7, $v0_3 << (arg7 & 0x1f), $v0) + 0x200) >> (arg7 & 0x1f), arg2, arg3, 
        arg4, arg5, arg6, arg7, arg8, arg9);
    void* $a1_13 = arg4;
    int32_t $v0_21 = 0;
    int32_t $a1_20;
    
    while (true)
    {
        int32_t $a0_9 = *$a1_13;
        
        if ($v0_20 < $a0_9)
        {
            void* $a3_4 = &arg3[$v0_21];
            int32_t $v0_25 = fix_point_div_32(arg7, 
                (*$a3_4 - *(arg3 + ($v0_21 << 1) - 2)) << (arg7 & 0x1f), 
                ($a0_9 - *(arg4 + ($v0_21 << 1) - 2)) << (arg7 & 0x1f));
            int32_t $v0_27 =
                fix_point_mult2_32(arg7, $v0_25, ($a0_9 - $v0_20) << (arg7 & 0x1f)) + 0x200;
            $a1_20 = *$a3_4 - ($v0_27 >> (arg7 & 0x1f));
            break;
        }
        
        $v0_21 += 1;
        $a1_13 += 2;
        
        if ($v0_21 == 0x81)
        {
            $a1_20 = arg3[0x80];
            break;
        }
    }
    
    arg1[2] = $a1_20;
    int32_t $v0_36 = subsection_map(
        arg5[(fix_point_div_32(arg7, $a1_20 << (arg7 & 0x1f), $fp_2) + 0x200) >> (arg7 & 0x1f)]
            / 2, 
        (fix_point_div_32(arg7, $v0_20 << (arg7 & 0x1f), $v0) + 0x200) >> (arg7 & 0x1f), arg2, 
        arg3, arg4, arg5, arg6, arg7, arg8, arg9);
    void* $a2_16 = arg4;
    int32_t $a0_17 = 0;
    int32_t $v0_43;
    
    while (true)
    {
        int32_t $a1_24 = *$a2_16;
        
        if ($v0_36 < $a1_24)
        {
            void* $t1_7 = &arg3[$a0_17];
            int32_t $v0_39 = fix_point_div_32(arg7, 
                (*$t1_7 - *(arg3 + ($a0_17 << 1) - 2)) << (arg7 & 0x1f), 
                ($a1_24 - *(arg4 + ($a0_17 << 1) - 2)) << (arg7 & 0x1f));
            int32_t $v0_41 =
                fix_point_mult2_32(arg7, $v0_39, ($a1_24 - $v0_36) << (arg7 & 0x1f)) + 0x200;
            $v0_43 = *$t1_7 - ($v0_41 >> (arg7 & 0x1f));
            break;
        }
        
        $a0_17 += 1;
        $a2_16 += 2;
        
        if ($a0_17 == 0x81)
        {
            $v0_43 = arg3[0x80];
            break;
        }
    }
    
    int32_t $a1_30 = arg1[2] << (arg7 & 0x1f);
    arg1[1] = $v0_43;
    int32_t $v0_44 = fix_point_div_32(arg7, $a1_30, $fp_2);
    int32_t $v0_55 = subsection_map(
        (arg5[(fix_point_div_32(arg7, arg1[4] << (arg7 & 0x1f), $fp_2) + 0x200) >> (arg7 & 0x1f)]
            + arg5[($v0_44 + 0x200) >> (arg7 & 0x1f)]) / 2, 
        (fix_point_div_32(arg7, ($v0_3 + $v0_20) << (arg7 & 0x1f), $v0) + 0x200) >> (arg7 & 0x1f), 
        arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9);
    int32_t $a0_29 = 0;
    int32_t $v1_14;
    
    while (true)
    {
        int32_t $a1_36 = *var_3c_9;
        
        if ($v0_55 < $a1_36)
        {
            void* $s4_2 = &arg3[$a0_29];
            int32_t $v0_57 = fix_point_div_32(arg7, 
                (*$s4_2 - *(arg3 + ($a0_29 << 1) - 2)) << (arg7 & 0x1f), 
                ($a1_36 - *(arg4 + ($a0_29 << 1) - 2)) << (arg7 & 0x1f));
            int32_t $v0_59 =
                fix_point_mult2_32(arg7, $v0_57, ($a1_36 - $v0_55) << (arg7 & 0x1f)) + 0x200;
            $v1_14 = *$s4_2 - ($v0_59 >> (arg7 & 0x1f));
            break;
        }
        
        $a0_29 += 1;
        var_3c_10 = &var_3c_11[1];
        
        if ($a0_29 == 0x81)
        {
            $v1_14 = arg3[0x80];
            break;
        }
    }
    
    int32_t $v0_60 = arg1[4];
    arg1[3] = $v1_14;
    arg1[5] = $v0_60 + 1;
    arg1[7] = $v0_60 + 3;
    arg1[6] = $v0_60 + 2;
    
    if ($v0_60 + 3 >= 0xfff)
    {
        arg1[4] = 0xffb;
        arg1[5] = 0xffc;
        arg1[6] = 0xffd;
        arg1[7] = 0xffe;
    }
    
    if (arg1[8] >= 0x1000)
        arg1[8] = 0xfff;
    
    return 0xfff;
}

