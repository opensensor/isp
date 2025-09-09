#include "include/main.h"


  uint32_t sensor_set_integration_time_short(int16_t arg1)

{
    uint32_t ispcore_1 = g_ispcore;
    uint32_t $a0 = arg1;
    int32_t* $v1 = (int32_t*)((char*)ispcore_1  + 0x120); // Fixed void pointer arithmetic
    
    if ($a0 != *($v1 + 0xdc))
    {
        *((int32_t*)((char*)$v1 + 0xdc)) = $a0; // Fixed void pointer dereference
        *((int32_t*)((char*)ispcore_1 + 0x1a0)) = 1; // Fixed void pointer dereference
        *((int32_t*)((char*)ispcore_1 + 0x1a4)) = $a0; // Fixed void pointer dereference
    }
    
    return ispcore_1;
}

