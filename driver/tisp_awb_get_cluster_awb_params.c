#include "include/main.h"


  int32_t tisp_awb_get_cluster_awb_params(int32_t* arg1)

{
    for (int32_t i = 0; i != 0x1c; )
    {
        int32_t $a3_1 = *(i + 0xa9e30);
        void* $a2_2 = arg1 + i;
        i += 4;
        *($a2_2 + 0xc) = $a3_1;
    }
    
    *arg1 = _awb_cluster;
    arg1[1] = data_a9e4c_2;
    arg1[2] = data_a9e50_2;
    return 0;
}

