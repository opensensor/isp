#include "include/main.h"


  int32_t tisp_awb_set_cluster_awb_params(int32_t arg1, int32_t arg2, int32_t arg3)

{
    int32_t arg_0 = arg1;
    int32_t* $a0 = &_awb_cluster;
    int32_t $a3;
    int32_t arg_c = $a3;
    int32_t* i = &arg_0;
    void arg_1c;
        int32_t $a3_1 = i[3];
    return 0;
    
    do
    {
        i = &i[1];
        $a0[1] = $a3_1;
        $a0 = &$a0[1];
    } while (i != &arg_1c);
    
    _awb_cluster = arg_0;
    data_a9e4c = arg2;
    data_a9e50 = arg3;
}

