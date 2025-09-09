#include "include/main.h"


  int32_t tisp_simple_intp(int32_t arg1, int32_t arg2, void* arg3)

{
    void* $a2 = arg3 + (arg1 << 2);
    int32_t $a3 = *$a2;
    int32_t result = *($a2 + 4);
    if (arg1 >= 8)
        return *(arg3 + 0x20);
    
    
    if ($a3 != result)
    {
        int32_t $v0_1;
        int32_t $a0_1;
        
        if ($a3 >= result)
        {
            $v0_1 = $a3 - result;
            $a0_1 = 1;
        }
        else
        {
            $v0_1 = result - $a3;
            $a0_1 = 0;
        }
        
        int32_t $a1 = $v0_1 * arg2;
        int32_t $a1_3 = ($a1 >> 0x10) + (($a1 & 0x8000) >> 0xf);
        result = $a3 - $a1_3;
        
        if (!$a0_1)
            return $a3 + $a1_3;
    }
    
    return result;
}

