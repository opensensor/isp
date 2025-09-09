#include "include/main.h"


  uint32_t* Tiziano_ae0_fpga(void* arg1, void* arg2, void* arg3, void* arg4, void* arg5, int32_t arg6, void* arg7, int32_t* arg8, int32_t arg9, void* arg10, void* arg11, void* arg12, int32_t* arg13)

{
    int32_t $v1 = arg13[2];
    int32_t $s4 = *arg13;
    int32_t $s1 = arg13[1];
    int32_t $s7 = arg13[3];
    int32_t $v1_1 = arg13[4];
    int32_t $a1 = arg13[5];
    int32_t $s5 = arg13[6];
    int32_t $t4 = arg13[7];
    int32_t $a0 = arg13[8];
    int32_t $a3 = arg13[9];
    int32_t $t1 = arg13[0xa];
    uint32_t var_54_11 = 0;
    int32_t $a2 = *(arg12 + 0xc);
    int32_t var_60_14 = 0;
    int32_t $v1_2 = *(arg7 + 0xc);
    int32_t var_68_16 = 0;
    int32_t $v1_3 = *(arg7 + 0x4c);
    int32_t var_64_16 = 0;
    int32_t $fp = *(arg7 + 4);
    int32_t $s6 = *(arg7 + 0x10);
    uint32_t i_4 = *(arg7 + 0x8c);
    
    if (!$s4)
    {
        $a0 = 1;
        $a1 = 1;
    }
    else if ($s4 == 1)
        $a0 = $s5;
    else if ($a2 == 2)
        $a0 = $s5;
    else
    {
        $a1 = $t4;
        
        if ($a2 != 1)
        {
            $a0 = $t1;
            $a1 = $a3;
            
            if ($a2)
                $a0 = 0;
            
            if ($a2)
                $a1 = 0;
        }
    }
    
    int32_t var_5c_5;
    int32_t* var_c8_12 = &var_5c_6;
    int32_t var_58_20;
    int32_t* var_cc_7 = &var_58_21;
    int32_t* var_d0_16 = &var_60_15;
    int32_t* var_d4_15 = &var_68_17;
    int32_t* var_d8_3 = &var_54_12;
    int32_t var_dc_6 = $a0;
    int32_t var_e0_14 = $a1;
    char var_118_15[0x38];
    
    for (int32_t i = 0; i < 0x38; i += 1)
    {
        char arg_10[0x8];
        var_118_16[i] = arg_10[i];
    }
    
    ae0_weight_mean2(arg1, arg2, arg3, arg4);
    int32_t $v0_3 = $s1 + 1;
    
    if (data_b0e10_2 == 1)
    {
        tisp_wdr_rx_ae0_infm(arg5, arg6);
        $v0_3 = $s1 + 1;
    }
    
    int32_t* $s1_2 = arg5 + ($s1 << 2);
    int32_t $a3_2 = 0;
    
    while (true)
    {
        $s1_2 = &$s1_2[1];
        
        if ($v0_3 >= 0x100)
            break;
        
        $v0_3 += 1;
        $a3_2 += *$s1_2;
    }
    
    void* $a0_8 = arg5;
    int32_t $v0_4 = 0;
    int32_t $t1_1 = 0;
    
    while (true)
    {
        int32_t temp1_1 = $v0_4;
        $v0_4 += 1;
        
        if (temp1_1 == $s7)
            break;
        
        $t1_1 += *$a0_8;
        $a0_8 += 4;
    }
    
    int32_t $s1_3 = 1;
    
    if ($s5)
    {
        $s1_3 = 0x20;
        
        if ($s5 < 0x21)
            $s1_3 = $s5;
    }
    
    int32_t $v0_16 = var_5c_7;
    
    if ($s4 == 1)
    {
        if (i_4 < 0xff)
            i_4 = (i_4 << 1) / 3;
        
        void* $s1_5 = arg5 + (i_4 << 2);
        uint32_t i_1 = i_4;
        int32_t $v1_4 = 0;
        
        while (i_1 < 0x100)
        {
            int32_t $v0_9 =
                (&data_20000_15 + 0x1044)(0x10, (i_1 - i_4) << 0x10, (0x100 - i_4) << 0x10);
            int32_t $v0_11 = fix_point_mult2_32(0x10, ($s1_3 - 1) << 0x10, 
                fix_point_mult2_32(0x10, $v0_9, $v0_9));
            $v1_4 += fix_point_mult2_32(5, *$s1_5 << 5, $v0_11 >> 0xb) >> 5;
            i_1 += 1;
            $s1_5 += 4;
        }
        
        int32_t $s5_1 = $v1_2 * $fp * $s6 * $v1_3;
        var_54_13 = ($v1_4 + $s5_1) * var_54_14 / $s5_1;
        $v0_16 = var_5c_8;
    }
    
    int32_t var_7c_5 = $t1_1;
    int32_t var_70_18 = $v0_16;
    int32_t var_84_10 = $a3_2;
    int32_t var_74_12 = var_58_22;
    int32_t var_78_34 = $v1_1;
    int32_t var_80_18 = $v1;
    int32_t* var_88_16 = &var_68_18;
    uint32_t var_8c_17 = var_54_15;
    
    for (int32_t i_2 = 0; i_2 < 0x40; i_2 += 1)
    {
        void arg_a4;
        *(&var_cc_8 + i_2) = *(&arg_a4 + i_2);
    }
    
    for (int32_t i_3 = 0; i_3 < 0x4c; i_3 += 1)
    {
        char arg_58[0xc];
        var_118_17[i_3] = arg_58[i_3];
    }
    
    return ae0_tune2(arg8, arg9, arg10, arg11);
}

