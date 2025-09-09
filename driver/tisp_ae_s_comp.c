#include "include/main.h"


  int32_t tisp_ae_s_comp(char arg1)

{
    uint32_t $a0 = arg1;
    int32_t $a0_2;
    ae_comp_param = 1;
    ae_comp_x = $a0;
    
    if ($(uintptr_t)a0 >= 0x81)
        $a0_2 = $a0 * 0x12c / 0x7f + ae_comp_default - 0x12e;
    else
        $a0_2 = $a0 * ae_comp_default / 0x80;
    
    data_b0c18 = $a0_2;
    data_b0e00 = 1;
    data_b0e04 = 1;
    tiziano_ae_set_hardware_param(0, &_ae_parameter, 1);
    /* tailcall */
    return tiziano_ae_set_hardware_param(1, &_ae_parameter, 1);
}

