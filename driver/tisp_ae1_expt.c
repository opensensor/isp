#include "include/main.h"


  int32_t tisp_ae1_expt(void* arg1, int32_t* arg2, int32_t* arg3, int32_t* arg4, int32_t* arg5, int32_t* arg6, int32_t* arg7, int32_t* arg8, int32_t* arg9, int32_t* arg10, int32_t* arg11)

{
    int32_t _AePointPos_1 = *_AePointPos;
    int32_t arg_4 = $a1;
    int32_t arg_8 = $a2;
    int32_t arg_c = $a3;
    int32_t arg_0 = $a0;
    int32_t $v0 = fix_point_mult3_32(_AePointPos_1, *arg2 << (_AePointPos_1 & 0x1f), arg2[1]);
    uint32_t* i = arg5;
    void* $a0_2 = &ae1_ev_list;
        uint32_t $a1_4 = *$a0_2;
        int32_t* $a1_7 = arg4;
            int32_t $v1_6 = 0;
    int32_t $a1;
    int32_t $a2;
    int32_t $a3;
    int32_t $a0;
    
    do
    {
        
        if (*arg3 == 1)
            $a1_4 = ($a1_4 * arg3[3]) >> 7;
        
        *i = $a1_4;
        
        if (!*i)
            *i = 1;
        
        i = &i[1];
        $a0_2 += 4;
    } while (&arg5[0xa] != i);
    
    int32_t $a0_3;
    
    if (*arg4 << (_AePointPos_1 & 0x1f) < $v0)
    {
        
        if ($v0 < arg4[9] << (_AePointPos_1 & 0x1f))
        {
            int32_t $v1_7;
            
            while (true)
            {
                if ($v0 < *$a1_7 << (_AePointPos_1 & 0x1f))
                    $v1_6 += 1;
                else
                {
                    if ($a1_7[1] << (_AePointPos_1 & 0x1f) >= $v0)
                    {
                        $v1_7 = $v1_6 << 2;
                        break;
                    }
                    
                    $v1_6 += 1;
                }
                
                $a1_7 = &$a1_7[1];
                
                if ($v1_6 == 9)
                {
                    $v1_7 = 0;
                    break;
                }
            }
            
            int32_t $a0_11 = *(arg5 + $v1_7);
            int32_t $a1_8 = *(arg5 + $v1_7 + 4);
            void* $v1_8 = arg4 + $v1_7;
            uint32_t $v0_1 = $v0 >> (_AePointPos_1 & 0x1f);
            void* $s1_1 = arg4 + $v1_7 + 4;
            
            if ($a1_8 >= $a0_11)
            {
                int32_t $a2_5 = *$v1_8;
                int32_t $v0_4 = $a2_5 - $v0_1;
                int32_t $a1_12 = *$s1_1;
                int32_t $s4_5 = $a2_5 - $a1_12;
                
                if ($a2_5 < $v0_1)
                    $v0_4 = $v0_1 - $a2_5;
                
                
                if ($a1_12 >= $a2_5)
                    $s4_5 = $a1_12 - $a2_5;
                
                $a0_3 = $v0_4 * ($a1_8 - $a0_11) / $s4_5 + $a0_11;
            }
            else
            {
                int32_t $a2_3 = *$v1_8;
                int32_t $v0_2 = $a2_3 - $v0_1;
                int32_t $a1_10 = *$s1_1;
                int32_t $s4_2 = $a2_3 - $a1_10;
                
                if ($a2_3 < $v0_1)
                    $v0_2 = $v0_1 - $a2_3;
                
                
                if ($a1_10 >= $a2_3)
                    $s4_2 = $a1_10 - $a2_3;
                
                $a0_3 = $a0_11 - $v0_2 * ($a0_11 - $a1_8) / $s4_2;
            }
        }
        else
            $a0_3 = arg5[9];
    }
    else
        $a0_3 = *arg5;
    
    uint32_t $v1_13 = *(arg1 + 0xc);
    uint32_t $s7 = *arg6;
    int32_t $t0_2 = *(arg1 + 0x10);
    int32_t $s5_1 = *arg7;
    int32_t $t1 = *(arg1 + 0x14);
    int32_t $fp = *arg8;
    uint32_t $s3_2 = *arg9;
    int32_t $s2_1 = *arg10;
    int32_t $s1_2 = *arg11;
    int32_t $s4_7 = $a0_3 << (_AePointPos_1 & 0x1f);
    
    if ($s4_7 != fix_point_mult3_32(_AePointPos_1, $t0_2, $t1) || data_b0e08_2 || !data_b0e0c_3)
    {
        int32_t $a0_13 = data_c46ec;
        data_b0e0c = 1;
        data_b0e08 = 0;
        
        if ($a0_13)
        {
            if ($a0_13 != 1)
            {
                $s1_2 = 0x400;
                $s2_1 = 0x400;
                $s3_2 = 1;
            }
            else
            {
                uint32_t $v1_15 = data_c46f8;
                uint32_t $v1_16;
                
                if ($s3_2 >= $v1_15)
                    $v1_16 = $s3_2 << (_AePointPos_1 & 0x1f);
                else
                {
                    if ($v1_15 >= $s7)
                        $v1_15 = $s7;
                    
                    $s3_2 = $v1_15;
                    $v1_16 = $s3_2 << (_AePointPos_1 & 0x1f);
                }
                
                data_c46f8_2 = $s3_2;
                
                if (fix_point_mult3_32(_AePointPos_1, $v1_16, $s2_1) < $s4_7)
                {
                    int32_t $v0_36 = fix_point_div_32(_AePointPos_1, $s4_7, $v1_16);
                        int32_t $v0_42 = fix_point_div_32(_AePointPos_1, $s4_7, 
                    
                    if (fix_point_mult2_32(_AePointPos_1, $s5_1, $s1_2) < $v0_36)
                    {
                            fix_point_mult2_32(_AePointPos_1, $v1_16, $s5_1));
                        
                        if ($v0_42 >= $fp)
                            $v0_42 = $fp;
                        
                        $s1_2 = $v0_42;
                        $s2_1 = $s5_1;
                    }
                    else
                        $s2_1 = fix_point_div_32(_AePointPos_1, $s4_7, 
                            fix_point_mult2_32(_AePointPos_1, $v1_16, $s1_2));
                }
            }
            
            arg2[3] = $s3_2;
        }
        else
        {
            int32_t $v0_15 = $s3_2 << (_AePointPos_1 & 0x1f);
            int32_t $v0_16 = fix_point_mult3_32(_AePointPos_1, $v0_15, $s2_1);
                int32_t $v0_18 = fix_point_div_32(_AePointPos_1, $s4_7, $v0_16);
                uint32_t $s3_3 =
                int32_t $s7_1 = fix_point_div_32(_AePointPos_1, $s5_1, $s2_1);
                int32_t $fp_1 = fix_point_div_32(_AePointPos_1, $fp, $s1_2);
                int32_t $v0_22 = 1 << (_AePointPos_1 & 0x1f);
            
            if ($v0_16 >= $s4_7)
                arg2[3] = $s3_2;
            else
            {
                    fix_point_div_32(_AePointPos_1, $s7 << (_AePointPos_1 & 0x1f), $v0_15);
                fix_point_div_32(_AePointPos_1, $v0_22, $v0_16);
                int32_t $a2_17;
                
                if ($s3_3 < $v0_18)
                {
                    if (fix_point_mult2_32(_AePointPos_1, $s3_3, $s7_1) >= $v0_18)
                        goto label_61f48;
                    
                    $a2_17 = $v0_15;
                    
                    if (fix_point_mult3_32(_AePointPos_1, $s3_3, $s7_1) >= $v0_18)
                    {
                        $fp_1 = fix_point_div_32(_AePointPos_1, $v0_18, 
                            fix_point_mult2_32(_AePointPos_1, $s3_3, $s7_1));
                        $a2_17 = $v0_15;
                    }
                }
                else
                {
                    $s3_3 = $v0_18 >> (_AePointPos_1 & 0x1f) << (_AePointPos_1 & 0x1f);
                label_61f48:
                    $s7_1 = fix_point_div_32(_AePointPos_1, $v0_18, $s3_3);
                    $fp_1 = $v0_22;
                    $a2_17 = $v0_15;
                }
                
                $s3_2 = fix_point_mult2_32(_AePointPos_1, $s3_3, $a2_17) >> (_AePointPos_1 & 0x1f);
                $s2_1 = fix_point_mult2_32(_AePointPos_1, $s7_1, $s2_1);
                $s1_2 = fix_point_mult2_32(_AePointPos_1, $fp_1, $s1_2);
                arg2[3] = $s3_2;
            }
        }
    }
    else
    {
        $s1_2 = $t1;
        $s2_1 = $t0_2;
        arg2[3] = $v1_13;
    }
    
    arg2[4] = $s2_1;
    arg2[5] = $s1_2;
    return 0;
}

