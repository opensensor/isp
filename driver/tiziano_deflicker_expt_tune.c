#include "include/main.h"


  int32_t tiziano_deflicker_expt_tune(int32_t arg1, int32_t arg2, int32_t arg3, int32_t arg4)

{
    tiziano_deflicker_expt(arg1, arg2, arg3, arg4, &_deflick_lut, &_nodes_num);
    return &data_b0000_19;
}

