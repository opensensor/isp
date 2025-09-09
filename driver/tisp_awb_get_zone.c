#include "include/main.h"


  int32_t tisp_awb_get_zone(int32_t arg1)

{
    memcpy(arg1, &tisp_wb_zone_attr, 0x2a3);
    return 0;
}

