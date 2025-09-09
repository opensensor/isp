#include "include/main.h"


  int32_t getVar(void* arg1)

{
    int32_t* $v0 = arg1 + 4;
    int32_t $a2 = 0;
    int32_t $a1 = 0;
    int32_t $v1 = *$v0;
    
    while (true)
    {
        if ($v1 >= 0xc8)
            $v0 = &$v0[1];
        else
        {
            int32_t $v1_2 = $v0[1];
            
            if ($v1_2 >= 0xc8)
                $v0 = &$v0[1];
            else
            {
                $a1 += 1;
                $a2 += ($v1_2 - *($v0 - 4) + 1) / 2;
                $v0 = &$v0[1];
            }
        }
        
        if (arg1 + 0x400 == $v0)
            break;
        
        $v1 = *$v0;
    }
    
    int32_t $t0_1 = $a1 >> 1;
    void* $a0 = arg1 + 8;
    int32_t $a3 = 1;
    int32_t $v0_1 = 0;
    
    while (true)
    {
        int32_t $v1_7 = $a3 < $a1 + 1 ? 1 : 0;
        $a3 += 1;
        
        if (!$v1_7)
            break;
        
        int32_t $lo_3 = (*$a0 - *($a0 - 8) + 1) / 2;
        $a0 += 4;
        int32_t $v1_13 = $lo_3 * 0xa - ($a2 * 0xa + $t0_1) / $a1;
        $v0_1 += $v1_13 * $v1_13;
    }
    
    return ($v0_1 + $t0_1) / $a1;
}

