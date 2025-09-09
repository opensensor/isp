#include "include/main.h"


  int32_t tisp_awb_get_ct_trend(void* arg1)

{
    for (int32_t i = 0; i != 0x18; )
    {
        int32_t $a3_1 = *(i + &data_a9e14_2);
        int32_t* $a2_2 = arg1 + i;
        i += 4;
        *$a2_2 = $a3_1;
    }
    
    return 0;
}

