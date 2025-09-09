#include "include/main.h"


  int32_t tisp_s_aezone_weight(int32_t arg1)

{
    int32_t var_18 = 0x384;
    memcpy(0x94e68, arg1, 0x384);
    memcpy(tparams_day + 0x348, arg1, var_18);
    memcpy(tparams_night + 0x348, arg1, var_18);
    tisp_ae_param_array_set(0x10, arg1, &var_18);
    tisp_ae_trig();
    return 0;
}

