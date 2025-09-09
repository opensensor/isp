#include "include/main.h"


  int32_t tiziano_bcsh_Tccm_RGB2YUV(int32_t* arg1, int32_t* arg2)

{
    void var_38;
    memcpy(&var_38, 0x7b8a4, 0x24);
    tiziano_bcsh_Tccm_RGBYUV(&var_38, &MMatrix, arg2, &MinvMatrix);
    return tiziano_bcsh_para2reg(arg1, &var_38);
}

