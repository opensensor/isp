#include "include/main.h"


  int32_t tisp_s_af_weight(int32_t arg1)

{
    int32_t var_18_106 = 0x384;
    memcpy(0xa69bc, arg1, 0x384);
    memcpy(tparams_day + 0x11e9c, arg1, var_18_107);
    memcpy(tparams_night + 0x11e9c, arg1, var_18_108);
    tisp_af_param_array_set(0x3bf, arg1, &var_18_109);
    return 0;
}

