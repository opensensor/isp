#include "include/main.h"


  uint32_t sensor_set_analog_gain_short(int32_t arg1)

{
    uint32_t ispcore_1 = g_ispcore;
    void* $v1 = *(ispcore_1 + 0x120);
    
    if (*($v1 + 0xe4) != arg1)
    {
        *($v1 + 0xe4) = arg1;
        *(ispcore_1 + 0x188) = 1;
        *(ispcore_1 + 0x18c) = arg1;
    }
    
    return ispcore_1;
}

