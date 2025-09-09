#include "include/main.h"


  int32_t private_get_driver_interface(int32_t* arg1)

{
        return 0xffffffff;
    int32_t* $v0 = get_driver_common_interfaces();
    int32_t result = 0;
        int32_t $a2_1 = *$v0;
        int32_t $v1_1 = $v0[0x78];
            int32_t var_10_1 = $v1_1;
            return 0xffffffff;
    if (!(uintptr_t)arg1)
    
    *arg1 = $v0;
    
    if ($v0)
    {
        
        if ($a2_1 == printk)
            result = 0;
        
        if ($a2_1 != printk || $v1_1 != $a2_1)
        {

        }
    }
    
    return result;
}

