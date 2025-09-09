#include "include/main.h"


  uint32_t sensor_set_analog_gain(int32_t arg1)

{
    uint32_t ispcore_1 = g_ispcore;
    int32_t* $v1 = (int32_t*)((char*)ispcore_1  + 0x120); // Fixed void pointer arithmetic
    
    if (*($v1 + 0x9c) != arg1)
    {
        *((int32_t*)((char*)$v1 + 0xec)) = *($v1 + 0xec) | arg1 << 0x10; // Fixed void pointer dereference
        *((int32_t*)((char*)$v1 + 0x9c)) = arg1; // Fixed void pointer dereference
        *((int32_t*)((char*)ispcore_1 + 0x180)) = 1; // Fixed void pointer dereference
        *((int32_t*)((char*)ispcore_1 + 0x184)) = arg1; // Fixed void pointer dereference
        *((int32_t*)((char*)ispcore_1 + 0x1b0)) = 1; // Fixed void pointer dereference
        *((int32_t*)((char*)ispcore_1 + 0x1b4)) = *($v1 + 0xec); // Fixed void pointer dereference
    }
    
    return ispcore_1;
}

