#include "include/main.h"


  int32_t tiziano_bcsh_lut_parameter(int32_t* arg1, int32_t* arg2, int32_t* arg3, int32_t* arg4, int32_t* arg5, int32_t* arg6, int32_t* arg7, int32_t* arg8, int32_t* arg9, int32_t* arg10, int32_t* arg11, int32_t* arg12, int32_t* arg13, int32_t* arg14, int32_t* arg15, int32_t* arg16, int32_t* arg17, int32_t* arg18, int32_t* arg19)

{
    int32_t* arg_4 = arg2;
    int32_t $t6_2 = *arg2 << 0x10 | arg2[1];
    int32_t* arg_0 = arg1;
    int32_t* arg_8 = arg3;
    int32_t* arg_c = arg4;
    int32_t $t5_2 = *arg3 << 0x10 | arg3[1];
    int32_t $t4_2 = arg1[2] << 0x10 | arg1[3];
    int32_t $t3_2 = arg2[2] << 0x10 | arg2[3];
    int32_t $t2_2 = arg3[2] << 0x10 | arg3[3];
    int32_t $t1_6 = *arg5 << 0x10 | *arg4;
    int32_t $t0_3 = arg5[1] << 0x10 | arg4[1];
    int32_t $a3_1 = arg5[2] << 0x10 | arg4[2];
    int32_t $fp_2 = arg13[1] << 0x10 | *arg13;
    int32_t $t7_1 = arg13[2];
    int32_t $s7_2 = arg13[4] << 0x10 | arg13[3];
    int32_t $a0_8 = arg13[5];
    int32_t $a0_11 = arg13[7] << 0x10 | arg13[6];
    int32_t $a0_12 = arg13[8];
    int32_t $a0_16 = *arg15 << 0x10 | *arg14;
    int32_t $a0_19 = arg14[2] << 0x10 | arg14[1];
    int32_t $a0_23 = *arg17 << 0x10 | *arg16;
    int32_t $a0_26 = arg16[2] << 0x10 | arg16[1];
    int32_t $a0_30 = *arg19 << 0x10 | *arg18;
    uint32_t $s1_3 = arg18[2] << 0x10 | arg18[1];
    int32_t $a0_35 = *arg7 << 0x10 | *arg6;
    int32_t $a0_39 = *arg9 << 0x10 | *arg8;
    int32_t $a0_42 = arg6[2] << 0x10 | arg6[1];
    int32_t $s6_2 = arg6[4] << 0x10 | arg6[3];
    int32_t $s2_4 = arg12[1] << 0x10 | *arg12 << 3 | *arg10;
    int32_t $s5_2 = arg10[2] << 0x10 | arg10[1];
    int32_t $s4_2 = *arg11 << 0x10 | arg11[1];
    int32_t $s3_2 = arg11[2] << 0x10 | arg11[3];
    uint32_t tisp_BCSH_au32clip0_1 = tisp_BCSH_au32clip0;
    uint32_t $v1_1 = data_c53e4;
    uint32_t $a2_10 = data_c53ec;
    uint32_t $v0_7 = data_9a614 >> 0xa;
        void* tisp_BCSH_au32HLSP_now_1 = tisp_BCSH_au32HLSP_now;
        int32_t $v1_7 = $v0_7 < 2 ? 1 : 0;
        uint32_t $v0_8 = tisp_BCSH_au32clip0_1 - $v0_7;
        uint32_t $a2_11 = $v0_7 - 1;
        uint32_t tisp_BCSH_au32clip0_2 = $a0_48 - 1;
        int32_t $v0_9 = 1;
    
    if ($(uintptr_t)v1_1 >= 0xa)
        $v1_1 = 9;
    else if (!$v1_1)
        $v1_1 = 1;
    
    if ($(uintptr_t)a2_10 >= 0xa)
        $a2_10 = 9;
    else if (!$a2_10)
        $a2_10 = 1;
    
    int32_t $a0_48;
    
    if (tisp_BCSH_au32clip0_1 == 1)
        $a0_48 = *(tisp_BCSH_au32EvList_now + (($v1_1 - 1) << 2));
    
    if (tisp_BCSH_au32clip0_1 == 1 && $v0_7 < $a0_48)
    {
        
        if ($v1_7)
            $a2_11 = $v0_8;
        
        
        if (!$a0_48)
            tisp_BCSH_au32clip0_2 = tisp_BCSH_au32clip0_1;
        
        if (!$v1_7)
            $v0_8 = $v0_7 - 1;
        
        
        if ($a0_48)
            $v0_9 = $a0_48 - 1;
        
        $s1_3 = ($v0_8 * *(tisp_BCSH_au32HLSP_now_1 + 4) / $v0_9)
            | ($a2_11 * *(tisp_BCSH_au32HLSP_now_1 + 8) / tisp_BCSH_au32clip0_2) << 0x10;
    }
    else if (data_c53e8_1 == 1)
    {
        void* tisp_BCSH_au32EvList_now_1 = tisp_BCSH_au32EvList_now;
        int32_t $v1_8 = *(tisp_BCSH_au32EvList_now_1 + 0x20);
            int32_t $a0_50 = *(tisp_BCSH_au32EvList_now_1 + (($a2_10 - 1) << 2));
                void* tisp_BCSH_au32HLSP_now_2 = tisp_BCSH_au32HLSP_now;
                int32_t $t9_1 = *(tisp_BCSH_au32HLSP_now_2 + 8);
                    int32_t $s0_1 = $a0_50 - $v1_8;
        
        if ($v0_7 >= $v1_8)
            $s1_3 = 0;
        else
        {
            
            if ($a0_50 < $v0_7)
            {
                int32_t $s1_6;
                
                if (!$t9_1)
                    $s1_6 = *(tisp_BCSH_au32HLSP_now_2 + 4);
                else
                {
                    
                    if ($v1_8 >= $a0_50)
                        $s0_1 = $v1_8 - $a0_50;
                    
                    $t9_1 = ($t9_1 - ($v0_7 - $a0_50) * $t9_1 / $s0_1) << 0x10;
                    $s1_6 = *(tisp_BCSH_au32HLSP_now_2 + 4);
                }
                
                if (!$s1_6)
                    $s1_3 = $t9_1 | $s1_6;
                else
                {
                    int32_t $v1_9 = $v1_8 - $a0_50;
                    
                    if ($v1_8 < $a0_50)
                        $v1_9 = $a0_50 - $v1_8;
                    
                    $s1_3 = $t9_1 | ($s1_6 - ($v0_7 - $a0_50) * $s1_6 / $v1_9);
                }
            }
        }
    }
    
    system_reg_write(0x8000, *arg1 << 0x10 | arg1[1]);
    system_reg_write(0x8004, $t6_2);
    system_reg_write(0x8008, $t5_2);
    system_reg_write(0x800c, $t4_2);
    system_reg_write(0x8010, $t3_2);
    system_reg_write(0x8014, $t2_2);
    system_reg_write(0x8018, $t1_6);
    system_reg_write(0x801c, $t0_3);
    system_reg_write(0x8020, $a3_1);
    system_reg_write(0x8024, $fp_2);
    system_reg_write(0x8028, $t7_1);
    system_reg_write(0x802c, $s7_2);
    system_reg_write(0x8030, $a0_8);
    system_reg_write(0x8034, $a0_11);
    system_reg_write(0x8038, $a0_12);
    system_reg_write(0x803c, $a0_16);
    system_reg_write(0x8040, $a0_19);
    system_reg_write(0x8044, $a0_23);
    system_reg_write(0x8048, $a0_26);
    system_reg_write(0x804c, $a0_30);
    system_reg_write(0x8050, $s1_3);
    system_reg_write(0x8054, $a0_35);
    system_reg_write(0x8058, $a0_39);
    system_reg_write(0x805c, $a0_42);
    system_reg_write(0x8060, $s6_2);
    system_reg_write(0x8064, $s2_4);
    system_reg_write(0x8068, $s5_2);
    system_reg_write(0x806c, $s4_2);
    system_reg_write(0x8070, $s3_2);
    return 0;
}

