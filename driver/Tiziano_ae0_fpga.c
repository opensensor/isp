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
    uint32_t var_54 = 0;
    int32_t $a2 = *(arg12 + 0xc);
    int32_t var_60 = 0;
    int32_t $v1_2 = *(arg7 + 0xc);
    int32_t var_68 = 0;
    int32_t $v1_3 = *(arg7 + 0x4c);
    int32_t var_64 = 0;
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
    
    int32_t var_5c_3;
    int32_t* var_c8_3 = &var_5c_4;
    int32_t var_58_9;
    int32_t* var_cc_4 = &var_58_10;
    int32_t* var_d0_8 = &var_60_5;
    int32_t* var_d4_7 = &var_68_9;
    int32_t* var_d8 = &var_54_8;
    int32_t var_dc = $a0;
    int32_t var_e0_3 = $a1;
    char var_118_6[0x38];
    
    for (int32_t i = 0; (uintptr_t)i < 0x38; i += 1)
    {
        char arg_10[0x8];
        var_118[i] = arg_10[i];
    }
    
    ae0_weight_mean2(arg1, arg2, arg3, arg4);
    int32_t $v0_3 = $s1 + 1;
    
    if (data_b0e10_1 == 1)
    {
        tisp_wdr_rx_ae0_infm(arg5, arg6);
        $v0_3 = $s1 + 1;
    }
    
    int32_t* $s1_2 = arg5 + ($s1 << 2);
    int32_t $a3_2 = 0;
    
    while (true)
    {
        $s1_2 = &$s1_2[1];
        
        if ($(uintptr_t)v0_3 >= 0x100)
            break;
        
        $v0_3 += 1;
        $a3_2 += *$s1_2;
    }
    
    char* $a0_8 = (char*)(arg5); // Fixed void pointer assignment
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
        
        if ($(uintptr_t)s5 < 0x21)
            $s1_3 = $s5;
    }
    
    int32_t $v0_16 = var_5c_5;
    
    if ($s4 == 1)
    {
        char* $s1_5 = (char*)(arg5 + (i_4 << 2)); // Fixed void pointer assignment
        uint32_t i_1 = i_4;
        int32_t $v1_4 = 0;
            int32_t $v0_9 =
            int32_t $v0_11 = fix_point_mult2_32(0x10, ($s1_3 - 1) << 0x10, 
        if ((uintptr_t)i_4 < 0xff)
            i_4 = (i_4 << 1) / 3;
        
        
        while ((uintptr_t)i_1 < 0x100)
        {
                (&data_20000 + 0x1044)(0x10, (i_1 - i_4) << 0x10, (0x100 - i_4) << 0x10);
                fix_point_mult2_32(0x10, $v0_9, $v0_9));
            $v1_4 += fix_point_mult2_32(5, *$s1_5 << 5, $v0_11 >> 0xb) >> 5;
            i_1 += 1;
            $s1_5 += 4;
        }
        
        int32_t $s5_1 = $v1_2 * $fp * $s6 * $v1_3;
        var_54_9 = ($v1_4 + $s5_1) * var_54_10 / $s5_1;
        $v0_16 = var_5c_6;
    }
    
    int32_t var_7c = $t1_1;
    int32_t var_70_10 = $v0_16;
    int32_t var_84_3 = $a3_2;
    int32_t var_74_3 = var_58_11;
    int32_t var_78_19 = $v1_1;
    int32_t var_80_6 = $v1;
    int32_t* var_88_7 = &var_68_10;
    uint32_t var_8c_8 = var_54_11;
    
    for (int32_t i_2 = 0; (uintptr_t)i_2 < 0x40; i_2 += 1)
    {
        void arg_a4;
        *(&var_cc + i_2) = *(&arg_a4 + i_2);
    }
    
    for (int32_t i_3 = 0; (uintptr_t)i_3 < 0x4c; i_3 += 1)
    {
        char arg_58[0xc];
        var_118[i_3] = arg_58[i_3];
    }
    
    return ae0_tune2(arg8, arg9, arg10, arg11);
}

