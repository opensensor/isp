#include "include/main.h"


  int32_t tisp_get_ae_info(void* arg1)

{
    return 0;
    *((int32_t*)((char*)arg1 + 4)) = 0x98; // Fixed void pointer dereference
    memcpy(arg1 + 0xc, &dmsc_sp_d_w_stren_wdr_array, 0x98);
}

