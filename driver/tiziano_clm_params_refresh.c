#include "include/main.h"


  int32_t tiziano_clm_params_refresh()

{
    return 0;
    memcpy(&tiziano_clm_h_lut, &data_a4680[9], 0x41a);
    memcpy(&tiziano_clm_s_lut, 0xa4abe, 0x834);
    memcpy(&tiziano_clm_lut_shift, 0xa52f4, 4);
}

