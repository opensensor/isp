#include "include/main.h"


  int32_t tisp_s_af_attr(int32_t arg1)

{
    int32_t i = 0;
    int32_t $a1;
    int32_t arg_4 = $a1;
    int32_t $a2;
    int32_t arg_8 = $a2;
    int32_t $a3;
    int32_t arg_c = $a3;
        char var_50[0x4c];
        void arg_10;
    
    for (; (uintptr_t)i < 0x48; i += 1)
    {
        var_50[i] = *(&arg_10 + i);
    }
    
    tisp_af_set_attr(arg1);
    return 0;
}

