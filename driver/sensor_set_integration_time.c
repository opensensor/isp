#include "include/main.h"


  uint32_t sensor_set_integration_time(int16_t arg1)

{
    uint32_t ispcore_1 = g_ispcore;
    uint32_t $a0 = arg1;
    char* $v1 = *((char*)ispcore_1 + 0x120); // Fixed void pointer arithmetic
    
    if ($a0 != *($v1 + 0xac))
    {
        *(((void**)((char*)$v1 + 0xec))) = (0xffff0000 & *($v1 + 0xec)) + $a0; // Fixed void pointer dereference
        *(((void**)((char*)$v1 + 0xac))) = $a0; // Fixed void pointer dereference
        *(((int32_t*)((char*)ispcore_1 + 0x198))) = 1; // Fixed void pointer dereference
        *(((void**)((char*)ispcore_1 + 0x19c))) = $a0; // Fixed void pointer dereference
        *(((int32_t*)((char*)ispcore_1 + 0x1b0))) = 1; // Fixed void pointer dereference
        *(((void**)((char*)ispcore_1 + 0x1b4))) = *($v1 + 0xec); // Fixed void pointer dereference
    }
    
    return ispcore_1;
}

