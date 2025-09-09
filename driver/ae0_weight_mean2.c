#include "include/main.h"


  int32_t ae0_weight_mean2(void* arg1, void* arg2, void* arg3, void* arg4, void* arg5, void* arg6, int32_t arg7, void* arg8, void* arg9, void* arg10, void* arg11, void* arg12, void* arg13, int32_t arg14, int32_t arg15, int32_t* arg16, int32_t arg17, int32_t arg18, uint32_t* arg19, int32_t* arg20, int32_t* arg21, uint32_t* arg22, uint32_t* arg23)

{
    void* arg_0 = arg1;
    void* arg_4 = arg2;
    void* arg_8 = arg3;
    int32_t $a3;
    int32_t arg_c = $a3;
    int32_t $v1 = *(arg8 + 0x8c);
    int32_t $s2 = *arg16;
    int32_t $v1_2 = *(arg11 + 4);
    int32_t $v0_11 = *(arg8 + 0xc);
    int32_t $v0_12 = *(arg8 + 4);
    int32_t $v0_13 = *(arg8 + 0x88);
    int32_t var_e0_6 = 0;
    int32_t var_d8_1_1;
    __builtin_memset(&var_d8_1_2, 0, 0x24);
    int32_t var_60_10 = 0x60000;
    int32_t $s1 = 0;
    int32_t $s7 = 0;
    void* const var_58_18 = &data_20000_13;
    int32_t var_d4_1_1;
    int32_t var_d0_1_1;
    int32_t var_cc_1_1;
    int32_t var_c8_1_1;
    int32_t var_c4_1_5;
    int32_t var_c0_1_5;
    int32_t var_bc_1_5;
    int32_t var_b8_1_5;
    
    while ($s7 != $v0_12)
    {
        void* $t6_1 = arg8 + 0x10;
        int32_t* $t7_1 = arg9 + $s1;
        int32_t $t8_1 = 0;
        int32_t* $t0_1 = arg1 + $s1;
        int32_t* $v1_8 = arg2 + $s1;
        int32_t* $fp_1 = arg3 + $s1;
        uint32_t* $t5_1 = arg6 + $s1;
        int32_t* $t4_1 = arg13 + $s1;
        int32_t* $t3_1 = arg12 + $s1;
        int32_t* $t2_1 = arg5 + $s1;
        int32_t* $t1_1 = arg4 + $s1;
        
        while ($t8_1 != $v0_11)
        {
            int32_t $v0_30 = *(arg8 + ($s7 << 2) + 0x4c) * *$t6_1;
            int32_t $a0_2 = *$t0_1 + *$v1_8 + *$fp_1;
            uint32_t $lo_1 = $a0_2 / $v0_30;
            *$t5_1 = $lo_1;
            *arg21 += $lo_1;
            int32_t $a1_4 = *$t4_1;
            int32_t $lo_3;
            int32_t $hi_2;
            $hi_2 = HIGHD(COMBINE($a0_2 % $v0_30, var_c4_1_6) + $a0_2 * $a1_4);
            $lo_3 = LOWD(COMBINE($a0_2 % $v0_30, var_c4_1_7) + $a0_2 * $a1_4);
            int32_t $s0_2 = *$t2_1;
            var_c4_1_8 = $lo_3;
            int32_t $lo_5;
            int32_t $hi_3;
            $hi_3 = HIGHD(COMBINE($hi_2, var_c0_1_6) + $v0_30 * $a1_4);
            $lo_5 = LOWD(COMBINE($hi_2, var_c0_1_7) + $v0_30 * $a1_4);
            var_c0_1_8 = $lo_5;
            int32_t $a1_6 = *$t3_1;
            int32_t $lo_7;
            int32_t $hi_4;
            $hi_4 = HIGHD(COMBINE($hi_3, var_bc_1_6) + $a0_2 * $a1_6);
            $lo_7 = LOWD(COMBINE($hi_3, var_bc_1_7) + $a0_2 * $a1_6);
            var_bc_1_8 = $lo_7;
            int32_t $lo_9;
            int32_t $hi_5;
            $hi_5 = HIGHD(COMBINE($hi_4, var_b8_1_6) + $v0_30 * $a1_6);
            $lo_9 = LOWD(COMBINE($hi_4, var_b8_1_7) + $v0_30 * $a1_6);
            var_b8_1_8 = $lo_9;
            int32_t $t9_1 = $s0_2 * arg18;
            int32_t $a0_7 = *$t1_1;
            int32_t $a1_8 = $a0_7 * arg17;
            int32_t $s0_3 = $v0_30 - $s0_2 - $a0_7;
            int32_t $lo_11;
            int32_t $hi_7;
            $hi_7 = HIGHD(arg18 * *$fp_1 + arg17 * *$t0_1);
            $lo_11 = LOWD(arg18 * *$fp_1 + arg17 * *$t0_1);
            int32_t $s3_2 = $t9_1 + $a1_8 + $s0_3;
            int32_t $v0_34 = $lo_11 + *$v1_8;
            var_d8_1_3 += $t9_1;
            var_d4_1_2 += $a1_8;
            var_d0_1_2 += $s3_2;
            int32_t $v0_42 = 1;
            
            if (*(arg10 + 0x24) && $v1_2 != 1)
                $v0_42 = ((var_58_19 + 0xfd4)($s2, 
                    (var_60_11 - 0x75c)($s2, $a1_8, $s3_2, $v0_13 + 1, arg14, arg15)
                        + (var_60_12 - 0x75c)($s2, $s0_3, $s3_2, $v1 - 1 - $v0_13, arg14, arg15)
                        + (var_60_13 - 0x75c)($s2, $t9_1, $s3_2, 0x100 - $v1, arg14, arg15)) >> ($s2 & 0x1f)
                    >> 2) + 1;
            
            int32_t $a0_16 = *$t7_1;
            $t8_1 += 1;
            $t7_1 = &$t7_1[1];
            int32_t $lo_13;
            int32_t $hi_8;
            $hi_8 = HIGHD(COMBINE($hi_7, var_cc_1_2) + $v0_34 * $a0_16 * $v0_42);
            $lo_13 = LOWD(COMBINE($hi_7, var_cc_1_3) + $v0_34 * $a0_16 * $v0_42);
            $t6_1 += 4;
            var_cc_1_4 = $lo_13;
            int32_t $lo_15;
            int32_t $hi_9;
            $hi_9 = HIGHD(COMBINE($hi_8, var_c8_1_2) + $s3_2 * $a0_16 * $v0_42);
            $lo_15 = LOWD(COMBINE($hi_8, var_c8_1_3) + $s3_2 * $a0_16 * $v0_42);
            $t0_1 = &$t0_1[1];
            $v1_8 = &$v1_8[1];
            var_c8_1_4 = $lo_15;
            $fp_1 = &$fp_1[1];
            $t5_1 = &$t5_1[1];
            $t4_1 = &$t4_1[1];
            $t3_1 = &$t3_1[1];
            $t2_1 = &$t2_1[1];
            $t1_1 = &$t1_1[1];
        }
        
        $s7 += 1;
        $s1 += $v0_11 << 2;
    }
    
    __private_spin_lock_irqsave(0, &var_e0_7);
    memcpy(arg7, arg6, 0x384);
    private_spin_unlock_irqrestore(0, var_e0_8);
    *arg22 = var_c4_1_9 / var_c0_1_9;
    *arg23 = var_bc_1_9 / var_b8_1_9;
    uint32_t $v0_66 = var_cc_1_5 / var_c8_1_5;
    
    if (!$v0_66)
        $v0_66 = 1;
    
    *arg19 = $v0_66;
    int32_t $s0_5 = var_d0_1_3 << ($s2 & 0x1f);
    *arg20 = fix_point_div_32($s2, var_d8_1_4 << ($s2 & 0x1f), $s0_5);
    int32_t result = fix_point_div_32($s2, var_d4_1_3 << ($s2 & 0x1f), $s0_5);
    arg20[1] = result;
    return result;
}

