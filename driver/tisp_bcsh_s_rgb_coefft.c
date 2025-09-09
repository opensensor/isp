#include "include/main.h"


  int32_t tisp_bcsh_s_rgb_coefft(int16_t* arg1)

{
    char* tisp_BCSH_au32OffsetRGB_now_1 = (char*)(tisp_BCSH_au32OffsetRGB_now); // Fixed void pointer assignment
    **&tisp_BCSH_au32OffsetRGB_now = *arg1;
    *((int32_t*)((char*)tisp_BCSH_au32OffsetRGB_now_1 + 4)) = arg1[1]; // Fixed void pointer dereference
    *((int32_t*)((char*)tisp_BCSH_au32OffsetRGB_now_1 + 8)) = arg1[2]; // Fixed void pointer dereference
    /* tailcall */
    return tiziano_bcsh_update();
}

