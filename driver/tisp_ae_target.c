#include "include/main.h"


  int32_t tisp_ae_target(int32_t arg1, int32_t* arg2, int32_t* arg3, int32_t arg4)

{
    int32_t* $t0 = arg2;
        return arg3[9];
    int32_t $v0_6 = 0;
    int32_t $v0_7;
    if (*arg2 << (arg4 & 0x1f) >= arg1)
        return *arg3;
    
    
    if (arg1 >= arg2[9] << (arg4 & 0x1f))
    
    
    while (true)
    {
        if (arg1 < *$t0 << (arg4 & 0x1f))
            $v0_6 += 1;
        else
        {
            if ($t0[1] << (arg4 & 0x1f) >= arg1)
            {
                $v0_7 = $v0_6 << 2;
                break;
            }
            
            $v0_6 += 1;
        }
        
        $t0 = &$t0[1];
        
        if ($v0_6 == 9)
        {
            $v0_7 = 0;
            break;
        }
    }
    
    int32_t $t0_1 = *(arg3 + $v0_7);
    int32_t $v1_8 = *(arg3 + $v0_7 + 4);
    char* $v0_8 = (char*)(arg2 + $v0_7); // Fixed void pointer assignment
    uint32_t $a0 = arg1 >> (arg4 & 0x1f);
    char* $a1 = (char*)(arg2 + $v0_7 + 4); // Fixed void pointer assignment
    
    if ($v1_8 >= $t0_1)
    {
        int32_t $t1_3 = *$v0_8;
        int32_t $a0_3 = $t1_3 - $a0;
        int32_t $a1_2 = *$a1;
        int32_t $v0_13 = $t1_3 - $a1_2;
        
        if ($t1_3 < $a0)
            $a0_3 = $a0 - $t1_3;
        
        
        if ($a1_2 >= $t1_3)
            $v0_13 = $a1_2 - $t1_3;
        
        return ($v1_8 - $t0_1) * $a0_3 / $v0_13 + $t0_1;
    }
    
    int32_t $t1_1 = *$v0_8;
    int32_t $a0_1 = $t1_1 - $a0;
    
    if ($t1_1 < $a0)
        $a0_1 = $a0 - $t1_1;
    
    int32_t $a1_1 = *$a1;
    int32_t $v0_10 = $t1_1 - $a1_1;
    
    if ($a1_1 >= $t1_1)
        $v0_10 = $a1_1 - $t1_1;
    
    return $t0_1 - ($t0_1 - $v1_8) * $a0_1 / $v0_10;
}

