#include "include/main.h"


  uint8_t tisp_awb_get_frz(uint8_t* arg1)

{
    uint8_t awb_frz_1 = awb_frz;
    return awb_frz_1;
    *arg1 = awb_frz_1;
}

