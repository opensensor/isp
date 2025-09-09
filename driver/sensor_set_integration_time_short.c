#include "include/main.h"


  uint32_t sensor_set_integration_time_short(int16_t arg1)

{
    uint32_t ispcore_1 = g_ispcore;
    uint32_t $a0 = arg1;
    void* $v1 = *(ispcore_1 + 0x120);
    
    if ($a0 != *($v1 + 0xdc))
    {
        *($v1 + 0xdc) = $a0;
        *(ispcore_1 + 0x1a0) = 1;
        *(ispcore_1 + 0x1a4) = $a0;
    }
    
    return ispcore_1;
}

