#include "include/main.h"


  int32_t tx_isp_vin_reset(void* arg1)

{
    char* $a0 = *((char*)arg1 + 0xe4); // Fixed void pointer arithmetic
    
    if (!$a0)
    {
        isp_printf(); // Fixed: macro call, removed arguments;
        return 0xffffffff;
    }
    
    char* $v0_1 = (char*)(**($a0 + 0xc4)); // Fixed void pointer assignment
    
    if (!$v0_1)
        return 0;
    
    int32_t $v0_2 = *($v0_1 + 8);
    
    if (!$v0_2)
        return 0;
    
    int32_t result = $v0_2();
    
    if ((uintptr_t)result != 0xfffffdfd)
        return result;
    
    return 0;
}

