#include "include/main.h"


  int32_t defog_3x3_5x5_params_init(int32_t arg1, int32_t arg2)

{
    uint32_t $lo = (arg2 + 5) / 0xa;
    int32_t i = 0;
    uint32_t $lo_1 = (arg1 + 9) / 0x12;
    uint32_t $t2 = $lo_1 << 2;
    uint32_t $lo_3 = ((arg2 * arg1 + 0x3f4) / 0x7e9 * data_acd54 + 0x32) / 0x64;
        void* $t0_1 = &param_defog_cent3_w_dis_array_tmp + i;
        int32_t $v1_3 = $lo_3 * *(&var_b8 + i);
        void* $a3_1 = &var_2b0 + i_1;
        int32_t $v1_7 = *(&var_1b4 + i_1);
    void var_b8;
    memcpy(&var_b8, 0x7d0b4, 0x60);
    void var_1b4;
    memcpy(&var_1b4, 0x7d114, 0xfc);
    void var_2b0;
    memset(&var_2b0, 0, 0xfc);
    
    do
    {
        i += 4;
        *$t0_1 = ($v1_3 + 0x2000) >> 0xe;
    } while ((uintptr_t)i != 0x60);
    
    for (int32_t i_1 = 0; (uintptr_t)i_1 != 0xfc; )
    {
        i_1 += 4;
        *$a3_1 = ($lo_3 * $v1_7 + 0x1000) >> 0xd;
    }
    
    void* $v1_11 = &var_2b0;
    
    for (int32_t i_2 = 0; (uintptr_t)i_2 != 0x7c; )
    {
        void* $a3_2 = &param_defog_cent5_w_dis_array_tmp + i_2;
        i_2 += 4;
        *$a3_2 = *($v1_11 + 4);
        $v1_11 += 8;
    }
    
    int32_t $s7 = $lo * $lo_1;
    int32_t $s2_1 = $s7 << 2;
    int32_t* $v0_6 = private_vmalloc($s2_1);
    int32_t* $v0_7 = private_vmalloc($s2_1);
    int32_t* $v0_8 = private_vmalloc($s2_1);
    int32_t* $v0_9 = private_vmalloc($s2_1);
    int32_t* $v0_10 = private_vmalloc($s2_1);
    int32_t* $v0_11 = private_vmalloc($s2_1);
    int32_t* $v0_12 = private_vmalloc($s2_1);
    int32_t* $v0_13 = private_vmalloc($s2_1);
    int32_t* $t5 = $v0_9;
    int32_t* $t3 = $v0_10;
    int32_t* $v1_12 = $v0_11;
    int32_t* $a0_11 = $v0_12;
    int32_t* $t1 = $v0_13;
    int32_t* $t0_3 = $v0_6;
    int32_t* $a3_3 = $v0_7;
    int32_t* $a1_2 = $v0_8;
    int32_t $v0_14 = 0;
    
    while (true)
    {
        int32_t temp1_1 = $v0_14;
        $v0_14 += 1;
        
        if (temp1_1 == $s7)
            break;
        
        *$t0_3 = 0;
        *$a3_3 = 0;
        $t0_3 = &$t0_3[1];
        *$a1_2 = 0;
        $a3_3 = &$a3_3[1];
        *$t5 = 0;
        $a1_2 = &$a1_2[1];
        *$t3 = 0;
        $t5 = &$t5[1];
        *$v1_12 = 0;
        $t3 = &$t3[1];
        *$a0_11 = 0;
        $v1_12 = &$v1_12[1];
        *$t1 = 0;
        $a0_11 = &$a0_11[1];
        $t1 = &$t1[1];
    }
    
    int32_t $a1_3 = $lo * 5 - 1;
    int32_t $a3_4 = 0;
    int32_t $t9 = 0;
    
    while ($t9 != $lo)
    {
        int32_t $v1_14 = $a1_3 + (-($lo) << 1);
        int32_t $fp_1 = $v1_14 * $v1_14;
        int32_t $v1_15 = $v1_14 - ($lo << 1);
        int32_t $s7_1 = $a1_3 * $a1_3;
        int32_t $t4_1 = $v1_15 * $v1_15;
        int32_t $t3_1 = $t2 + $lo_1 - 1;
        int32_t $a0_13 = 0;
            int32_t $v1_17 = $t3_1 + (-($lo_1) << 1);
            int32_t $t1_1 = $v1_17 * $v1_17;
            int32_t $v1_18 = $v1_17 - ($lo_1 << 1);
            int32_t $t0_4 = $t3_1 * $t3_1;
            int32_t $v1_19 = $v1_18 * $v1_18;
            void* $t0_7 = &var_2b0;
        
        while ($a0_13 != $t2)
        {
            
            for (int32_t i_3 = 0; (uintptr_t)i_3 != 0x3f; )
            {
                if (($s7_1 + $t0_4 + 0x20) >> 6 < *$t0_7)
                {
                    *(((void**)((char*)$v0_6 + $a0_13 + $a3_4))) = 0x3f - i_3; // Fixed void pointer dereference
                    break;
                }
                
                i_3 += 1;
                $t0_7 += 4;
            }
            
            void* $t0_10 = &var_2b0_1;
            
            for (int32_t i_4 = 0; (uintptr_t)i_4 != 0x3f; )
            {
                if (($s7_1 + $t1_1 + 0x20) >> 6 < *$t0_10)
                {
                    *(((void**)((char*)$v0_7 + $a0_13 + $a3_4))) = 0x3f - i_4; // Fixed void pointer dereference
                    break;
                }
                
                i_4 += 1;
                $t0_10 += 4;
            }
            
            void* $t0_13 = &var_2b0_2;
            
            for (int32_t i_5 = 0; (uintptr_t)i_5 != 0x3f; )
            {
                if (($s7_1 + $v1_19 + 0x20) >> 6 < *$t0_13)
                {
                    *(((void**)((char*)$v0_8 + $a0_13 + $a3_4))) = 0x3f - i_5; // Fixed void pointer dereference
                    break;
                }
                
                i_5 += 1;
                $t0_13 += 4;
            }
            
            void* $t0_16 = &var_2b0_3;
            
            for (int32_t i_6 = 0; (uintptr_t)i_6 != 0x3f; )
            {
                if (($t0_4 + $fp_1 + 0x20) >> 6 < *$t0_16)
                {
                    *(((void**)((char*)$v0_9 + $a0_13 + $a3_4))) = 0x3f - i_6; // Fixed void pointer dereference
                    break;
                }
                
                i_6 += 1;
                $t0_16 += 4;
            }
            
            void* $t0_19 = &var_2b0_4;
            
            for (int32_t i_7 = 0; (uintptr_t)i_7 != 0x3f; )
            {
                if (($t1_1 + $fp_1 + 0x20) >> 6 < *$t0_19)
                {
                    *(((void**)((char*)$v0_10 + $a0_13 + $a3_4))) = 0x3f - i_7; // Fixed void pointer dereference
                    break;
                }
                
                i_7 += 1;
                $t0_19 += 4;
            }
            
            void* $t0_22 = &var_2b0_5;
            
            for (int32_t i_8 = 0; (uintptr_t)i_8 != 0x3f; )
            {
                if (($v1_19 + $fp_1 + 0x20) >> 6 < *$t0_22)
                {
                    *(((void**)((char*)$v0_11 + $a0_13 + $a3_4))) = 0x3f - i_8; // Fixed void pointer dereference
                    break;
                }
                
                i_8 += 1;
                $t0_22 += 4;
            }
            
            void* $t0_25 = &var_2b0_6;
            
            for (int32_t i_9 = 0; (uintptr_t)i_9 != 0x3f; )
            {
                if (($t0_4 + $t4_1 + 0x20) >> 6 < *$t0_25)
                {
                    *(((void**)((char*)$v0_12 + $a0_13 + $a3_4))) = 0x3f - i_9; // Fixed void pointer dereference
                    break;
                }
                
                i_9 += 1;
                $t0_25 += 4;
            }
            
            void* $t0_28 = &var_2b0_7;
            
            for (int32_t i_10 = 0; (uintptr_t)i_10 != 0x3f; )
            {
                if (($t1_1 + $t4_1 + 0x20) >> 6 < *$t0_28)
                {
                    *(((void**)((char*)$v0_13 + $a0_13 + $a3_4))) = 0x3f - i_10; // Fixed void pointer dereference
                    break;
                }
                
                i_10 += 1;
                $t0_28 += 4;
            }
            
            $a0_13 += 4;
            $t3_1 -= 2;
        }
        
        $t9 += 1;
        $a1_3 -= 2;
        $a3_4 += $t2;
    }
    
    void* $v0_21 = private_vmalloc($s2_1);
    void* $v0_22 = private_vmalloc($s2_1);
    void* $v0_23 = private_vmalloc($s2_1);
    void* $v0_24 = private_vmalloc($s2_1);
    void* $v0_25 = private_vmalloc($s2_1);
    int32_t $a0_19 = 0;
    int32_t $s2_2 = 0;
    
    while ($s2_2 != $lo)
    {
        int32_t $t1_25 = 0;
        void* $a1_4 = $v0_10 + $a0_19;
        void* $t5_7 = $v0_11 + $a0_19;
        void* $t3_2 = $v0_13 + $a0_19;
            int32_t $v1_35 = $a0_19 + $t1_25;
        
        while (true)
        {
            
            if ($t2 == $t1_25)
                break;
            
            *(((void**)((char*)$v0_21 + $v1_35))) = *$a1_4 >> 1; // Fixed void pointer dereference
            *(((void**)((char*)$v0_22 + $v1_35))) = (*$a1_4 + *$t5_7) >> 2; // Fixed void pointer dereference
            *(((void**)((char*)$v0_23 + $v1_35))) = (*$a1_4 + *$t3_2) >> 2; // Fixed void pointer dereference
            *(((void**)((char*)$v0_24 + $v1_35))) = *$t5_7 >> 1; // Fixed void pointer dereference
            *(((void**)((char*)$v0_25 + $v1_35))) = *$t3_2 >> 1; // Fixed void pointer dereference
            $a1_4 += 4;
            $t1_25 += 4;
            $t5_7 += 4;
            $t3_2 += 4;
        }
        
        $s2_2 += 1;
        $a0_19 += $t2;
    }
    
    defog_wei_interpcot($s2_2, $lo_1, $v0_21, $v0_6, &param_defog_weightlut22_tmp);
    defog_wei_interpcot($s2_2, $lo_1, $v0_22, $v0_7, &param_defog_weightlut21_tmp);
    defog_wei_interpcot($s2_2, $lo_1, $v0_23, $v0_9, &param_defog_weightlut12_tmp);
    defog_wei_interpcot($s2_2, $lo_1, $v0_24, $v0_8, &param_defog_weightlut20_tmp);
    defog_wei_interpcot($s2_2, $lo_1, $v0_25, $v0_12, &param_defog_weightlut02_tmp);
    private_vfree($v0_6);
    private_vfree($v0_7);
    private_vfree($v0_8);
    private_vfree($v0_9);
    private_vfree($v0_10);
    private_vfree($v0_11);
    private_vfree($v0_12);
    private_vfree($v0_13);
    private_vfree($v0_21);
    private_vfree($v0_22);
    private_vfree($v0_23);
    private_vfree($v0_24);
    private_vfree($v0_25);
    return 0;
}

