#include "include/main.h"


  int32_t tisp_s_Hilightdepress(int32_t arg1)

{
    int32_t var_14_26 = 0x2c;
    int32_t var_40_52;
    memcpy(&var_40_53, 0x94d8c, 0x2c);
    int32_t $a2 = var_14_27;
    int32_t var_28_37 = arg1 + 1;
    var_40_54 = 1;
    int32_t var_2c_19 = 1;
    memcpy(0x94d8c, &var_40_55, $a2);
    tisp_ae_param_array_set(0xc, &var_40_56, &var_14_28);
    tisp_ae_trig();
    return 0;
}

