#include "include/main.h"


  int32_t tisp_set_awb_info(void* arg1)

{
    *(arg1 + 4) = 0x1c;
    int32_t var_28_9;
    memcpy(&var_28_10, arg1 + 0xc, 0x1c);
    int32_t var_10_33;
    int32_t var_30_7 = var_10_34;
    int32_t var_24_4;
    int32_t var_20_70;
    int32_t var_1c_8;
    int32_t var_18_59;
    int32_t var_14_16;
    tisp_s_wb_attr(var_28_11, var_24_5, var_20_71, var_1c_9, var_18_60, var_14_17);
    return 0;
}

