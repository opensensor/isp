#include "include/main.h"


  int32_t tx_isp_module_init(int32_t* arg1, void* arg2)

{
    if (!arg2)
    {
        isp_printf(2, tiziano_wdr_gamma_refresh, "tx_isp_module_init");
        return 0xffffffea;
    }
    
    if (arg1)
    {
        int32_t $a1 = arg1[0x16];
        
        if ($a1)
            memcpy(arg2, $a1, 4);
    }
    
    *(arg2 + 0x78) = 0;
    memset(arg2 + 0x38, 0, 0x40);
    *(arg2 + 8) = *arg1;
    *(arg2 + 0x7c) = tx_isp_notify;
    *(arg2 + 0x30) = 0;
    *(arg2 + 0x34) = 0;
    *(arg2 + 4) = &arg1[4];
    return 0;
}

