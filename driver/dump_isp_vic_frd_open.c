#include "include/main.h"


  int32_t dump_isp_vic_frd_open(int32_t arg1, int32_t arg2)

{
    /* tailcall */
    return private_single_open_size(arg2, isp_vic_frd_show, PDE_DATA(), 0x400);
}

