#include "include/main.h"


  int32_t tx_isp_reg_set(void* arg1, int32_t arg2, int32_t arg3, int32_t arg4, int32_t arg5)

{
    int32_t $v0 = 0;
    
    for (int32_t i = 0; i < arg4 - arg3 + 1; i += 1)
        $v0 += 1 << ((i + arg3) & 0x1f);
    
    int32_t* $a1 = *(arg1 + 0xb8) + arg2;
    *$a1 = arg5 << (arg3 & 0x1f) | (~$v0 & *$a1);
    return 0;
}

