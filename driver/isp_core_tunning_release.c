#include "include/main.h"


  int32_t isp_core_tunning_release(int32_t arg1, void* arg2)

{
    void* $s0 = *(*(*(arg2 + 0x70) + 0xc8) + 0x1bc);
    isp_printf(0, &$LC0, "isp_core_tunning_release");
    
    if (*($s0 + 0x40c4) != 2)
    {
        int32_t $a0 = *($s0 + 0x40ac);
        
        if (!$a0)
            *($s0 + 0x40c4) = 2;
        else
        {
            isp_free_buffer($a0);
            *($s0 + 0x40c4) = 2;
        }
    }
    
    return 0;
}

