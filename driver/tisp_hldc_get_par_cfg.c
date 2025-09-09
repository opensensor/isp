#include "include/main.h"


  int32_t tisp_hldc_get_par_cfg(int32_t arg1, int32_t* arg2)

{
    int32_t var_10 = 0;
    *arg2 = 0;
    tisp_hldc_param_array_get(0x3ac, arg1, &var_10);
    *arg2 += var_10;
    return 0;
}

