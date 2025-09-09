#include "include/main.h"


  int32_t vic_sensor_ops_sync_sensor_attr(void* arg1, int32_t arg2, int32_t arg3)

{
    if (arg1 && arg1 < 0xfffff001)
    {
        int32_t $a0 = *(arg1 + 0xd4);
        
        if ($a0 && $a0 < 0xfffff001)
        {
            void* const $v0_1;
            
            $v0_1 = !arg2 ? memset : memcpy;
            
            $v0_1();
            return 0;
        }
    }
    
    isp_printf(2, "The parameter is invalid!\\n", arg3);
    return 0xffffffea;
}

