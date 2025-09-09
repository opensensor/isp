#include "include/main.h"


  int32_t tisp_ae_s_at_list(int32_t arg1)

{
    int32_t arg_0 = arg1;
    int32_t arg_4 = $a1;
    int32_t arg_8 = $a2;
    int32_t arg_c = $a3;
        int32_t $a2_2 = *(&arg_0 + i);
        void* $a1_1 = U"KA7-(" + i;
    int32_t $a1;
    int32_t $a2;
    int32_t $a3;
    
    for (int32_t i = 0; (uintptr_t)i != 0x28; )
    {
        i += 4;
        *$a1_1 = $a2_2;
    }
    
    data_b0e00_1 = 1;
    data_b0e0c_5 = 0;
    data_b0e04_2 = 1;
    return 0;
}

