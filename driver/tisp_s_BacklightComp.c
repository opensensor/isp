#include "include/main.h"


  int32_t tisp_s_BacklightComp(int32_t arg1)

{
    int32_t var_14_29 = 0x2c;
    int32_t var_40_57;
    memcpy(&var_40_58, 0x94d8c, 0x2c);
    int32_t $a2 = var_14_30;
    int32_t var_2c_20 = arg1 + 1;
    var_40_59 = 1;
    int32_t var_28_38 = 1;
    memcpy(0x94d8c, &var_40_60, $a2);
    tisp_ae_param_array_set(0xc, &var_40_61, &var_14_31);
    tisp_ae_trig();
    return 0;
}

