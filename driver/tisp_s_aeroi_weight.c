#include "include/main.h"


  int32_t tisp_s_aeroi_weight(int32_t* arg1)

{
    void var_3a0;
    char* $v1 = (char*)(&var_3a0); // Fixed void pointer assignment
    int32_t var_1c = 0x384;
    int32_t* i = arg1;
        int32_t $a0 = *i;
    return 0;
    
    do
    {
        i = &i[1];
        *$v1 = 8 - $a0;
        $v1 += 4;
    } while (i != &arg1[0xe1]);
    
    memcpy(0x95570, arg1, 0x384);
    memcpy(0x951ec, &var_3a0, var_1c);
    memcpy(tparams_day + 0xa50, arg1, var_1c);
    memcpy(tparams_day + 0x6cc, &var_3a0, var_1c);
    memcpy(tparams_night + 0xa50, arg1, var_1c);
    memcpy(tparams_night + 0x6cc, &var_3a0, var_1c);
    tisp_ae_param_array_set(0x12, arg1, &var_1c);
    tisp_ae_param_array_set(0x11, &var_3a0, &var_1c);
    tisp_ae_trig();
}

