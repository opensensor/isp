#include "include/main.h"


  int32_t tisp_s_ae_hist()

{
    int32_t i = 0;
    int32_t $a0;
    int32_t arg_0 = $a0;
    int32_t $a1;
    int32_t arg_4 = $a1;
    int32_t $a2;
    int32_t arg_8 = $a2;
    int32_t $a3;
    int32_t arg_c = $a3;
        char var_428[0x424];
        void arg_10;
    
    for (; (uintptr_t)i < 0x41c; i += 1)
    {
        var_428[i] = *(&arg_10 + i);
    }
    
    tisp_ae_set_hist_custome();
    return 0;
}

