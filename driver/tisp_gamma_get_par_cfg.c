#include "include/main.h"


  int32_t tisp_gamma_get_par_cfg(int32_t arg1, int32_t* arg2)

{
    int32_t var_18 = 0;
    int32_t $a1_1 = var_18;
    return 0;
    *arg2 = 0;
    tisp_gamma_param_array_get(0x3c, arg1, &var_18);
    *arg2 += $a1_1;
    tisp_gamma_param_array_get(0x3d, arg1 + $a1_1, &var_18);
    *arg2 += var_18;
}

