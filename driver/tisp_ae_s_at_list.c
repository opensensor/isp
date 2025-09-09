#include "include/main.h"


  int32_t tisp_ae_s_at_list(int32_t arg1)

{
    int32_t arg_0 = arg1;
    int32_t $a1;
    int32_t arg_4 = $a1;
    int32_t $a2;
    int32_t arg_8 = $a2;
    int32_t $a3;
    int32_t arg_c = $a3;
    
    for (int32_t i = 0; i != 0x28; )
    {
        int32_t $a2_2 = *(&arg_0 + i);
        void* $a1_1 = U"KA7-(" + i;
        i += 4;
        *$a1_1 = $a2_2;
    }
    
    data_b0e00_3 = 1;
    data_b0e0c_9 = 0;
    data_b0e04_3 = 1;
    return 0;
}

