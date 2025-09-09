#include "include/main.h"


  int32_t tisp_gamma_set_par_cfg(int32_t arg1)

{
    int32_t var_18_75 = 0;
    tisp_gamma_param_array_set(0x3c, arg1, &var_18_76);
    tisp_gamma_param_array_set(0x3d, arg1 + var_18_77, &var_18_78);
    return 0;
}

