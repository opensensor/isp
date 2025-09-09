#include "include/main.h"


  int32_t tisp_wdr_rx_ae1_infm(int32_t arg1, int32_t arg2)

{
    memcpy(&wdr_hist_Y1, arg1, 0x400);
    memcpy(&wdr_block_mean1, arg2, 0x384);
    return 0;
}

