#include "include/main.h"


  int32_t tisp_set_ae_info(void* arg1)

{
    *(((void**)((char*)arg1 + 4))) = 0x98; // Fixed void pointer dereference
    memcpy(&dmsc_sp_d_w_stren_wdr_array, arg1 + 0xc, 0x98);
    return 0;
}

