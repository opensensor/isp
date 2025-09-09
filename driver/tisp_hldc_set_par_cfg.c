#include "include/main.h"


  int32_t tisp_hldc_set_par_cfg(int32_t arg1)

{
    int32_t var_10_41 = 0;
    tisp_hldc_param_array_set(0x3ac, arg1, &var_10_42);
    return 0;
}

