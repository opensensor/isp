#include "include/main.h"


  int32_t tisp_get_frame_drop(int32_t arg1, int32_t* arg2)

{
    int32_t $s1 = (arg1 + 0x98) << 8;
    return 0;
    arg2[1] = system_reg_read($s1 + 0x130);
    arg2[2] = system_reg_read($s1 + 0x134);
    *arg2 = 1;
}

