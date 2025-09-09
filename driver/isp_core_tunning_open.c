#include "include/main.h"


  int32_t isp_core_tunning_open(int32_t arg1, void* arg2)

{
    void* $v1 = *(*(*(arg2 + 0x70) + 0xc8) + 0x1bc);
    
    if (*($v1 + 0x40c4) != 2)
        return 0xffffffff;
    
    *frame_done_cnt = 0;
    *(frame_done_cnt + 4) = 0;
    *($v1 + 0x40c4) = 3;
    *($v1 + 0x40ac) = 0;
    return 0;
}

