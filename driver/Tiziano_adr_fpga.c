#include "include/main.h"


  int32_t Tiziano_adr_fpga(int32_t* arg1, int32_t* arg2, int32_t* arg3, int32_t* arg4, int32_t* arg5, int32_t* arg6, int32_t* arg7, int32_t* arg8, void* arg9, int32_t* arg10, int32_t arg11, int32_t arg12, int32_t* arg13, int32_t* arg14, int32_t* arg15, int32_t* arg16)

{
    int32_t* arg_4 = arg2;
    int32_t* arg_0 = arg1;
    int32_t* arg_8 = arg3;
    int32_t* var_c4 = arg3;
    int32_t* arg_c = arg4;
    void var_2b0;
    void var_2d4;
    int32_t $v0_3 = fix_point_div_32(0x10, 0x385b0000, 0x27100000);
    int32_t $v0_4 = *arg16;
    int32_t $v1_1 = arg16[0xa];
    int32_t $v0_5 = arg16[1];
    int32_t var_128 = arg16[2];
    int32_t $v1_2 = arg16[0xb];
    int32_t $v0_7 = arg16[3];
    int32_t var_300 = *arg15;
    int32_t var_11c = $v0_7;
    int32_t $v0_8 = arg16[4];
    int32_t $v1_3 = arg16[0xc];
    int32_t var_2fc = arg15[1];
    int32_t $v0_9 = arg16[7];
    int32_t $v1_4 = arg16[0xd];
    int32_t var_2f8 = arg15[2];
    int32_t $s1 = arg16[6];
    int32_t $a1 = arg16[9];
    int32_t $v0_10 = arg16[8];
    int32_t $t2 = arg16[0xe];
    int32_t var_2f4 = arg15[3];
    int32_t $v1_5 = arg15[0x12];
    int32_t $v1_6 = arg15[0x13];
    int32_t var_2f0 = arg15[4];
    int32_t $v1_7 = arg15[0x14];
    int32_t var_2ec = arg15[5];
    int32_t $v1_8 = arg15[0x15];
    int32_t var_2e8 = arg15[6];
    int32_t $v1_9 = arg15[0x16];
    int32_t var_2e4 = arg15[7];
    int32_t $v1_10 = arg15[0x17];
    int32_t var_2e0 = arg15[8];
    int32_t $v1_11 = arg15[0x18];
    int32_t var_2dc = arg15[9];
    int32_t $v1_12 = arg15[0x19];
    int32_t var_2d8 = arg15[0xa];
    int32_t $s6_1 = arg15[0xe];
    int32_t $t0 = arg15[0x1a];
    int32_t $v1_13 = arg15[0x1c];
    int32_t var_148 = 0;
    int32_t $a0_13 = *arg14;
    int32_t $a0_14 = arg14[1];
    int32_t $a0_15 = arg14[2];
    int32_t $a0_16 = arg14[3];
    int32_t $a0_17 = arg14[4];
    int32_t var_124_1 = arg14[5];
    int32_t $a0_19 = arg14[6];
    int32_t $a0_20 = arg14[7];
    int32_t $a0_21 = arg14[8];
    int32_t $a0_22 = arg14[9];
    int32_t $v1_15 = arg14[0xa];
    int32_t var_144;
    int32_t var_1d8;
    int32_t var_190;
    int32_t var_16c;
        int32_t $a0_25 = *(arg10 + i);
        void var_220;
        char* $a3_4 = (char*)(&var_1d8 + i); // Fixed void pointer assignment
    memcpy(&var_2b0, 0x7b1f0, 0x24);
    memcpy(&var_2d4, 0x7b214, 0x24);
    int32_t (* var_118)(int32_t arg1, int32_t arg2, int32_t arg3) = fix_point_div_32;
    __builtin_memcpy(&var_144, 
        "\x1e\x00\x00\x00\x28\x00\x00\x00\x32\x00\x00\x00\x3c\x00\x00\x00\x46\x00\x00\x00\x50\x00\x00\x"
    "00\x64\x00\x00\x00", 
        0x1c);
    
    for (int32_t i = 0; (uintptr_t)i != 0x24; )
    {
        *(&var_220 + i) = $a0_25;
        *(&var_16c + i) = $a0_25;
        *(&var_190 + i) = $a0_25;
        i += 4;
        *$a3_4 = $a0_25;
    }
    
    int32_t $v0_12 = $v0_10 * $a1 / 4;
    
    if ($(uintptr_t)v0_12 < 0x1400)
        $v0_12 = 0x1400;
    
    int32_t* i_1 = arg8;
    int32_t $v1_16 = 0;
    
    do
    {
        int32_t $t3_1 = *i_1;
    int32_t $lo_2 = $v1_16 / $v0_12;
    int32_t* $a0_27 = &adr_hist_512;
    int32_t $a2_1 = 0;
    int32_t* i_2 = arg7;
        int32_t $t3_2 = *i_2;
    void* $t1_4;
        char* $t1_3 = (char*)(&data_a80c0 + i_3); // Fixed void pointer assignment
        int32_t $lo_3 = (*(&adr_hist_512 + i_3) << 8) / ($v0_12 >> 8);
        i_1 = &i_1[1];
        $v1_16 += $t3_1;
    } while (i_1 != &arg8[0x30]);
    
    arg15[0xb] = $lo_2;
    
    do
    {
        i_2 = &i_2[1];
        $a2_1 += $t3_2;
        
        if ($a2_1 >= $v0_12)
            $a2_1 = $v0_12;
        
        *$a0_27 = $a2_1;
        $a0_27 = &$a0_27[1];
    } while (&arg7[0x200] != i_2);
    
    
    for (int32_t i_3 = 0; (uintptr_t)i_3 != 0x800; )
    {
        i_3 += 4;
        *$t1_3 = $lo_3 * 0x2710 / isp_printf;
        $t1_4 = &data_a80c0;
    }
    
    char* $s7_1 = (char*)(arg9 + 0x78); // Fixed void pointer assignment
    
    for (int32_t i_4 = 0; (uintptr_t)i_4 != 0x1e0; )
    {
        char* $v1_18 = (char*)(&adr_block_hist_120 + i_4); // Fixed void pointer assignment
        char* $v0_19 = (char*)($s7_1); // Fixed void pointer assignment
            int32_t $t5_3 = *($v0_19 + 0x1c);
        
        for (int32_t j = 1; j != 5; )
        {
            j += 1;
            *((int32_t*)$v1_18) = *($v0_19 + 0x14); // Fixed void pointer dereference
            $v1_18 += 0x14;
            *($v1_18 - 0x10) = *($v0_19 + 0x18);
            $v0_19 += 0x14;
            *($v1_18 - 0xc) = $t5_3;
            *($v1_18 - 8) = *($v0_19 + 0xc);
            *($v1_18 - 4) = *($v0_19 + 0x10);
        }
        
        i_4 += 0x50;
        $s7_1 += 0x78;
    }
    
    char* $a3_6 = (char*)(&arg8[6]); // Fixed void pointer assignment
    int32_t* $v0_23;
    
    for (int32_t i_5 = 0; (uintptr_t)i_5 != 0x60; )
    {
            int32_t $t6_1 = *($a3_6 + j_1 + 4);
            char* $v0_22 = (char*)(&adr_block_y_24 + i_5 + j_1); // Fixed void pointer assignment
        for (int32_t j_1 = 0; (uintptr_t)j_1 != 0x10; )
        {
            j_1 += 4;
            *$v0_22 = $t6_1;
            $v0_23 = &adr_block_y_24;
        }
        
        i_5 += 0x10;
        $a3_6 += 0x18;
    }
    
    if ($t2 == 1)
    {
        adr_block_y_24 = adr_block_y_24 + *arg8 + arg8[1] + arg8[6];
        data_b7804 += arg8[2];
        data_b7808 += arg8[3];
        data_b780c = data_b780c + arg8[4] + arg8[5] + arg8[0xb];
        data_b7810 += arg8[0xc];
        data_b781c += arg8[0x11];
        data_b7820 += arg8[0x12];
        data_b782c += arg8[0x17];
        data_b7830 += arg8[0x18];
        data_b783c += arg8[0x1d];
        data_b7840 += arg8[0x1e];
        data_b784c += arg8[0x23];
        data_b7850 = data_b7850 + arg8[0x24] + arg8[0x2a] + arg8[0x2b];
        data_b7854 += arg8[0x2c];
        data_b7858 += arg8[0x2d];
        data_b785c = data_b785c + arg8[0x29] + arg8[0x2e] + arg8[0x2f];
    }
    
    char* var_118_1 = (char*)(&block_mean_y); // Fixed void pointer assignment
    char* $t3_4 = (char*)(&block_mean_y); // Fixed void pointer assignment
    int32_t i_20 = 0;
    int32_t i_21 = 0;
    int32_t var_114_1 = 0xfff;
    int32_t $a0_52 = 0;
    int32_t $t6_2;
    
    for (int32_t i_6 = 0; (uintptr_t)i_6 != 0x18; )
    {
        int32_t $lo_5 = *$v0_23 / ($v0_12 / 0x18);
        int32_t $t4_1 = $a0_52 < $lo_5 ? 1 : 0;
        int32_t $t4_3 = $lo_5 < var_114_1 ? 1 : 0;
        $t6_2 = var_114_1;
        $v0_23 = &$v0_23[1];
        $t3_4 += 4;
        
        if ($t4_1)
            i_21 = i_6;
        
        if ($t4_1)
            $a0_52 = $lo_5;
        
        *($t3_4 - 4) = $lo_5;
        
        if ($t4_3)
            i_20 = i_6;
        
        if ($t4_3)
            $t6_2 = $lo_5;
        
        i_6 += 1;
        var_114_1 = $t6_2;
    }
    
    int32_t var_420;
    int32_t* $s4_1 = &var_420_1;
    arg15[0xc] = $a0_52 * 0x3e8 + i_21;
    int32_t i_7 = 0;
    arg15[0xd] = $t6_2 * 0x3e8 + i_20;
    
    do
    {
        int32_t var_1fc;
        int32_t var_1f8;
        int32_t var_1f4;
        int32_t var_1f0;
        int32_t var_1ec;
        int32_t var_1e8;
        int32_t var_1e4;
        int32_t var_1e0;
        int32_t var_1dc;
    int32_t var_41c;
    int32_t var_418;
    int32_t var_414;
    int32_t var_410;
    int32_t var_40c;
    int32_t var_408;
    int32_t var_404;
    int32_t var_400;
    int32_t $v0_171;
    int32_t var_1b4;
    int32_t var_1b0;
    int32_t var_1ac;
    int32_t var_1a8;
    int32_t var_1a4;
    int32_t var_1a0;
    int32_t var_19c;
    int32_t var_198;
    int32_t var_194;
    int32_t var_168;
    int32_t var_164;
    int32_t var_160;
    int32_t var_15c;
    int32_t var_14c;
        int32_t* var_e0_1 = &var_148;
        int32_t* $v1_100 = &var_148;
        int32_t i_9 = 0;
        int32_t $v0_172;
            int32_t $t2_2 = *$v1_100;
            int32_t $t0_2 = i_9 * 9;
                    char* $a1_80 = (char*)(&var_16c + i_8); // Fixed void pointer assignment
                    int32_t $v1_103 = *(&var_420 + ($t0_2 << 2) + i_8);
        (&data_30000 - 0x427c)(&var_1fc, *(&var_148 + i_7), arg11, arg12, $t1_4, 8, 0xa, 0x10, $t0);
        i_7 += 4;
        *$s4_1 = var_1fc;
        $s4_1 = &$s4_1[9];
        *($s4_1 - 0x20) = var_1f8;
        *($s4_1 - 0x1c) = var_1f4;
        *($s4_1 - 0x18) = var_1f0;
        *($s4_1 - 0x14) = var_1ec;
        *($s4_1 - 0x10) = var_1e8;
        *($s4_1 - 0xc) = var_1e4;
        *($s4_1 - 8) = var_1e0;
        *($s4_1 - 4) = var_1dc;
    } while ((uintptr_t)i_7 != 0x20);
    
    
    if ($s6_1 != 1)
    {
        
        while (true)
        {
            
            if ($s1 == $t2_2)
            {
                for (int32_t i_8 = 0; (uintptr_t)i_8 != 0x24; )
                {
                    i_8 += 4;
                    *$a1_80 = $v1_103;
                }
                
                $v0_172 = var_16c;
                break;
            }
            
            if ($s1 < $t2_2)
            {
                    int32_t $v0_182;
                    int32_t $t0_5;
                    int32_t $t1_5;
                    int32_t $t2_3;
                    int32_t $t3_9;
                    int32_t $v0_189;
                    int32_t $t0_6;
                    int32_t $t1_6;
                    int32_t $t2_4;
                    int32_t $t3_10;
                    int32_t $v0_196;
                    int32_t $t0_7;
                    int32_t $t1_7;
                    int32_t $t2_5;
                    int32_t $t3_11;
                if (i_9)
                {
                    $v0_182 = interpolate_adr_x8_y12((&var_420)[i_9 + 0xb5], $t2_2, 
                        (&var_420)[$t0_2 - 8], (&var_420)[$t0_2 + 1], $s1);
                    var_168 = $v0_182;
                    $v0_189 =
                        $t1_5($t3_9, $t2_3, (&var_420)[$t0_5 - 7], (&var_420)[$t0_5 + 2], $s1);
                    var_164 = $v0_189;
                    $v0_196 =
                        $t1_6($t3_10, $t2_4, (&var_420)[$t0_6 - 6], (&var_420)[$t0_6 + 3], $s1);
                    var_160 = $v0_196;
                    var_15c =
                        $t1_7($t3_11, $t2_5, (&var_420)[$t0_7 - 5], (&var_420)[$t0_7 + 4], $s1);
                    $v0_172 = var_16c;
                }
                else
                {
                        char* $a1_82 = (char*)(&var_16c + i_9); // Fixed void pointer assignment
                        int32_t $v1_106 = *(&var_420 + ($t0_2 << 2) + i_9);
                    do
                    {
                        i_9 += 4;
                        *$a1_82 = $v1_106;
                    } while ((uintptr_t)i_9 != 0x24);
                    
                    $v0_172 = var_16c;
                }
                
                break;
            }
            
            i_9 += 1;
            $v1_100 = &$v1_100[1];
            
            if (i_9 == 8)
            {
                    int32_t $a1_88 = *(&var_420 + i_10 + 0x120);
                    char* $a0_88 = (char*)(&var_16c + i_10); // Fixed void pointer assignment
                for (int32_t i_10 = 0; (uintptr_t)i_10 != 0x24; )
                {
                    i_10 += 4;
                    *$a0_88 = $a1_88;
                }
                
                $v0_172 = var_16c_1;
                break;
            }
        }
        
        if ($v0_172 <= 0)
            var_16c_2 = 0;
        
        int32_t* $v0_455 = &var_16c_3;
        int32_t $v1_200 = var_16c_4;
        
        while (true)
        {
            if ($v1_200 < $v0_455[1])
                $v0_455 = &$v0_455[1];
            else
            {
                $v0_455[1] = $v1_200 + 1;
                $v0_455 = &$v0_455[1];
            }
            
            if ($v0_455 == &var_14c)
                break;
            
            $v1_200 = *$v0_455;
        }
        
        int32_t $v0_456 = var_16c_5;
        int32_t $a0_128 = arg10[8];
        int32_t $v0_461 = var_14c_1;
        int32_t $a1_142 = var_168 - $v0_456;
        int32_t* i_11 = arg4;
        int32_t $v0_463 = *arg10;
        int32_t $v1_202 = arg10[1];
        int32_t $a2_108 = arg10[2];
        int32_t $s4_6 = $v0_463 + $a1_142;
        
        if ($v1_202 - $v0_463 >= $a1_142)
            $s4_6 = $v1_202;
        
        int32_t $s2_31 = $a2_108 + $s4_6 - $v1_202;
        int32_t $s1_9 = arg10[3];
        int32_t $a0_133 = var_164 - var_168_1;
        
        if ($s2_31 - $s4_6 < $a0_133)
            $s2_31 = $a0_133 + $s4_6;
        
        int32_t $s3_11 = $s1_9 + $s2_31 - $a2_108;
        int32_t $v1_207 = var_160 - var_164_1;
        
        if ($s3_11 - $s2_31 < $v1_207)
            $s3_11 = $v1_207 + $s2_31;
        
        int32_t $s1_10 = arg10[4] + $s3_11 - $s1_9;
        int32_t $v0_468 = var_15c - var_160_1;
        
        if ($s1_10 - $s3_11 < $v0_468)
            $s1_10 = $v0_468 + $s3_11;
        
        if ($(uintptr_t)s1_10 >= 0xfff)
            $s1_10 = 0xfff;
        
        int32_t* $v0_477 = var_c4;
        
        do
        {
            int32_t $a2_122 = *$v0_477;
            int32_t $v0_493;
        int32_t $v0_523 = 0;
        int32_t var_18c;
            int32_t $s1_11 = *var_e0_1;
            int32_t $v0_528;
                int32_t $v0_524 = $v0_523 * 9;
                int32_t var_188_1 = (&var_420)[$v0_524 + 2];
                int32_t var_184_1 = (&var_420)[$v0_524 + 3];
                int32_t var_180_1 = (&var_420)[$v0_524 + 4];
                int32_t var_17c_1 = (&var_420)[$v0_524 + 5];
                int32_t var_178_1 = (&var_420)[$v0_524 + 6];
                int32_t var_174_1 = (&var_420)[$v0_524 + 7];
            
            if ($a2_122 < $v0_456)
                $v0_493 = $v0_463 - ((fix_point_mult2_32(0xa, 
                    fix_point_div_32(0xa, $v0_463 << 0xa, $v0_456 << 0xa), 
                    ($v0_456 - $a2_122) << 0xa) + 0x200) >> 0xa);
            else if ($a2_122 < var_168)
                $v0_493 = $s4_6 - ((fix_point_mult2_32(0xa, 
                    fix_point_div_32(0xa, ($s4_6 - $v0_463) << 0xa, $a1_142 << 0xa), 
                    (var_168 - $a2_122) << 0xa) + 0x200) >> 0xa);
            else if ($a2_122 < var_164)
                $v0_493 = $s2_31 - ((fix_point_mult2_32(0xa, 
                    fix_point_div_32(0xa, ($s2_31 - $s4_6) << 0xa, $a0_133 << 0xa), 
                    (var_164 - $a2_122) << 0xa) + 0x200) >> 0xa);
            else if ($a2_122 < var_160)
                $v0_493 = $s3_11 - ((fix_point_mult2_32(0xa, 
                    fix_point_div_32(0xa, ($s3_11 - $s2_31) << 0xa, $v1_207 << 0xa), 
                    (var_160 - $a2_122) << 0xa) + 0x200) >> 0xa);
            else if ($a2_122 < var_15c)
                $v0_493 = $s1_10 - ((fix_point_mult2_32(0xa, 
                    fix_point_div_32(0xa, ($s1_10 - $s3_11) << 0xa, $v0_468 << 0xa), 
                    (var_15c - $a2_122) << 0xa) + 0x200) >> 0xa);
            else if ($a2_122 >= $v0_461)
                $v0_493 = $a0_128;
            else
                $v0_493 = $a0_128 - ((fix_point_mult2_32(0xa, 
                    fix_point_div_32(0xa, ($a0_128 - $s1_10) << 0xa, ($v0_461 - var_15c) << 0xa), 
                    ($v0_461 - $a2_122) << 0xa) + 0x200) >> 0xa);
            
            *i_11 = $v0_493;
            i_11 = &i_11[1];
            var_c4 = &var_c4[1];
            $v0_477 = var_c4;
        } while (&arg4[0xb] != i_11);
        
        
        while (true)
        {
            
            if ($v0_9 == $s1_11)
            {
                var_190 = (&var_420)[$v0_524];
                var_18c = (&var_420)[$v0_524 + 1];
                $v0_528 = (&var_420)[$v0_524 + 8];
            }
            else if ($v0_9 >= $s1_11)
            {
                $v0_523 += 1;
                var_e0_1 = &var_e0_1[1];
                
                if ($v0_523 == 8)
                    break;
                
                continue;
            }
            else
            {
                    int32_t $s4_8 = $v0_523 * 9;
                    int32_t $s2_32 = (&var_420)[$v0_523 + 0xb5];
                    int32_t $a1_158 = (&var_420)[$s4_8 + 1];
                    int32_t $s5_5 = (&var_420)[$s4_8 - 8];
                    int32_t $a1_159;
                    int32_t $s7_2;
                if ($v0_523)
                {
                    
                    if ($s5_5 >= $a1_158)
                    {
                        $a1_159 = $s5_5 - $a1_158;
                        $s7_2 = 0;
                    }
                    else
                    {
                        $a1_159 = $a1_158 - $s5_5;
                        $s7_2 = 1;
                    }
                    
                    int32_t $s1_13 = ($s1_11 - $s2_32) << 0xa;
                    int32_t $s2_34 = ($v0_9 - $s2_32) << 0xa;
                    int32_t $v0_551 = (fix_point_mult2_32(0xa, 
                        fix_point_div_32(0xa, $a1_159 << 0xa, $s1_13), $s2_34) + 0x200) >> 0xa;
                    int32_t $s5_6;
                    
                    $s5_6 = $s7_2 ? $s5_5 + $v0_551 : $s5_5 - $v0_551;
                    
                    int32_t $a1_162 = (&var_420_2)[$s4_8 + 2];
                    var_18c = $s5_6;
                    int32_t $s5_7 = (&var_420_3)[$s4_8 - 7];
                    int32_t $a1_163;
                    int32_t $s7_3;
                    
                    if ($s5_7 >= $a1_162)
                    {
                        $a1_163 = $s5_7 - $a1_162;
                        $s7_3 = 0;
                    }
                    else
                    {
                        $a1_163 = $a1_162 - $s5_7;
                        $s7_3 = 1;
                    }
                    
                    int32_t $v0_562 = (fix_point_mult2_32(0xa, 
                        fix_point_div_32(0xa, $a1_163 << 0xa, $s1_13), $s2_34) + 0x200) >> 0xa;
                    int32_t $s5_8;
                    
                    $s5_8 = $s7_3 ? $s5_7 + $v0_562 : $s5_7 - $v0_562;
                    
                    int32_t $a1_166 = (&var_420_4)[$s4_8 + 3];
                    int32_t var_188_3 = $s5_8;
                    int32_t $s5_9 = (&var_420_5)[$s4_8 - 6];
                    int32_t $a1_167;
                    int32_t $s7_4;
                    
                    if ($s5_9 >= $a1_166)
                    {
                        $a1_167 = $s5_9 - $a1_166;
                        $s7_4 = 0;
                    }
                    else
                    {
                        $a1_167 = $a1_166 - $s5_9;
                        $s7_4 = 1;
                    }
                    
                    int32_t $v0_573 = (fix_point_mult2_32(0xa, 
                        fix_point_div_32(0xa, $a1_167 << 0xa, $s1_13), $s2_34) + 0x200) >> 0xa;
                    int32_t $s5_10;
                    
                    $s5_10 = $s7_4 ? $s5_9 + $v0_573 : $s5_9 - $v0_573;
                    
                    int32_t $a1_170 = (&var_420_6)[$s4_8 + 4];
                    int32_t $s4_12 = (&var_420_7)[$s4_8 - 5];
                    int32_t var_184_3 = $s5_10;
                    int32_t $a1_171;
                    int32_t $s5_11;
                    
                    if ($s4_12 >= $a1_170)
                    {
                        $a1_171 = $s4_12 - $a1_170;
                        $s5_11 = 0;
                    }
                    else
                    {
                        $a1_171 = $a1_170 - $s4_12;
                        $s5_11 = 1;
                    }
                    
                    int32_t $v0_581 = (fix_point_mult2_32(0xa, 
                        fix_point_div_32(0xa, $a1_171 << 0xa, $s1_13), $s2_34) + 0x200) >> 0xa;
                    int32_t $s4_13;
                    
                    $s4_13 = $s5_11 ? $s4_12 + $v0_581 : $s4_12 - $v0_581;
                    
                    int32_t var_180_3 = $s4_13;
                    break;
                }
                
                var_190 = var_420_8;
                var_18c_1 = var_41c;
                int32_t var_188_2 = var_418;
                int32_t var_184_2 = var_414;
                int32_t var_180_2 = var_410_3;
                int32_t var_17c_2 = var_40c;
                int32_t var_178_2 = var_408;
                int32_t var_174_2 = var_404;
                $v0_528 = var_400;
            }
            
            int32_t var_170_1 = $v0_528;
            break;
        }
        
        int32_t* $v0_584 = &var_190_1;
        
        if (var_18c_2 < 2)
        {
            int32_t var_18c_1 = 1;
            $v0_584 = &var_190;
        }
        
        int32_t* $a1_174 = &var_1b4;
        
        for (int32_t i_12 = 0; i_12 != 9; )
        {
            int32_t $a0_143 = *$v0_584;
            
            if (i_12 == 8)
                i_12 += 1;
            else if ($a0_143 < $v0_584[1])
                i_12 += 1;
            else
            {
                $v0_584[1] = $a0_143 + 1;
                $a0_143 = *$v0_584;
                i_12 += 1;
            }
            
            *$a1_174 = $a0_143;
            $v0_584 = &$v0_584[1];
            $a1_174 = &$a1_174[1];
        }
        
        int32_t* $s1_14 = arg2;
        int32_t* $s6_4 = arg1;
        int32_t* i_13 = arg13;
        
        do
        {
            int32_t $v1_257 = var_1b4;
            int32_t $s7_5 = *$s6_4;
            int32_t $v0_591;
                int32_t $a1_175 = *arg13;
            *$s1_14 = *i_13;
            
            if ($s7_5 < $v1_257)
            {
                
                if ($a1_175 >= 0)
                {
                    $v0_591 = (fix_point_mult2_32(0xa, 
                        fix_point_div_32(0xa, $a1_175 << 0xa, $v1_257 << 0xa), 
                        ($v1_257 - $s7_5) << 0xa) + 0x200) >> 0xa;
                    *$s1_14 = *arg13 - $v0_591;
                }
            }
            else if ($s7_5 < var_1b0)
            {
                int32_t $a1_178 = arg13[1];
                int32_t $v0_593 = *arg13;
                int32_t $a0_144 = $a1_178 - $v0_593;
                int32_t $a2_149 = (var_1b0 - $v1_257) << 0xa;
                int32_t $s2_37 = (var_1b0 - $s7_5) << 0xa;
                
                if ($a0_144 < 0)
                    *$s1_14 = ((fix_point_mult2_32(0xa, 
                        fix_point_div_32(0xa, ($v0_593 - $a1_178) << 0xa, $a2_149), $s2_37) + 0x200)
                        >> 0xa) + arg13[1];
                else
                {
                    $v0_591 = (fix_point_mult2_32(0xa, 
                        fix_point_div_32(0xa, $a0_144 << 0xa, $a2_149), $s2_37) + 0x200) >> 0xa;
                    *$s1_14 = arg13[1] - $v0_591;
                }
            }
            else if ($s7_5 < var_1ac)
            {
                int32_t $a1_184 = arg13[2];
                int32_t $v0_603 = arg13[1];
                int32_t $a0_145 = $a1_184 - $v0_603;
                int32_t $a2_153 = (var_1ac - var_1b0) << 0xa;
                int32_t $s2_39 = (var_1ac - $s7_5) << 0xa;
                    int32_t $v0_606 = fix_point_mult2_32(0xa, 
                
                if ($a0_145 < 0)
                    *$s1_14 = ((fix_point_mult2_32(0xa, 
                        fix_point_div_32(0xa, ($v0_603 - $a1_184) << 0xa, $a2_153), $s2_39) + 0x200)
                        >> 0xa) + arg13[2];
                else
                {
                        fix_point_div_32(0xa, $a0_145 << 0xa, $a2_153), $s2_39) + 0x200;
                    *$s1_14 = arg13[2] - ($v0_606 >> 0xa);
                }
            }
            else if ($s7_5 < var_1a8)
            {
                int32_t $a1_190 = arg13[3];
                int32_t $v0_611 = arg13[2];
                int32_t $a0_147 = $a1_190 - $v0_611;
                int32_t $a2_157 = (var_1a8 - var_1ac) << 0xa;
                int32_t $s2_41 = (var_1a8 - $s7_5) << 0xa;
                
                if ($a0_147 < 0)
                    *$s1_14 = ((fix_point_mult2_32(0xa, 
                        fix_point_div_32(0xa, ($v0_611 - $a1_190) << 0xa, $a2_157), $s2_41) + 0x200)
                        >> 0xa) + arg13[3];
                else
                {
                    $v0_591 = (fix_point_mult2_32(0xa, 
                        fix_point_div_32(0xa, $a0_147 << 0xa, $a2_157), $s2_41) + 0x200) >> 0xa;
                    *$s1_14 = arg13[3] - $v0_591;
                }
            }
            else if ($s7_5 < var_1a4)
            {
                int32_t $a1_196 = arg13[4];
                int32_t $v0_619 = arg13[3];
                int32_t $a0_149 = $a1_196 - $v0_619;
                int32_t $a2_161 = (var_1a4 - var_1a8) << 0xa;
                int32_t $s2_43 = (var_1a4 - $s7_5) << 0xa;
                
                if ($a0_149 < 0)
                    *$s1_14 = ((fix_point_mult2_32(0xa, 
                        fix_point_div_32(0xa, ($v0_619 - $a1_196) << 0xa, $a2_161), $s2_43) + 0x200)
                        >> 0xa) + arg13[4];
                else
                {
                    $v0_591 = (fix_point_mult2_32(0xa, 
                        fix_point_div_32(0xa, $a0_149 << 0xa, $a2_161), $s2_43) + 0x200) >> 0xa;
                    *$s1_14 = arg13[4] - $v0_591;
                }
            }
            else if ($s7_5 < var_1a0)
            {
                int32_t $a1_202 = arg13[5];
                int32_t $v0_626 = arg13[4];
                int32_t $a0_151 = $a1_202 - $v0_626;
                int32_t $a2_165 = (var_1a0 - var_1a4) << 0xa;
                int32_t $s2_45 = (var_1a0 - $s7_5) << 0xa;
                
                if ($a0_151 < 0)
                    *$s1_14 = ((fix_point_mult2_32(0xa, 
                        fix_point_div_32(0xa, ($v0_626 - $a1_202) << 0xa, $a2_165), $s2_45) + 0x200)
                        >> 0xa) + arg13[5];
                else
                {
                    $v0_591 = (fix_point_mult2_32(0xa, 
                        fix_point_div_32(0xa, $a0_151 << 0xa, $a2_165), $s2_45) + 0x200) >> 0xa;
                    *$s1_14 = arg13[5] - $v0_591;
                }
            }
            else if ($s7_5 < var_19c)
            {
                int32_t $a1_208 = arg13[6];
                int32_t $v0_634 = arg13[5];
                int32_t $a0_153 = $a1_208 - $v0_634;
                int32_t $a2_169 = (var_19c - var_1a0) << 0xa;
                int32_t $s2_47 = (var_19c - $s7_5) << 0xa;
                
                if ($a0_153 < 0)
                    *$s1_14 = ((fix_point_mult2_32(0xa, 
                        fix_point_div_32(0xa, ($v0_634 - $a1_208) << 0xa, $a2_169), $s2_47) + 0x200)
                        >> 0xa) + arg13[6];
                else
                {
                    $v0_591 = (fix_point_mult2_32(0xa, 
                        fix_point_div_32(0xa, $a0_153 << 0xa, $a2_169), $s2_47) + 0x200) >> 0xa;
                    *$s1_14 = arg13[6] - $v0_591;
                }
            }
            else if ($s7_5 >= var_198)
            {
                int32_t $a1_220 = arg13[8];
                    int32_t $v1_265 = arg13[7];
                    int32_t $a0_156 = $a1_220 - $v1_265;
                    int32_t $a2_176 = (var_194 - var_198) << 0xa;
                    int32_t $s2_52 = (var_194 - $s7_5) << 0xa;
                
                if ($s7_5 >= var_194)
                    *$s1_14 = $a1_220;
                else
                {
                    
                    if ($a0_156 < 0)
                        *$s1_14 = ((fix_point_mult2_32(0xa, 
                            fix_point_div_32(0xa, ($v1_265 - $a1_220) << 0xa, $a2_176), $s2_52) + 0x200)
                            >> 0xa) + arg13[8];
                    else
                    {
                        $v0_591 = (fix_point_mult2_32(0xa, 
                            fix_point_div_32(0xa, $a0_156 << 0xa, $a2_176), $s2_52) + 0x200) >> 0xa;
                        *$s1_14 = arg13[8] - $v0_591;
                    }
                }
            }
            else
            {
                int32_t $a1_214 = arg13[7];
                int32_t $v0_641 = arg13[6];
                int32_t $a0_155 = $a1_214 - $v0_641;
                int32_t $a2_173 = (var_198 - var_19c) << 0xa;
                int32_t $s2_50 = (var_198 - $s7_5) << 0xa;
                
                if ($a0_155 < 0)
                    *$s1_14 = ((fix_point_mult2_32(0xa, 
                        fix_point_div_32(0xa, ($v0_641 - $a1_214) << 0xa, $a2_173), $s2_50) + 0x200)
                        >> 0xa) + arg13[7];
                else
                {
                    $v0_591 = (fix_point_mult2_32(0xa, 
                        fix_point_div_32(0xa, $a0_155 << 0xa, $a2_173), $s2_50) + 0x200) >> 0xa;
                    *$s1_14 = arg13[7] - $v0_591;
                }
            }
            i_13 = &i_13[1];
            $s1_14 = &$s1_14[1];
            $s6_4 = &$s6_4[1];
        } while (&arg13[8] != i_13);
        
        $v0_171 = $v0_4;
    }
    else
    {
        void var_244;
        void var_268;
        void* $t3_6;
        int32_t $t4_4;
            int32_t $a1_6 = *($t3_6 + i_14);
            char* $a0_58 = (char*)(&var_16c + i_14); // Fixed void pointer assignment
        subsection_up(&var_244, &var_2b0, $t1_4, 8);
        $t3_6 = subsection_light(&var_244, &var_2d4, $v0_9 * 0xa, &var_268);
        
        for (int32_t i_14 = 0; (uintptr_t)i_14 != 0x24; )
        {
            i_14 += 4;
            *$a0_58 = $a1_6;
        }
        
        if (var_16c_6 <= 0)
            var_16c_7 = 0;
        
        int32_t* $v0_38 = &var_16c_8;
        int32_t $v1_63 = var_16c_9;
        
        while (true)
        {
            if ($v1_63 < $v0_38[1])
                $v0_38 = &$v0_38[1];
            else
            {
                $v0_38[1] = $v1_63 + 1;
                $v0_38 = &$v0_38[1];
            }
            
            if ($v0_38 == &var_14c_2)
                break;
            
            $v1_63 = *$v0_38;
        }
        
        int32_t $v0_39 = var_16c_10;
        int32_t $a0_62 = arg10[8];
        int32_t $v0_44 = var_14c_3;
        int32_t $a1_8 = var_168_2 - $v0_39;
        int32_t* i_15 = arg4;
        int32_t $v0_46 = *arg10;
        int32_t $v1_65 = arg10[1];
        int32_t $a2_5 = arg10[2];
        int32_t $s4_3 = $v0_46 + $a1_8;
        
        if ($v1_65 - $v0_46 >= $a1_8)
            $s4_3 = $v1_65;
        
        int32_t $s2_2 = $a2_5 + $s4_3 - $v1_65;
        int32_t $s1_1 = arg10[3];
        int32_t $a0_67 = var_164_2 - var_168_3;
        
        if ($s2_2 - $s4_3 < $a0_67)
            $s2_2 = $a0_67 + $s4_3;
        
        int32_t $s3_2 = $s1_1 + $s2_2 - $a2_5;
        int32_t $v1_70 = var_160_2 - var_164_3;
        
        if ($s3_2 - $s2_2 < $v1_70)
            $s3_2 = $v1_70 + $s2_2;
        
        int32_t $s1_2 = arg10[4] + $s3_2 - $s1_1;
        int32_t $v0_51 = var_15c_1 - var_160_3;
        
        if ($s1_2 - $s3_2 < $v0_51)
            $s1_2 = $v0_51 + $s3_2;
        
        if ($(uintptr_t)s1_2 >= 0xfff)
            $s1_2 = 0xfff;
        
        int32_t* var_cc = var_c4_1;
        int32_t* $v0_61 = var_cc_1;
        
        do
        {
            int32_t $a2_19 = *$v0_61;
            int32_t $v0_77;
        void var_28c;
        char* $t3_7 = (char*)($t4_4(&var_244, &var_2d4, 0, &var_28c)); // Fixed void pointer assignment
            int32_t $v1_89 = *($t3_7 + i_16);
            char* $a1_26 = (char*)(&var_1b4 + i_16); // Fixed void pointer assignment
            
            if ($a2_19 < $v0_39)
                $v0_77 = $v0_46 - ((fix_point_mult2_32(0xa, 
                    fix_point_div_32(0xa, $v0_46 << 0xa, $v0_39 << 0xa), ($v0_39 - $a2_19) << 0xa)
                    + 0x200) >> 0xa);
            else if ($a2_19 < var_168)
                $v0_77 = $s4_3 - ((fix_point_mult2_32(0xa, 
                    fix_point_div_32(0xa, ($s4_3 - $v0_46) << 0xa, $a1_8 << 0xa), 
                    (var_168 - $a2_19) << 0xa) + 0x200) >> 0xa);
            else if ($a2_19 < var_164)
                $v0_77 = $s2_2 - ((fix_point_mult2_32(0xa, 
                    fix_point_div_32(0xa, ($s2_2 - $s4_3) << 0xa, $a0_67 << 0xa), 
                    (var_164 - $a2_19) << 0xa) + 0x200) >> 0xa);
            else if ($a2_19 < var_160)
                $v0_77 = $s3_2 - ((fix_point_mult2_32(0xa, 
                    fix_point_div_32(0xa, ($s3_2 - $s2_2) << 0xa, $v1_70 << 0xa), 
                    (var_160 - $a2_19) << 0xa) + 0x200) >> 0xa);
            else if ($a2_19 < var_15c)
                $v0_77 = $s1_2 - ((fix_point_mult2_32(0xa, 
                    fix_point_div_32(0xa, ($s1_2 - $s3_2) << 0xa, $v0_51 << 0xa), 
                    (var_15c - $a2_19) << 0xa) + 0x200) >> 0xa);
            else if ($a2_19 >= $v0_44)
                $v0_77 = $a0_62;
            else
                $v0_77 = $a0_62 - ((fix_point_mult2_32(0xa, 
                    fix_point_div_32(0xa, ($a0_62 - $s1_2) << 0xa, ($v0_44 - var_15c) << 0xa), 
                    ($v0_44 - $a2_19) << 0xa) + 0x200) >> 0xa);
            
            *i_15 = $v0_77;
            i_15 = &i_15[1];
            var_cc = &var_cc[1];
            $v0_61 = var_cc;
        } while (i_15 != &arg4[0xb]);
        
        
        for (int32_t i_16 = 0; (uintptr_t)i_16 != 0x24; )
        {
            *(&var_190 + i_16) = $v1_89;
            i_16 += 4;
            *$a1_26 = $v1_89;
        }
        
        int32_t* $s1_3 = arg2;
        int32_t* $s6_2 = arg1;
        int32_t* i_17 = arg13;
        
        do
        {
            int32_t $a2_32 = var_1b4;
            int32_t $v0_110 = *$s6_2;
                int32_t $a1_27 = arg13[1];
                int32_t $v0_112 = *arg13;
                int32_t $a0_73 = $a1_27 - $v0_112;
                int32_t $a2_34 = (var_1b0 - $a2_32) << 0xa;
                int32_t $s2_5 = (var_1b0 - $v0_110) << 0xa;
                    int32_t $v0_115 =
            *$s1_3 = *i_17;
            
            if ($v0_110 < $a2_32)
                *$s1_3 = *arg13;
            else if ($v0_110 < var_1b0)
            {
                
                if ($a0_73 < 0)
                    *$s1_3 = ((fix_point_mult2_32(0xa, 
                        fix_point_div_32(0xa, ($v0_112 - $a1_27) << 0xa, $a2_34), $s2_5) + 0x200)
                        >> 0xa) + arg13[1];
                else
                {
                        fix_point_mult2_32(0xa, fix_point_div_32(0xa, $a0_73 << 0xa, $a2_34), $s2_5)
                        + 0x200;
                    *$s1_3 = arg13[1] - ($v0_115 >> 0xa);
                }
            }
            else
            {
                int32_t $v0_116;
                    int32_t $a1_33 = arg13[2];
                    int32_t $v0_121 = arg13[1];
                    int32_t $a0_74 = $a1_33 - $v0_121;
                    int32_t $a2_38 = (var_1ac - var_1b0) << 0xa;
                    int32_t $s2_7 = (var_1ac - $v0_110) << 0xa;
                
                if ($v0_110 < var_1ac)
                {
                    
                    if ($a0_74 < 0)
                        *$s1_3 = ((fix_point_mult2_32(0xa, 
                            fix_point_div_32(0xa, ($v0_121 - $a1_33) << 0xa, $a2_38), $s2_7) + 0x200)
                            >> 0xa) + arg13[2];
                    else
                    {
                        $v0_116 = (fix_point_mult2_32(0xa, 
                            fix_point_div_32(0xa, $a0_74 << 0xa, $a2_38), $s2_7) + 0x200) >> 0xa;
                        *$s1_3 = arg13[2] - $v0_116;
                    }
                }
                else if ($v0_110 < var_1a8_1)
                {
                    int32_t $a1_40 = arg13[3];
                    int32_t $v0_128 = arg13[2];
                    int32_t $a0_76 = $a1_40 - $v0_128;
                    int32_t $a2_42 = (var_1a8 - var_1ac) << 0xa;
                    int32_t $s2_9 = (var_1a8 - $v0_110) << 0xa;
                    
                    if ($a0_76 < 0)
                        *$s1_3 = ((fix_point_mult2_32(0xa, 
                            fix_point_div_32(0xa, ($v0_128 - $a1_40) << 0xa, $a2_42), $s2_9) + 0x200)
                            >> 0xa) + arg13[3];
                    else
                    {
                        $v0_116 = (fix_point_mult2_32(0xa, 
                            fix_point_div_32(0xa, $a0_76 << 0xa, $a2_42), $s2_9) + 0x200) >> 0xa;
                        *$s1_3 = arg13[3] - $v0_116;
                    }
                }
                else if ($v0_110 < var_1a4_1)
                {
                    int32_t $a1_47 = arg13[4];
                    int32_t $v0_135 = arg13[3];
                    int32_t $a0_77 = $a1_47 - $v0_135;
                    int32_t $a2_46 = (var_1a4 - var_1a8) << 0xa;
                    int32_t $s2_11 = (var_1a4 - $v0_110) << 0xa;
                    
                    if ($a0_77 < 0)
                        *$s1_3 = ((fix_point_mult2_32(0xa, 
                            fix_point_div_32(0xa, ($v0_135 - $a1_47) << 0xa, $a2_46), $s2_11) + 0x200)
                            >> 0xa) + arg13[4];
                    else
                    {
                        $v0_116 = (fix_point_mult2_32(0xa, 
                            fix_point_div_32(0xa, $a0_77 << 0xa, $a2_46), $s2_11) + 0x200) >> 0xa;
                        *$s1_3 = arg13[4] - $v0_116;
                    }
                }
                else if ($v0_110 < var_1a0_1)
                {
                    int32_t $a1_54 = arg13[5];
                    int32_t $v0_142 = arg13[4];
                    int32_t $a0_79 = $a1_54 - $v0_142;
                    int32_t $a2_50 = (var_1a0 - var_1a4) << 0xa;
                    int32_t $s2_13 = (var_1a0 - $v0_110) << 0xa;
                    
                    if ($a0_79 < 0)
                        *$s1_3 = ((fix_point_mult2_32(0xa, 
                            fix_point_div_32(0xa, ($v0_142 - $a1_54) << 0xa, $a2_50), $s2_13) + 0x200)
                            >> 0xa) + arg13[5];
                    else
                    {
                        $v0_116 = (fix_point_mult2_32(0xa, 
                            fix_point_div_32(0xa, $a0_79 << 0xa, $a2_50), $s2_13) + 0x200) >> 0xa;
                        *$s1_3 = arg13[5] - $v0_116;
                    }
                }
                else if ($v0_110 < var_19c_1)
                {
                    int32_t $a1_61 = arg13[6];
                    int32_t $v0_149 = arg13[5];
                    int32_t $a0_80 = $a1_61 - $v0_149;
                    int32_t $a2_54 = (var_19c - var_1a0) << 0xa;
                    int32_t $s2_15 = (var_19c - $v0_110) << 0xa;
                    
                    if ($a0_80 < 0)
                        *$s1_3 = ((fix_point_mult2_32(0xa, 
                            fix_point_div_32(0xa, ($v0_149 - $a1_61) << 0xa, $a2_54), $s2_15) + 0x200)
                            >> 0xa) + arg13[6];
                    else
                    {
                        $v0_116 = (fix_point_mult2_32(0xa, 
                            fix_point_div_32(0xa, $a0_80 << 0xa, $a2_54), $s2_15) + 0x200) >> 0xa;
                        *$s1_3 = arg13[6] - $v0_116;
                    }
                }
                else if ($v0_110 >= var_198_1)
                {
                    int32_t $a1_74 = arg13[8];
                        int32_t $v0_163 = arg13[7];
                        int32_t $a0_83 = $a1_74 - $v0_163;
                        int32_t $a2_62 = (var_194 - var_198) << 0xa;
                        int32_t $s2_20 = (var_194 - $v0_110) << 0xa;
                    
                    if ($v0_110 >= var_194)
                        *$s1_3 = $a1_74;
                    else
                    {
                        
                        if ($a0_83 < 0)
                            *$s1_3 = ((fix_point_mult2_32(0xa, 
                                fix_point_div_32(0xa, ($v0_163 - $a1_74) << 0xa, $a2_62), $s2_20)
                                + 0x200) >> 0xa) + arg13[8];
                        else
                        {
                            $v0_116 = (fix_point_mult2_32(0xa, 
                                fix_point_div_32(0xa, $a0_83 << 0xa, $a2_62), $s2_20) + 0x200) >> 0xa;
                            *$s1_3 = arg13[8] - $v0_116;
                        }
                    }
                }
                else
                {
                    int32_t $a1_68 = arg13[7];
                    int32_t $v0_156 = arg13[6];
                    int32_t $a0_81 = $a1_68 - $v0_156;
                    int32_t $a2_58 = (var_198 - var_19c) << 0xa;
                    int32_t $s2_18 = (var_198 - $v0_110) << 0xa;
                    
                    if ($a0_81 < 0)
                        *$s1_3 = ((fix_point_mult2_32(0xa, 
                            fix_point_div_32(0xa, ($v0_156 - $a1_68) << 0xa, $a2_58), $s2_18) + 0x200)
                            >> 0xa) + arg13[7];
                    else
                    {
                        $v0_116 = (fix_point_mult2_32(0xa, 
                            fix_point_div_32(0xa, $a0_81 << 0xa, $a2_58), $s2_18) + 0x200) >> 0xa;
                        *$s1_3 = arg13[7] - $v0_116;
                    }
                }
            }
            
            i_17 = &i_17[1];
            $s1_3 = &$s1_3[1];
            $s6_2 = &$s6_2[1];
        } while (i_17 != &arg13[9]);
        
        $v0_171 = $v0_4;
    }
    int32_t $v1_98;
    
    if (!$v0_171)
        $v1_98 = $v1_5;
    else if ($v0_4 == 1)
    {
        var_128 = $lo_2;
        $v1_98 = $v1_5;
    }
    else if ($v0_4 == 2)
    {
        var_128 = $a0_13;
        $v1_98 = $v1_5;
    }
    else
    {
            int32_t $lo_7;
            int32_t $hi_6;
        $v1_98 = $v1_5;
        
        if ($v0_4 == 3)
        {
            $hi_6 = HIGHD($a0_13 * $a0_14 + (0x100 - $a0_14) * $lo_2);
            $lo_7 = LOWD($a0_13 * $a0_14 + (0x100 - $a0_14) * $lo_2);
            var_128 = $lo_7 / 0x100;
            $v1_98 = $v1_5;
        }
    }
    
    int32_t var_128_1_1;
    int32_t var_124_2;
    int32_t var_11c_1;
    int32_t $v0_224;
    int32_t $v1_112;
    int32_t $v1_121;
    int32_t $a0_103;
    
    if ($v1_98 != 1)
    {
            int32_t $a0_91 = var_128;
        if ((uintptr_t)var_128 < 0x14)
            var_128_1 = 0x14;
        else
        {
            
            if ((uintptr_t)var_128 >= 0xfa1)
                $a0_91 = 0xfa0;
            
            var_128_1 = $a0_91;
        }
        
        $v1_112 = $v0_5;
        
        if ($v0_5)
            goto label_2d828;
        
        $v0_224 = var_11c;
    label_2d8fc:
        
        if ($(uintptr_t)v0_224 < 0x14)
            var_11c_1_1 = 0x14;
        else
        {
            int32_t $a0_98 = var_11c;
            
            if ((uintptr_t)var_11c >= 0x259)
                $a0_98 = 0x258;
            
            var_11c_1 = $a0_98;
        }
        
        int32_t $v0_228 = var_124_1_1;
        
        if ($a0_17)
        {
            int32_t $v1_125;
            int32_t $v0_231;
            int32_t $v1_124;
        label_2d938:
            
            if ($a0_17 == 1)
            {
                $v1_124 = 2;
                $v0_231 = $v0_8 + 0xc8;
            label_2d9a0:
                var_124_1 = $v0_231 / $v1_124;
                $v1_125 = $v1_5;
            }
            else if ($a0_17 == 2)
            {
                var_124_1 = $v0_8;
                $v1_125 = $v1_5;
            }
            else
            {
                    int32_t $lo_15;
                    int32_t $hi_12;
                    goto label_2d9a0;
                if ($a0_17 == 3)
                {
                    $v1_124 = 0x100;
                    $hi_12 = HIGHD(var_124_1 * $a0_19 + ($v0_8 + 0xc8) / 2 * (0x100 - $a0_19));
                    $lo_15 = LOWD(var_124_1 * $a0_19 + ($v0_8 + 0xc8) / 2 * (0x100 - $a0_19));
                    $v0_231 = $lo_15;
                }
                
                $v1_125 = $v1_5;
            }
            $v1_121 = $v1_10;
            
            if ($v1_125 == 1)
                goto label_2d9c8;
            
            $v0_228 = var_124_1_2;
        }
        
        if ($(uintptr_t)v0_228 < 0x14)
            var_124_2_1 = 0x14;
        else
        {
            $a0_103 = var_124_1;
            
            if ((uintptr_t)var_124_1 >= 0x259)
                $a0_103 = 0x258;
            
            var_124_2 = $a0_103;
        }
    }
    else
    {
        int32_t $v1_109 = var_128;
        int32_t $a0_90 = $v1_7;
            int32_t $v1_116;
            int32_t $v0_215;
            int32_t $v1_115;
        
        if (var_128 < $v1_6)
            $v1_109 = $v1_6;
        
        
        if ($v1_109 < $v1_7)
            $a0_90 = $v1_109;
        
        var_128_1 = $a0_90;
        
        if ($v0_5)
        {
            $v1_112 = $v0_5;
        label_2d828:
            
            if ($v1_112 == 1)
            {
                $v0_215 = var_128_1 - var_114_1;
                $v1_115 = 2;
            label_2d898:
                var_11c = $v0_215 / $v1_115;
                $v1_116 = $v1_5;
            }
            else if ($v0_5 == 2)
            {
                var_11c = $a0_15;
                $v1_116 = $v1_5;
            }
            else
            {
                    int32_t $lo_11;
                    int32_t $hi_9;
                    goto label_2d898;
                if ($v0_5 == 3)
                {
                    $v1_115 = 0x100;
                    $hi_9 = HIGHD($a0_15 * $a0_16 + (var_128_1 - var_114_1) / 2 * (0x100 - $a0_16));
                    $lo_11 = LOWD($a0_15 * $a0_16 + (var_128_1 - var_114_1) / 2 * (0x100 - $a0_16));
                    $v0_215 = $lo_11;
                }
                
                $v1_116 = $v1_5;
            }
            $v0_224 = var_11c_2;
            
            if ($v1_116 != 1)
                goto label_2d8fc;
        }
        
        int32_t $v1_118 = var_11c_3;
        
        if (var_11c_4 < $v1_8)
            $v1_118 = $v1_8;
        
        int32_t $a0_97 = $v1_9;
        
        if ($v1_118 < $v1_9)
            $a0_97 = $v1_118;
        
        var_11c_1_2 = $a0_97;
        
        if ($a0_17)
            goto label_2d938;
        
        $v1_121 = $v1_10;
    label_2d9c8:
        int32_t $v1_126 = var_124_1_3;
        
        if (var_124_1_4 < $v1_121)
            $v1_126 = $v1_10;
        
        $a0_103 = $v1_11;
        
        if ($v1_126 < $v1_11)
            $a0_103 = $v1_126;
        
        var_124_2_2 = $a0_103;
    }
    arg15[0xf] = var_128_1_2;
    arg15[0x10] = var_11c_1_3;
    arg15[0x11] = var_124_2_3;
    int32_t* $fp_1 = arg6;
    int32_t $v0_248 = $v1_13 << 0x10;
    int32_t* var_11c_2_1 = $fp_1;
    char* var_98_1_1 = (char*)(&adr_hist_512); // Fixed void pointer assignment
    void* const var_8c_1_1 = &data_20000_2;
    int32_t var_e8_3 = 0x3ffffc;
    char* $v0_253 = (char*)(var_118_1_1); // Fixed void pointer assignment
    
    while (true)
    {
        char* $v0_256 = (char*)(var_118_1); // Fixed void pointer assignment
        
        if (*$v0_253 < 2)
        {
            *var_118_1 = 1;
            $v0_256 = var_118_1;
        }
        
        int32_t $a1_92 = *$v0_256;
        int32_t $v0_267;
        
        if (var_128_1_3 >= $a1_92)
        {
            int32_t $a1_98 = var_128_1 - $a1_92;
            int32_t $a1_99 = $a1_98 * $a1_98;
            int32_t $v0_272 = (var_8c_1 + 0xfd4)(0x10, 
            int32_t $v1_137 = 0xeffff;
            
            if ($(uintptr_t)a1_99 >= 0x3ffffd)
                $a1_99 = var_e8_3;
            
                var_118(0xa, $a1_99 << 0xa, ((var_11c_1 << 1) * var_11c_1) << 0xa) << 6, $v0_3);
            
            if ($(uintptr_t)v0_272 < 0xf0000)
                $v1_137 = $v0_272;
            
            $v0_267 = (($v0_248 - var_118(0x10, $v0_248, tisp_math_exp2($v1_137, 0x10, 0x10)))
                >> 0x10) + $v1_13;
        }
        else
        {
            int32_t $a1_93 = $a1_92 - var_128_1;
            int32_t $a1_94 = $a1_93 * $a1_93;
            int32_t $v0_263 = (var_8c_1 + 0xfd4)(0x10, 
            int32_t $v1_135 = 0xeffff;
            
            if ($(uintptr_t)a1_94 >= 0x3ffffd)
                $a1_94 = var_e8_3;
            
                var_118(0xa, $a1_94 << 0xa, ((var_124_2 << 1) * var_124_2) << 0xa) << 6, $v0_3);
            
            if ($(uintptr_t)v0_263 < 0xf0000)
                $v1_135 = $v0_263;
            
            $v0_267 = var_118(0x10, $v0_248, tisp_math_exp2($v1_135, 0x10, 0x10)) >> 0x10;
        }
        
        int32_t $a1_104 = 0x2710;
        
        if ($(uintptr_t)v0_267 < 0x2711)
            $a1_104 = $v0_267;
        
        int32_t $v0_281 = (var_118_2(0xa, $a1_104 << 0xa, 0x19000) + 0x200) >> 0xa;
        int32_t* $a0_108 = &var_148;
        int32_t $v1_140 = 0;
        int32_t var_1d4;
        int32_t var_1d0;
        int32_t var_1cc;
        int32_t var_1c8;
        int32_t var_1b8;
        
        while (true)
        {
            int32_t $s1_4 = *$a0_108;
            int32_t $v0_314;
                int32_t $v1_141 = $v1_140 * 9;
                int32_t var_1c4_1 = (&var_420)[$v1_141 + 5];
                int32_t var_1c0_1 = (&var_420)[$v1_141 + 6];
                int32_t var_1bc_1 = (&var_420)[$v1_141 + 7];
            
            if ($v0_281 == $s1_4)
            {
                var_1d8 = (&var_420)[$v1_141];
                var_1d4 = (&var_420)[$v1_141 + 1];
                var_1d0 = (&var_420)[$v1_141 + 2];
                var_1cc = (&var_420)[$v1_141 + 3];
                var_1c8 = (&var_420)[$v1_141 + 4];
                $v0_314 = (&var_420)[$v1_141 + 8];
            }
            else if ($v0_281 >= $s1_4)
            {
                $v1_140 += 1;
                $a0_108 = &$a0_108[1];
                
                if ($v1_140 == 8)
                    break;
                
                continue;
            }
            else
            {
                    int32_t $s2_22 = $v1_140 * 9;
                    int32_t $s0_1 = (&var_420)[$v1_140 + 0xb5];
                    int32_t $a1_106 = (&var_420)[$s2_22 + 1];
                    int32_t $s3_3 = (&var_420)[$s2_22 - 8];
                    int32_t $a1_107;
                    int32_t $s5_1;
                if ($v1_140)
                {
                    
                    if ($s3_3 >= $a1_106)
                    {
                        $a1_107 = $s3_3 - $a1_106;
                        $s5_1 = 0;
                    }
                    else
                    {
                        $a1_107 = $a1_106 - $s3_3;
                        $s5_1 = 1;
                    }
                    
                    int32_t $s1_6 = ($s1_4 - $s0_1) << 0xa;
                    int32_t $s0_3 = ($v0_281 - $s0_1) << 0xa;
                    int32_t $v0_333 = (
                        (var_8c_1_2 + 0xfd4)(0xa, fix_point_div_32(0xa, $a1_107 << 0xa, $s1_6), $s0_3)
                        + 0x200) >> 0xa;
                    int32_t $s3_4;
                    
                    $s3_4 = $s5_1 ? $s3_3 + $v0_333 : $s3_3 - $v0_333;
                    
                    int32_t $a1_110 = (&var_420_9)[$s2_22 + 2];
                    var_1d4_1 = $s3_4;
                    int32_t $s3_5 = (&var_420_10)[$s2_22 - 7];
                    int32_t $a1_111;
                    int32_t $s5_2;
                    
                    if ($s3_5 >= $a1_110)
                    {
                        $a1_111 = $s3_5 - $a1_110;
                        $s5_2 = 0;
                    }
                    else
                    {
                        $a1_111 = $a1_110 - $s3_5;
                        $s5_2 = 1;
                    }
                    
                    int32_t $v0_344 = (
                        (var_8c_1_3 + 0xfd4)(0xa, fix_point_div_32(0xa, $a1_111 << 0xa, $s1_6), $s0_3)
                        + 0x200) >> 0xa;
                    int32_t $s3_6;
                    
                    $s3_6 = $s5_2 ? $s3_5 + $v0_344 : $s3_5 - $v0_344;
                    
                    int32_t $a1_114 = (&var_420_11)[$s2_22 + 3];
                    var_1d0_1 = $s3_6;
                    int32_t $s3_7 = (&var_420_12)[$s2_22 - 6];
                    int32_t $a1_115;
                    int32_t $s5_3;
                    
                    if ($s3_7 >= $a1_114)
                    {
                        $a1_115 = $s3_7 - $a1_114;
                        $s5_3 = 0;
                    }
                    else
                    {
                        $a1_115 = $a1_114 - $s3_7;
                        $s5_3 = 1;
                    }
                    
                    int32_t $v0_355 = (
                        (var_8c_1_4 + 0xfd4)(0xa, fix_point_div_32(0xa, $a1_115 << 0xa, $s1_6), $s0_3)
                        + 0x200) >> 0xa;
                    int32_t $s3_8;
                    
                    $s3_8 = $s5_3 ? $s3_7 + $v0_355 : $s3_7 - $v0_355;
                    
                    int32_t $a1_118 = (&var_420_13)[$s2_22 + 4];
                    int32_t $s2_26 = (&var_420_14)[$s2_22 - 5];
                    var_1cc_1 = $s3_8;
                    int32_t $a1_119;
                    int32_t $s3_9;
                    
                    if ($s2_26 >= $a1_118)
                    {
                        $a1_119 = $s2_26 - $a1_118;
                        $s3_9 = 0;
                    }
                    else
                    {
                        $a1_119 = $a1_118 - $s2_26;
                        $s3_9 = 1;
                    }
                    
                    int32_t $v0_363 = (
                        (var_8c_1_5 + 0xfd4)(0xa, fix_point_div_32(0xa, $a1_119 << 0xa, $s1_6), $s0_3)
                        + 0x200) >> 0xa;
                    int32_t $s2_27;
                    
                    $s2_27 = $s3_9 ? $s2_26 + $v0_363 : $s2_26 - $v0_363;
                    
                    var_1c8_1 = $s2_27;
                    break;
                }
                
                var_1d8 = var_420_15;
                var_1d4_2 = var_41c_1;
                var_1d0_2 = var_418_1;
                var_1cc_2 = var_414_1;
                var_1c8_2 = var_410_4;
                int32_t var_1c4_2 = var_40c_1;
                int32_t var_1c0_2 = var_408_1;
                int32_t var_1bc_2 = var_404_1;
                $v0_314 = var_400_1;
            }
            
            var_1b8_1 = $v0_314;
            break;
        }
        
        int32_t* $v0_366 = &var_1d8_1;
        
        if (var_1d8_2 < 2)
        {
            var_1d8 = 1;
            $v0_366 = &var_1d8;
        }
        
        int32_t $v1_148 = var_1d8_3;
        
        while (true)
        {
            if ($v1_148 < $v0_366[1])
                $v0_366 = &$v0_366[1];
            else
            {
                $v0_366[1] = $v1_148 + 1;
                $v0_366 = &$v0_366[1];
            }
            
            if (&var_1b8_2 == $v0_366)
                break;
            
            $v1_148 = *$v0_366;
        }
        
        int32_t $v0_367 = var_1d8_4;
        int32_t i_18 = 0;
        int32_t $v0_372 = var_1b8_3;
        int32_t $s5_4 = *arg10;
        int32_t $v0_374 = arg10[8];
        int32_t $a1_122 = var_1d4_3 - $v0_367;
        int32_t $v0_377 = $v1_1 + $s5_4;
        
        if ($v1_1 < $a1_122)
            $v0_377 = $s5_4 + $a1_122;
        
        int32_t $a0_111 = var_1d0_3 - var_1d4_4;
        int32_t $v0_380 = $v1_2 + $v0_377;
        
        if ($v1_2 < $a0_111)
            $v0_380 = $v0_377 + $a0_111;
        
        int32_t $v1_155 = var_1cc_3 - var_1d0_4;
        int32_t $v0_383 = $v1_3 + $v0_380;
        
        if ($v1_3 < $v1_155)
            $v0_383 = $v0_380 + $v1_155;
        
        int32_t $v0_385 = var_1c8_3 - var_1cc_4;
        int32_t $a2_89 = $v1_4 + $v0_383;
        
        if ($v1_4 < $v0_385)
            $a2_89 = $v0_383 + $v0_385;
        
        int32_t* $s0_5 = var_11c_2_2;
        int32_t* $s4_4 = arg5;
        
        do
        {
            int32_t $a2_95 = *$s4_4;
            int32_t $v0_401;
                int32_t $v0_435 = *$s4_4;
                    int32_t $v0_438 = $v0_435 - *($s4_4 - 4);
                    int32_t $a1_138 = (&var_300)[i_18];
                    int32_t $a0_118 = *($s0_5 - 4);
            
            if ($a2_95 < $v0_367)
                $v0_401 = $s5_4 - (((var_8c_1 + 0xfd4)(0xa, 
                    fix_point_div_32(0xa, $s5_4 << 0xa, $v0_367 << 0xa), ($v0_367 - $a2_95) << 0xa)
                    + 0x200) >> 0xa);
            else if ($a2_95 < var_1d4)
                $v0_401 = $v0_377 - (((var_8c_1 + 0xfd4)(0xa, 
                    fix_point_div_32(0xa, ($v0_377 - $s5_4) << 0xa, $a1_122 << 0xa), 
                    (var_1d4 - $a2_95) << 0xa) + 0x200) >> 0xa);
            else if ($a2_95 < var_1d0)
                $v0_401 = $v0_380 - (((var_8c_1 + 0xfd4)(0xa, 
                    fix_point_div_32(0xa, ($v0_380 - $v0_377) << 0xa, $a0_111 << 0xa), 
                    (var_1d0 - $a2_95) << 0xa) + 0x200) >> 0xa);
            else if ($a2_95 < var_1cc)
                $v0_401 = $v0_383 - (((var_8c_1 + 0xfd4)(0xa, 
                    fix_point_div_32(0xa, ($v0_383 - $v0_380) << 0xa, $v1_155 << 0xa), 
                    (var_1cc - $a2_95) << 0xa) + 0x200) >> 0xa);
            else if ($a2_95 < var_1c8)
                $v0_401 = $a2_89 - (((var_8c_1 + 0xfd4)(0xa, 
                    fix_point_div_32(0xa, ($a2_89 - $v0_383) << 0xa, $v0_385 << 0xa), 
                    (var_1c8 - $a2_95) << 0xa) + 0x200) >> 0xa);
            else if ($a2_95 >= $v0_372)
                $v0_401 = $v0_374;
            else
                $v0_401 = $v0_374 - (((var_8c_1 + 0xfd4)(0xa, 
                    fix_point_div_32(0xa, ($v0_374 - $a2_89) << 0xa, ($v0_372 - var_1c8) << 0xa), 
                    ($v0_372 - $a2_95) << 0xa) + 0x200) >> 0xa);
            
            *$s0_5 = $v0_401;
            
            if ($v1_12 != 1)
                i_18 += 1;
            else
            {
                
                if (i_18)
                {
                    
                    if ($a1_138 >= ((*$s0_5 - $a0_118) << 0x10) / $v0_438)
                        i_18 += 1;
                    else
                    {
                        *$s0_5 = ($v0_438 * $a1_138 + ($a0_118 << 0x10)) / isp_printf;
                        i_18 += 1;
                    }
                }
                else
                {
                    int32_t $a0_117 = var_300;
                    
                    if ($a0_117 >= (*var_11c_2 << 0x10) / $v0_435)
                        i_18 += 1;
                    else
                    {
                        *var_11c_2 = $v0_435 * $a0_117 / isp_printf;
                        i_18 += 1;
                    }
                }
            }
            
            $s4_4 = &$s4_4[1];
            $s0_5 = &$s0_5[1];
        } while ((uintptr_t)i_18 != 0xb);
        
        var_118_1_2 += 4;
        var_11c_2_3 = &var_11c_2_4[0xb];
        $v0_253 = var_118_1_3;
        
        if (var_98_1_2 == var_118_1_4)
            break;
    }
    
    int32_t result = 1;
    
    if ($a0_20 == 1)
    {
                int32_t $a1_140 = result - $a0_21;
                int32_t* i_19 = arg5;
                    int32_t* $a3_21 = $fp_1;
                        int32_t $v1_199 = *$a3_21;
                        int32_t $v0_451 = ($v1_199 - *i_19) * $a1_140;
        result = *(($v1_15 << 2) + &data_a80c0);
        
        if (result >= $a0_21)
        {
            if ($a0_22 >= result)
            {
                
                while (true)
                {
                    
                    do
                    {
                        i_19 = &i_19[1];
                        $a3_21 = &$a3_21[1];
                        result = (($v1_199 << 0xa) - ($v0_451 << 0xa) / ($a0_22 - $a0_21)) / 0x400;
                        *($a3_21 - 4) = result;
                    } while (i_19 != &arg5[0xb]);
                    
                    $fp_1 = &$fp_1[0xb];
                    
                    if (&arg6[0x108] == $fp_1)
                        break;
                    
                    i_19 = arg5;
                }
            }
            else
            {
                    int32_t $a1_139 = *(arg5 + result);
                    char* $a0_123 = (char*)($fp_1 + result); // Fixed void pointer assignment
                result = 0;
                
                while (true)
                {
                    result += 4;
                    *$a0_123 = $a1_139;
                    
                    if ((uintptr_t)result == 0x2c)
                    {
                        $fp_1 = &$fp_1[0xb];
                        
                        if (&arg6[0x108] == $fp_1)
                            break;
                        
                        result = 0;
                    }
                }
            }
        }
    }
    
    return result;
}

