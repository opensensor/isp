#include "include/main.h"


  int32_t sensor_alloc_integration_time_short(int32_t arg1, void* arg2)

{
    int32_t* $v1_1 = (int32_t*)((char*)g_ispcore  + 0x120); // Fixed void pointer arithmetic
    int32_t $a1 = *($v1_1 + 0xd0);
    int32_t var_10 = 0;
    int32_t result;
    
    if ($a1)
    {
        result = (*($v1_1 + 0xd4))(arg1, 0, &var_10);
        *((int32_t*)((char*)arg2 + 0x12)) = var_10; // Fixed void pointer dereference
    }
    else
    {
        result = arg1;
        *((int32_t*)((char*)arg2 + 0x12)) = arg1; // Fixed void pointer dereference
    }
    
    return result;
}

