#include "include/main.h"


  int32_t sensor_fps_control(int32_t arg1, void* arg2)

{
    uint32_t ispcore_1 = g_ispcore;
    char* $v0 = *((char*)ispcore_1 + 0x120); // Fixed void pointer arithmetic
    *(((void**)((char*)arg2 + 2))) = *($v0 + 0xb0); // Fixed void pointer dereference
    *(((void**)((char*)arg2 + 4))) = *($v0 + 0xb2); // Fixed void pointer dereference
    *(((void**)((char*)arg2 + 0x28))) = *($v0 + 0xa4); // Fixed void pointer dereference
    *(((void**)((char*)arg2 + 0x2c))) = *($v0 + 0xb4); // Fixed void pointer dereference
    *(((void**)((char*)arg2 + 0x50))) = *($v0 + 0xd8); // Fixed void pointer dereference
    *(((void**)((char*)arg2 + 0x54))) = *($v0 + 0xda); // Fixed void pointer dereference
    *(((void**)((char*)arg2 + 0x30))) = *($v0 + 0xb4); // Fixed void pointer dereference
    *(((void**)((char*)arg2 + 0x34))) = *($v0 + 0xaa); // Fixed void pointer dereference
    return *(ispcore_1 + 0x12c);
}

