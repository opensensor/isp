#include "include/main.h"


  int32_t fix_point_mult3(int32_t arg1, int32_t arg2, int32_t arg3, int32_t arg4, int32_t arg5, int32_t arg6, int32_t arg7, int32_t arg8)

{
    int32_t $v0_1;
    int32_t $v1_1;
    int32_t $a1;
    $v0_1 = fix_point_mult2(arg1, arg2, arg3, arg4, arg5, arg6);
    /* tailcall */
    return fix_point_mult2(arg1, $a1, $v0_1, $v1_1, arg7, arg8);
}

