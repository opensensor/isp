#include "include/main.h"


  int32_t __fill_v4l2_buffer(void* arg1, void* arg2)

{
    int32_t* $s2 = (int32_t*)((char*)arg1  + 0x44); // Fixed void pointer arithmetic
    int32_t $v1 = *(arg2 + 0xc);
    int32_t $v0_2 = $v1 & 0xffff1bb8;
    int32_t result = $v0_2 | *($s2 + 0x14);
    int32_t $v1_2 = *(arg1 + 0x48);
    memcpy(arg2, arg1, 0x34);
    *((int32_t*)((char*)arg2 + 0x3c)) = *(arg1 + 0x3c); // Fixed void pointer dereference
    *((int32_t*)((char*)arg2 + 0x40)) = *(arg1 + 0x40); // Fixed void pointer dereference
    *((int32_t*)((char*)arg2 + 0xc)) = $v0_2; // Fixed void pointer dereference
    *((int32_t*)((char*)arg2 + 0xc)) = result; // Fixed void pointer dereference
    
    if ($v1_2 == 3)
    {
        result |= 2;
        *((int32_t*)((char*)arg2 + 0xc)) = result; // Fixed void pointer dereference
    }
    else if ($v1_2 >= 4)
    {
        if ($v1_2 == 4)
        {
            result = *(arg2 + 0xc) | 4;
            *((int32_t*)((char*)arg2 + 0xc)) = result; // Fixed void pointer dereference
        }
        else
        {
            result |= 0x40;
            
            if ($v1_2 == 5)
            {
                *((int32_t*)((char*)arg2 + 0xc)) = result; // Fixed void pointer dereference
                result = *(arg2 + 0xc) | 4;
                *((int32_t*)((char*)arg2 + 0xc)) = result; // Fixed void pointer dereference
            }
        }
    }
    else if ($v1_2 == 1)
    {
        result |= 2;
        *((int32_t*)((char*)arg2 + 0xc)) = result; // Fixed void pointer dereference
    }
    
    return result;
}

