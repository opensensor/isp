#include "include/main.h"


  uint32_t tisp_cust_mode_g_ctrl()

{
    if (!tparams_cust)
        return 0xffffffff;
    
    return cust_mode;
}

