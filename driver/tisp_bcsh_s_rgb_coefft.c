#include "include/main.h"


  int32_t tisp_bcsh_s_rgb_coefft(int16_t* arg1)

{
    **&tisp_BCSH_au32OffsetRGB_now = *arg1;
    void* tisp_BCSH_au32OffsetRGB_now_1 = tisp_BCSH_au32OffsetRGB_now;
    *(tisp_BCSH_au32OffsetRGB_now_1 + 4) = arg1[1];
    *(tisp_BCSH_au32OffsetRGB_now_1 + 8) = arg1[2];
    /* tailcall */
    return tiziano_bcsh_update();
}

