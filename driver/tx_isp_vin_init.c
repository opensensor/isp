#include "include/main.h"


  int32_t tx_isp_vin_init(void* arg1, int32_t arg2)

{
    void* $a0 = *(arg1 + 0xe4);
    int32_t result;
    
    if (!$a0)
    {
        isp_printf(1, &$LC0, 0x158);
        result = 0xffffffff;
    }
    else
    {
        void* $v0_1 = **($a0 + 0xc4);
        
        if (!$v0_1)
            result = 0;
        else
        {
            int32_t $v0_2 = *($v0_1 + 4);
            
            if (!$v0_2)
                result = 0;
            else
            {
                result = $v0_2();
                
                if (result == 0xfffffdfd)
                    result = 0;
            }
        }
    }
    
    int32_t $v1 = 3;
    
    if (!arg2)
        $v1 = 2;
    
    *(arg1 + 0xf4) = $v1;
    return result;
}

