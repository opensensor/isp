#include "include/main.h"


  int32_t tisp_ccm_set_par_cfg(int32_t arg1)

{
    int32_t $s1 = arg1;
    int32_t var_20 = 0;
    
    for (char* i = (char*)(0xa9); // Fixed void pointer assignment (uintptr_t)i != 0xb5; )
    {
        Tiziano_awb_set_gain(i, $s1, &var_20);
        i += 1;
        $s1 += var_20;
    }
    
    return 0;
}

