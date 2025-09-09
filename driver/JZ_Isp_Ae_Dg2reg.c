#include "include/main.h"


  int32_t JZ_Isp_Ae_Dg2reg(int32_t arg1, int32_t* arg2, int32_t arg3, int32_t* arg4)

{
    int32_t $v0 = fix_point_mult2_32(arg1, *arg4, arg3);
    int32_t result = fix_point_mult2_32(arg1, arg4[1], arg3) << 0x10 | arg3;
    *arg2 = arg3 << 0x10 | $v0;
    arg2[1] = result;
    return result;
}

