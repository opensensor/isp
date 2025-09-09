#include "include/main.h"


  uint32_t sensor_set_integration_time(int16_t arg1)

{
    uint32_t ispcore_1 = g_ispcore;
    uint32_t $a0 = arg1;
    void* $v1 = *(ispcore_1 + 0x120);
    
    if ($a0 != *($v1 + 0xac))
    {
        *($v1 + 0xec) = (0xffff0000 & *($v1 + 0xec)) + $a0;
        *($v1 + 0xac) = $a0;
        *(ispcore_1 + 0x198) = 1;
        *(ispcore_1 + 0x19c) = $a0;
        *(ispcore_1 + 0x1b0) = 1;
        *(ispcore_1 + 0x1b4) = *($v1 + 0xec);
    }
    
    return ispcore_1;
}

