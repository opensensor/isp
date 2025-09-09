#include "include/main.h"


  uint32_t sensor_get_normal_fps()

{
    int32_t $v0 = *(g_ispcore + 0x12c);
    uint32_t $v1 = $v0 >> 0x10;
    int32_t $v0_1 = $v0 & 0xffff;
    return (($v1 % $v0_1) << 8) / $v0_1 + (($v1 / $v0_1) << 8);
}

