#include "include/main.h"


  int32_t tisp_awb_algo_handle(void* arg1)

{
    if (*(arg1 + 8) != 1)
        return 1;
    
    int32_t var_10_40;
    int32_t var_30_1_7 = var_10_41;
    int32_t var_1c_10;
    int32_t var_18_84;
    int32_t var_14_22;
    return tisp_s_wb_attr(1, *(arg1 + 0xc), *(arg1 + 0x10), var_1c_11, var_18_85, var_14_23);
}

