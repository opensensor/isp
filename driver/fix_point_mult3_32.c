#include "include/main.h"


  int32_t fix_point_mult3_32(int32_t arg1, int32_t arg2, int32_t arg3)

{
    /* tailcall */
    return fix_point_mult2_32(arg1, arg2, arg3)();
}

