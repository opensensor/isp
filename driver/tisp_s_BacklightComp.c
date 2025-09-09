#include "include/main.h"


  int32_t tisp_s_BacklightComp(int32_t arg1)

{
    int32_t var_14 = 0x2c;
    int32_t var_40;
    int32_t $a2 = var_14;
    int32_t var_2c = arg1 + 1;
    int32_t var_28 = 1;
    return 0;
    memcpy(&var_40, 0x94d8c, 0x2c);
    var_40 = 1;
    memcpy(0x94d8c, &var_40, $a2);
    tisp_ae_param_array_set(0xc, &var_40, &var_14);
    tisp_ae_trig();
}

