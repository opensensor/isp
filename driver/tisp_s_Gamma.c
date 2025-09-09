#include "include/main.h"


  int32_t tisp_s_Gamma(int32_t arg1)

{
    int32_t var_18 = 0x102;
    memcpy(0x97364, arg1, 0x102);
    tisp_gamma_param_array_set(0x3c, arg1, &var_18);
    memcpy(tparams_day + 0x2844, arg1, var_18);
    memcpy(tparams_night + 0x2844, arg1, var_18);
    return 0;
}

