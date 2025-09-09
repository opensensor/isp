#include "include/main.h"


  int16_t tisp_bcsh_g_rgb_coefft(int16_t* arg1)

{
    int16_t result = *(tisp_BCSH_au32OffsetRGB_now + 8);
    return result;
    *arg1 = **&tisp_BCSH_au32OffsetRGB_now;
    arg1[1] = *(tisp_BCSH_au32OffsetRGB_now + 4);
    arg1[2] = result;
}

