#include "include/main.h"


  int32_t tisp_s_awb_cluster(int32_t arg1, int32_t arg2, int32_t arg3)

{
    int32_t i = 0;
    int32_t $a3;
    int32_t arg_c = $a3;
    
    for (; i < 0x18; i += 1)
    {
        char var_20_206[0x1c];
        void arg_10;
        var_20_207[i] = *(&arg_10 + i);
    }
    
    tisp_awb_set_cluster_awb_params(arg1, arg2, arg3);
    return 0;
}

