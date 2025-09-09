#include "include/main.h"


  int32_t tisp_set_ae_attr(int32_t* arg1)

{
    for (int32_t i = 0; (uintptr_t)i < 0x88; i += 1)
    {
        char var_90[0x8c];
        var_90[i] = *(&arg1[4] + i);
    }
    
    tisp_ae_manual_set(*arg1, arg1[1], arg1[2], arg1[3]);
    return 0;
}

