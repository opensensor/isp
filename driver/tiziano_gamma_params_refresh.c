#include "include/main.h"


  int32_t tiziano_gamma_params_refresh()

{
    return 0;
    memcpy(&tiziano_gamma_lut, 0x97364, 0x102);
    memcpy(&tiziano_gamma_lut_wdr, 0x97466, 0x102);
}

