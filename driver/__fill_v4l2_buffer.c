#include "include/main.h"


  int32_t __fill_v4l2_buffer(void* arg1, void* arg2)

{
    void* $s2 = *(arg1 + 0x44);
    memcpy(arg2, arg1, 0x34);
    int32_t $v1 = *(arg2 + 0xc);
    *(arg2 + 0x3c) = *(arg1 + 0x3c);
    *(arg2 + 0x40) = *(arg1 + 0x40);
    int32_t $v0_2 = $v1 & 0xffff1bb8;
    *(arg2 + 0xc) = $v0_2;
    int32_t result = $v0_2 | *($s2 + 0x14);
    *(arg2 + 0xc) = result;
    int32_t $v1_2 = *(arg1 + 0x48);
    
    if ($v1_2 == 3)
    {
        result |= 2;
        *(arg2 + 0xc) = result;
    }
    else if ($v1_2 >= 4)
    {
        if ($v1_2 == 4)
        {
            result = *(arg2 + 0xc) | 4;
            *(arg2 + 0xc) = result;
        }
        else
        {
            result |= 0x40;
            
            if ($v1_2 == 5)
            {
                *(arg2 + 0xc) = result;
                result = *(arg2 + 0xc) | 4;
                *(arg2 + 0xc) = result;
            }
        }
    }
    else if ($v1_2 == 1)
    {
        result |= 2;
        *(arg2 + 0xc) = result;
    }
    
    return result;
}

