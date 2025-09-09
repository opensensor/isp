#include "include/main.h"


  uint32_t sensor_set_analog_gain(int32_t arg1)

{
    uint32_t ispcore_1 = g_ispcore;
    void* $v1 = *(ispcore_1 + 0x120);
    
    if (*($v1 + 0x9c) != arg1)
    {
        *($v1 + 0xec) = *($v1 + 0xec) | arg1 << 0x10;
        *($v1 + 0x9c) = arg1;
        *(ispcore_1 + 0x180) = 1;
        *(ispcore_1 + 0x184) = arg1;
        *(ispcore_1 + 0x1b0) = 1;
        *(ispcore_1 + 0x1b4) = *($v1 + 0xec);
    }
    
    return ispcore_1;
}

