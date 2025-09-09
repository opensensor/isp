#include "include/main.h"


  int32_t sensor_fps_control(int32_t arg1, void* arg2)

{
    uint32_t ispcore_1 = g_ispcore;
    void* $v0 = *(ispcore_1 + 0x120);
    *(arg2 + 2) = *($v0 + 0xb0);
    *(arg2 + 4) = *($v0 + 0xb2);
    *(arg2 + 0x28) = *($v0 + 0xa4);
    *(arg2 + 0x2c) = *($v0 + 0xb4);
    *(arg2 + 0x50) = *($v0 + 0xd8);
    *(arg2 + 0x54) = *($v0 + 0xda);
    *(arg2 + 0x30) = *($v0 + 0xb4);
    *(arg2 + 0x34) = *($v0 + 0xaa);
    return *(ispcore_1 + 0x12c);
}

