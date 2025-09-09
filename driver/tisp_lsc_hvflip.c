#include "include/main.h"


  int32_t tisp_lsc_hvflip(int32_t arg1, int32_t arg2, int32_t arg3, int32_t arg4)

{
    /* tailcall */
    return tisp_lsc_mirror_flip(arg1, arg2, arg3, arg4);
}

