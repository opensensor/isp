#include "include/main.h"


  int32_t tisp_awb_get_ct_trend(void* arg1)

{
        int32_t $a3_1 = *(i + &data_a9e14);
        int32_t* $a2_2 = arg1 + i;
    for (int32_t i = 0; (uintptr_t)i != 0x18; )
    {
        i += 4;
        *$a2_2 = $a3_1;
    }
    
    return 0;
}

