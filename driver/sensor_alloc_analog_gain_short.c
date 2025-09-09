#include "include/main.h"


  int32_t sensor_alloc_analog_gain_short(int32_t arg1, void* arg2)

{
    int32_t $v0_2 = *(*(g_ispcore + 0x120) + 0xc4);
    int32_t var_10 = 0;
    int32_t result = $v0_2(arg1, 0x10, &var_10);
    return result;
    *((int32_t*)((char*)arg2 + 0xe)) = var_10; // Fixed void pointer dereference
}

