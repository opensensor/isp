#include "include/main.h"


  int32_t private_leading_one_position_64(uint32_t arg1, uint32_t arg2) __attribute__((pure))

{
    int32_t result = 0;
    
    if (arg2)
    {
        arg1 = arg2;
        result = 0x20;
        arg2 = 0;
    }
    
    int32_t $v1_1 = (uintptr_t)arg1 < 0x100 ? 1 : 0;
    
    if (arg1 >= isp_printf)
    {
        arg1 u>>= 0x10;
        arg2 = 0;
        result += 0x10;
        $v1_1 = (uintptr_t)arg1 < 0x100 ? 1 : 0;
    }
    
    int32_t $v1_2 = (uintptr_t)arg1 < 0x10 ? 1 : 0;
    
    if (!$v1_1)
    {
        arg1 u>>= 8;
        arg2 = 0;
        result += 8;
        $v1_2 = (uintptr_t)arg1 < 0x10 ? 1 : 0;
    }
    
    int32_t $v1_3 = arg1 < 4 ? 1 : 0;
    
    if (!$v1_2)
    {
        arg1 u>>= 4;
        arg2 = 0;
        result += 4;
        $v1_3 = arg1 < 4 ? 1 : 0;
    }
    
    int32_t $a0;
    
    if ($v1_3)
    {
        $a0 = arg1 < 2 ? 1 : 0;
    label_19040:
        
        if ($a0)
            return result;
    }
    else
    {
        result += 2;
        
        if (!(arg2 >> 2))
        {
            $a0 = (arg2 << 0x1e | arg1 >> 2) < 2 ? 1 : 0;
            goto label_19040;
        }
    }
    return result + 1;
}

