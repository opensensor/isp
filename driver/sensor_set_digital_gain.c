#include "include/main.h"


  uint32_t sensor_set_digital_gain(int32_t arg1)

{
    uint32_t ispcore_1 = g_ispcore;
    void* $v1 = *(ispcore_1 + 0x120);
    
    if (*($v1 + 0xa0) != arg1)
    {
        *($v1 + 0xa0) = arg1;
        *(ispcore_1 + 0x190) = 1;
        *(ispcore_1 + 0x194) = arg1;
    }
    
    return ispcore_1;
}

