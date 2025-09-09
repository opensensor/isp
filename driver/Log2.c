#include "include/main.h"


  int32_t Log2(int32_t arg1) __attribute__((pure))

{
    int32_t result = 0;
        int32_t $v1_1 = 1;
        int32_t $a2_1 = 1;
            int32_t $t0_1 = $a2_1 < result + 1 ? 1 : 0;
    
    while (true)
    {
        
        while (true)
        {
            $a2_1 += 1;
            
            if (!$t0_1)
                break;
            
            $v1_1 <<= 1;
        }
        
        if ($v1_1 >= arg1)
            return result;
        
        if ((uintptr_t)result == 0xb)
            break;
        
        result += 1;
    }
    
    return 0xb;
}

