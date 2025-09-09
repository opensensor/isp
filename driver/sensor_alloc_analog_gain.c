#include "include/main.h"


  int32_t sensor_alloc_analog_gain(int32_t arg1, int16_t* arg2)

{
    int32_t $v0_2 = *(*(g_ispcore + 0x120) + 0xc0);
    int32_t var_10 = 0;
    int32_t result = $v0_2(arg1, 0x10, &var_10);
    *arg2 = var_10;
    return result;
}

