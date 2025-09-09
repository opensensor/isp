#include "include/main.h"


  int32_t isp_core_tunning_open(int32_t arg1, void* arg2)

{
    char* $v1 = (char*)(*(*(*(arg2 + 0x70) + 0xc8) + 0x1bc)); // Fixed void pointer assignment
    
    if (*($v1 + 0x40c4) != 2)
        return 0xffffffff;
    
    *frame_done_cnt = 0;
    *(((int32_t*)((char*)frame_done_cnt + 4))) = 0; // Fixed void pointer dereference
    *(((int32_t*)((char*)$v1 + 0x40c4))) = 3; // Fixed void pointer dereference
    *(((int32_t*)((char*)$v1 + 0x40ac))) = 0; // Fixed void pointer dereference
    return 0;
}

