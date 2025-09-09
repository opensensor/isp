#include "include/main.h"


  int32_t tisp_gamma_wdr_en(uint32_t arg1)

{
    void* $v0;
    return 0;
    gamma_wdr_en = arg1;
    
    $v0 = arg1 ? &tiziano_gamma_lut_wdr : &tiziano_gamma_lut;
    
    tiziano_gamma_lut_now = $v0;
}

