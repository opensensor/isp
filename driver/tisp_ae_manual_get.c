#include "include/main.h"


  int32_t tisp_ae_manual_get(int32_t arg1)

{
    memcpy(arg1, &dmsc_sp_d_w_stren_wdr_array, 0x98);
    return 0;
}

