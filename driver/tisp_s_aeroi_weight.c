#include "include/main.h"


  int32_t tisp_s_aeroi_weight(int32_t* arg1)

{
    void var_3a0;
    void* $v1 = &var_3a0_1;
    int32_t var_1c_15 = 0x384;
    int32_t* i = arg1;
    
    do
    {
        int32_t $a0 = *i;
        i = &i[1];
        *$v1 = 8 - $a0;
        $v1 += 4;
    } while (i != &arg1[0xe1]);
    
    memcpy(0x95570, arg1, 0x384);
    memcpy(0x951ec, &var_3a0_2, var_1c_16);
    memcpy(tparams_day + 0xa50, arg1, var_1c_17);
    memcpy(tparams_day + 0x6cc, &var_3a0_3, var_1c_18);
    memcpy(tparams_night + 0xa50, arg1, var_1c_19);
    memcpy(tparams_night + 0x6cc, &var_3a0_4, var_1c_20);
    tisp_ae_param_array_set(0x12, arg1, &var_1c_21);
    tisp_ae_param_array_set(0x11, &var_3a0_5, &var_1c_22);
    tisp_ae_trig();
    return 0;
}

