#include "include/main.h"


  int32_t fix_point_div(int32_t arg1, int32_t arg2, int32_t arg3, int32_t arg4, int32_t arg5, int32_t arg6)

{
    /* tailcall */
    return fix_point_div_64(arg1, arg2, arg3, arg4, arg5, arg6);
}

