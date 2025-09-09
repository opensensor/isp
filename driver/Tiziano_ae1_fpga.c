#include "include/main.h"


  int32_t Tiziano_ae1_fpga(void* arg1, void* arg2, void* arg3, void* arg4, int32_t arg5, int32_t arg6)

{
    char var_40[0x3c];
        char arg_10[0x8];
    
    for (int32_t i = 0; (uintptr_t)i < 0x38; i += 1)
    {
        var_40[i] = arg_10[i];
    }
    
    ae1_weight_mean2(arg1, arg2, arg3, arg4, var_40_27);
    /* tailcall */
    return tisp_wdr_rx_ae1_infm(arg5, arg6);
}

