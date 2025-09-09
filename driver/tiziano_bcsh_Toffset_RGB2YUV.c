#include "include/main.h"


  int32_t tiziano_bcsh_Toffset_RGB2YUV(int32_t* arg1, int32_t* arg2)

{
    int32_t var_14 = *arg2 - 0x400;
    int32_t var_10 = arg2[1] - 0x400;
    int32_t var_c = arg2[2] - 0x400;
    int32_t var_20 = 0;
    int32_t var_1c = 0;
    int32_t var_18 = 0;
    tiziano_bcsh_Toffset_RGBYUV(&var_20, &MMatrix, &var_14);
    *arg1 = var_20 + 0x400;
    arg1[1] = var_1c + 0x400;
    arg1[2] = var_18 + 0x400;
    return var_18 + 0x400;
}

