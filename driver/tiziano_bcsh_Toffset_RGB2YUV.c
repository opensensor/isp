#include "include/main.h"


  int32_t tiziano_bcsh_Toffset_RGB2YUV(int32_t* arg1, int32_t* arg2)

{
    int32_t var_14_24 = *arg2 - 0x400;
    int32_t var_10_42 = arg2[1] - 0x400;
    int32_t var_c_8 = arg2[2] - 0x400;
    int32_t var_20_151 = 0;
    int32_t var_1c_12 = 0;
    int32_t var_18_86 = 0;
    tiziano_bcsh_Toffset_RGBYUV(&var_20_152, &MMatrix, &var_14_25);
    *arg1 = var_20_153 + 0x400;
    arg1[1] = var_1c_13 + 0x400;
    arg1[2] = var_18_87 + 0x400;
    return var_18_88 + 0x400;
}

