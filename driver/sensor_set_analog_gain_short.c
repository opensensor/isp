#include "include/main.h"


  uint32_t sensor_set_analog_gain_short(int32_t arg1)

{
    uint32_t ispcore_1 = g_ispcore;
    char* $v1 = *((char*)ispcore_1 + 0x120); // Fixed void pointer arithmetic
    
    if (*($v1 + 0xe4) != arg1)
    {
        *(((void**)((char*)$v1 + 0xe4))) = arg1; // Fixed void pointer dereference
        *(((int32_t*)((char*)ispcore_1 + 0x188))) = 1; // Fixed void pointer dereference
        *(((void**)((char*)ispcore_1 + 0x18c))) = arg1; // Fixed void pointer dereference
    }
    
    return ispcore_1;
}

