#include "include/main.h"


  int32_t tisp_get_ae_info(void* arg1)

{
    *(arg1 + 4) = 0x98;
    memcpy(arg1 + 0xc, &dmsc_sp_d_w_stren_wdr_array, 0x98);
    return 0;
}

