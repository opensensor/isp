#include "include/main.h"


  int32_t sensor_alloc_integration_time_short(int32_t arg1, void* arg2)

{
    void* $v1_1 = *(g_ispcore + 0x120);
    int32_t $a1 = *($v1_1 + 0xd0);
    int32_t var_10_24 = 0;
    int32_t result;
    
    if ($a1)
    {
        result = (*($v1_1 + 0xd4))(arg1, 0, &var_10_25);
        *(arg2 + 0x12) = var_10_26;
    }
    else
    {
        result = arg1;
        *(arg2 + 0x12) = arg1;
    }
    
    return result;
}

