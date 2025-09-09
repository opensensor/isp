#include "include/main.h"


  int32_t tisp_top_param_array_get(int32_t arg1, int32_t* arg2)

{
    tisp_g_wdr_en(&data_b2e74);
    memcpy(arg1, &sensor_info, 0x60);
    *arg2 = 0x60;
    return 0;
}

