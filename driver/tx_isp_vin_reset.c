#include "include/main.h"


  int32_t tx_isp_vin_reset(void* arg1)

{
    void* $a0 = *(arg1 + 0xe4);
    
    if (!$a0)
    {
        isp_printf(1, &$LC0, 0x16c);
        return 0xffffffff;
    }
    
    void* $v0_1 = **($a0 + 0xc4);
    
    if (!$v0_1)
        return 0;
    
    int32_t $v0_2 = *($v0_1 + 8);
    
    if (!$v0_2)
        return 0;
    
    int32_t result = $v0_2();
    
    if (result != 0xfffffdfd)
        return result;
    
    return 0;
}

