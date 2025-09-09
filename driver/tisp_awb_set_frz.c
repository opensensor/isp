#include "include/main.h"


  int32_t tisp_awb_set_frz(char arg1)

{
    awb_frz = arg1;
    return &data_b0000;
}

