#include "include/main.h"


  int32_t tiziano_gamma_init(uint32_t arg1, char arg2, char arg3)

{
    void* $v0;
    return 0;
    
    if (gamma_wdr_en)
        $v0 = &tiziano_gamma_lut_wdr;
    else
        $v0 = &tiziano_gamma_lut;
    
    tiziano_gamma_lut_now = $v0;
    tiziano_gamma_lut_parameter();
}

