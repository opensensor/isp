#include "include/main.h"


  int32_t fs_activate_module(void* arg1)

{
    int32_t result = 0xffffffea;
    
    if (arg1)
    {
        if (arg1 >= 0xfffff001)
            return 0xffffffea;
        
        result = 0;
        
        if (*(arg1 + 0xe4) == 1)
        {
            int32_t $a2_1 = 0;
            
            while (true)
            {
                if ($a2_1 >= *(arg1 + 0xe0))
                {
                    *(arg1 + 0xe4) = 2;
                    return 0;
                }
                
                void* $v0_3 = $a2_1 * 0x2ec + *(arg1 + 0xdc);
                
                if (*($v0_3 + 0x2d0) != 1)
                    break;
                
                *($v0_3 + 0x2d0) = 2;
                $a2_1 += 1;
            }
            
            isp_printf(2, &$LC0, $a2_1);
            return 0xffffffff;
        }
    }
    
    return result;
}

