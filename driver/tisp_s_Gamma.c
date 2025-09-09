#include "include/main.h"


  int32_t tisp_s_Gamma(int32_t arg1)

{
    int32_t var_18_98 = 0x102;
    memcpy(0x97364, arg1, 0x102);
    tisp_gamma_param_array_set(0x3c, arg1, &var_18_99);
    memcpy(tparams_day + 0x2844, arg1, var_18_100);
    memcpy(tparams_night + 0x2844, arg1, var_18_101);
    return 0;
}

