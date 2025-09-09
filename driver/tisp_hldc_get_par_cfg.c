#include "include/main.h"


  int32_t tisp_hldc_get_par_cfg(int32_t arg1, int32_t* arg2)

{
    *arg2 = 0;
    int32_t var_10_38 = 0;
    tisp_hldc_param_array_get(0x3ac, arg1, &var_10_39);
    *arg2 += var_10_40;
    return 0;
}

