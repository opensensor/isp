#include "include/main.h"


  int32_t tisp_gamma_get_par_cfg(int32_t arg1, int32_t* arg2)

{
    *arg2 = 0;
    int32_t var_18_70 = 0;
    tisp_gamma_param_array_get(0x3c, arg1, &var_18_71);
    int32_t $a1_1 = var_18_72;
    *arg2 += $a1_1;
    tisp_gamma_param_array_get(0x3d, arg1 + $a1_1, &var_18_73);
    *arg2 += var_18_74;
    return 0;
}

