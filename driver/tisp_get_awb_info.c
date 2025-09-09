#include "include/main.h"


  int32_t tisp_get_awb_info(void* arg1)

{
    void var_28_6;
    tisp_g_wb_attr(&var_28_7);
    *(arg1 + 4) = 0x1c;
    memcpy(arg1 + 0xc, &var_28_8, 0x1c);
    return 0;
}

