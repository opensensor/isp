#include "include/main.h"


  int32_t tx_isp_vin_activate_subdev(void* arg1)

{
    private_mutex_lock(arg1 + 0xe8);
    
    if (*(arg1 + 0xf4) == 1)
        *(((int32_t*)((char*)arg1 + 0xf4))) = 2; // Fixed void pointer dereference
    
    private_mutex_unlock(arg1 + 0xe8);
    *(arg1 + 0xf8) += 1;
    return 0;
}

