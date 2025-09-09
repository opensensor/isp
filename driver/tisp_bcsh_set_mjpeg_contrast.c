#include "include/main.h"


  int32_t tisp_bcsh_set_mjpeg_contrast(uint8_t arg1, uint8_t arg2, char arg3)

{
    s_bcsh_mjpeg_mode = arg1;
    s_bcsh_mjpeg_y_range_low = arg2;
    data_9a91c_2 = arg3;
    /* tailcall */
    return tiziano_bcsh_update();
}

