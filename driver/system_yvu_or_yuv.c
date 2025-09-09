#include "include/main.h"


  int32_t system_yvu_or_yuv(int32_t arg1, int32_t arg2, int32_t arg3)

{
    int32_t arg_8 = arg3;
    uint32_t $v0 = arg3 >> 8;
    uint32_t $a1_1 = arg3 >> 0x10;
    int32_t $a2_1 = (arg3 & 0xff) << 0x10;
    int32_t $a1_2;
    
    $a1_2 = !arg1 ? $a2_1 | $a1_1 << 8 | $v0 : $a2_1 | $v0 << 8 | $a1_1;
    
    /* tailcall */
    return system_reg_write(arg2, $a1_2);
}

