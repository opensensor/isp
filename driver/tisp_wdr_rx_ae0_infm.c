#include "include/main.h"


  int32_t tisp_wdr_rx_ae0_infm(int32_t arg1, int32_t arg2)

{
    memcpy(&wdr_hist_Y0, arg1, 4);
    memcpy(&wdr_block_mean0, arg2, 4);
    return 0;
}

