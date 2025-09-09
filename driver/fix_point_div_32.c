#include "include/main.h"


  int32_t fix_point_div_32(int32_t arg1, int32_t arg2, int32_t arg3)

{
    int32_t $s0 = 0;
    int32_t $a1 = arg2 % arg3;
    int32_t $v0_1 = 0;
    
    while ($v0_1 != arg1)
    {
        $a1 <<= 1;
        $s0 <<= 1;
        
        if (arg3 >= $a1)
        {
            if (arg3 == $a1)
            {
                $s0 = ($s0 | 1) << ((arg1 - 1 - $v0_1) & 0x1f);
                break;
            }
            
            $v0_1 += 1;
        }
        else
        {
            $s0 |= 1;
            $a1 -= arg3;
            $v0_1 += 1;
        }
    }
    
    return $s0 | __ashldi3(arg2 / arg3, 0, arg1);
}

