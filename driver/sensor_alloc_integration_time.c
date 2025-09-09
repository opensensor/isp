#include "include/main.h"


  int32_t sensor_alloc_integration_time(int32_t arg1, void* arg2)

{
    int32_t $v1_2 = *(*(g_ispcore + 0x120) + 0xd0);
    int32_t var_10 = 0;
    int32_t result;
    
    if ($v1_2)
    {
        result = $v1_2(arg1, 0, &var_10);
        *(((void**)((char*)arg2 + 0x10))) = var_10; // Fixed void pointer dereference
    }
    else
    {
        result = arg1;
        *(((void**)((char*)arg2 + 0x10))) = arg1; // Fixed void pointer dereference
    }
    
    return result;
}

