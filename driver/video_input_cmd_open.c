#include "include/main.h"


  int32_t video_input_cmd_open(int32_t arg1, int32_t arg2)

{
    /* tailcall */
    return private_single_open_size(arg2, video_input_cmd_show, PDE_DATA(), 0x200);
}

