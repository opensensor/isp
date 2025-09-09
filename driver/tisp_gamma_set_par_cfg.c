#include "include/main.h"


  int32_t tisp_gamma_set_par_cfg(int32_t arg1)

{
    int32_t var_18 = 0;
    tisp_gamma_param_array_set(0x3c, arg1, &var_18);
    tisp_gamma_param_array_set(0x3d, arg1 + var_18, &var_18);
    return 0;
}

