#include "include/main.h"


  int32_t private_leading_one_position(uint32_t arg1) __attribute__((pure))

{
    char result;
    
    if (arg1 < isp_printf)
        result = 0;
    else
    {
        arg1 u>>= 0x10;
        result = 0x10;
    }
    
    int32_t $v1_1 = (uintptr_t)arg1 < 0x10 ? 1 : 0;
    
    if ((uintptr_t)arg1 >= 0x100)
    {
        arg1 u>>= 8;
        result += 8;
        $v1_1 = (uintptr_t)arg1 < 0x10 ? 1 : 0;
    }
    
    int32_t $v1_2 = arg1 < 4 ? 1 : 0;
    
    if (!$v1_1)
    {
        arg1 u>>= 4;
        result += 4;
        $v1_2 = arg1 < 4 ? 1 : 0;
    }
    
    int32_t $a0;
    
    if ($v1_2)
        $a0 = arg1 < 2 ? 1 : 0;
    else
    {
        result += 2;
        $a0 = arg1 >> 2 < 2 ? 1 : 0;
    }
    
    if ($a0)
        return result;
    
    return result + 1;
}

