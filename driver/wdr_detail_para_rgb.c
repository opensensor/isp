#include "include/main.h"


  int32_t wdr_detail_para_rgb(int32_t* arg1, int32_t* arg2, void* arg3)

{
    int32_t $v0 = *arg2;
    int32_t $t6_1 = $v0 - arg2[2];
    int32_t $v0_1 = arg2[3];
    int32_t $t5_1 = $v0_1 - arg2[5];
    int32_t $a3 = $t6_1;
    int32_t $t1 = 0;
    int32_t $t2 = 0;
    int32_t $v1_1;
    
    while (true)
    {
        $v1_1 = $a3 - $t6_1;
        
        if ($v0 + arg2[1] < $a3)
            break;
        
        int32_t $v0_3 = *(arg3 + 0x3fc);
        void* $v1_2 = arg3;
        
        for (int32_t i = 0x1f; i != 0x100f; )
        {
            int32_t $a1 = *($v1_2 + 4);
            
            if ($a3 < i)
            {
                $v0_3 = (($a1 << 0xc) - (((($a1 - *$v1_2) * (i - $a3)) << 0xc) + 8) / 0x10 + 0x800)
                    / 0x1000;
                break;
            }
            
            i += 0x10;
            $v1_2 += 4;
        }
        
        $t2 += $a3;
        $t1 += $v0_3;
        $a3 += 1;
    }
    
    int32_t $t6_2 = $t5_1;
    int32_t $v0_11 = 0;
    int32_t $a1_2 = 0;
    int32_t $a3_1;
    
    while (true)
    {
        $a3_1 = $t6_2 - $t5_1;
        
        if ($v0_1 + arg2[4] < $t6_2)
            break;
        
        int32_t $a3_2 = *(arg3 + 0x3fc);
        void* $t3_2 = arg3;
        
        for (int32_t i_1 = 0x1f; i_1 != 0x100f; )
        {
            int32_t $t0_2 = *($t3_2 + 4);
            
            if ($t6_2 < i_1)
            {
                $a3_2 = (($t0_2 << 0xc) - (((($t0_2 - *$t3_2) * (i_1 - $t6_2)) << 0xc) + 8) / 0x10
                    + 0x800) / 0x1000;
                break;
            }
            
            i_1 += 0x10;
            $t3_2 += 4;
        }
        
        $a1_2 += $t6_2;
        $v0_11 += $a3_2;
        $t6_2 += 1;
    }
    
    int32_t $lo_5 = $v1_1 / 2;
    int32_t $lo_6 = ($t2 + $lo_5) / $v1_1;
    int32_t $lo_7 = ($t1 + $lo_5) / $v1_1;
    *arg1 = $lo_6;
    int32_t $lo_8 = $a3_1 / 2;
    arg1[1] = $lo_7;
    int32_t $a1_5 = ($a1_2 + $lo_8) / $a3_1 - $lo_6;
    int32_t result = (((($v0_11 + $lo_8) / $a3_1 - $lo_7) << 0xc) + $a1_5 / 2) / $a1_5;
    arg1[2] = result;
    return result;
}

