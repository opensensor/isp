#include "include/main.h"


  uint32_t sensor_set_digital_gain(int32_t arg1)

{
    uint32_t ispcore_1 = g_ispcore;
    int32_t* $v1 = (int32_t*)((char*)ispcore_1  + 0x120); // Fixed void pointer arithmetic
    
    if (*($v1 + 0xa0) != arg1)
    {
        *((int32_t*)((char*)$v1 + 0xa0)) = arg1; // Fixed void pointer dereference
        *((int32_t*)((char*)ispcore_1 + 0x190)) = 1; // Fixed void pointer dereference
        *((int32_t*)((char*)ispcore_1 + 0x194)) = arg1; // Fixed void pointer dereference
    }
    
    return ispcore_1;
}

