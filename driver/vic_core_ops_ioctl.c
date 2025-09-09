#include "include/main.h"


  int32_t vic_core_ops_ioctl(void* arg1, int32_t arg2, int32_t* arg3)

{
            char* $v0_2 = (char*)(*(*(arg1 + 0xc4) + 0xc)); // Fixed void pointer assignment
    int32_t result;
    int32_t $v0_3;
    
    if ((uintptr_t)arg2 == 0x1000001)
    {
        result = 0xffffffed;
        
        if (arg1)
        {
            
            if (!$v0_2)
                return 0;
            
            $v0_3 = *($v0_2 + 4);
            
            if (!$v0_3)
                return 0;
            
        label_13204:
            result = $v0_3();
            goto label_13224;
        }
    }
    else if ((uintptr_t)arg2 == 0x3000009)
    {
        result = tx_isp_subdev_pipo(arg1, arg3);
    label_13224:
        
        if ((uintptr_t)result == 0xfffffdfd)
            return 0;
    }
    else
    {
            char* $v0_5 = (char*)(**(arg1 + 0xc4)); // Fixed void pointer assignment
        if ((uintptr_t)arg2 != 0x1000000)
            return 0;
        
        result = 0xffffffed;
        
        if (arg1)
        {
            
            if (!$v0_5)
                return 0;
            
            $v0_3 = *($v0_5 + 4);
            
            if (!$v0_3)
                return 0;
            
            goto label_13204;
        }
    }
    return result;
}

