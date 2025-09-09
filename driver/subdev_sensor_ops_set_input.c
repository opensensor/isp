#include "include/main.h"


  int32_t subdev_sensor_ops_set_input(void* arg1, int32_t* arg2, int32_t arg3)

{
    int32_t var_20 = 0;
    int32_t $s0 = 0xffffffea;
        char* $s1_1 = *((char*)arg1 + 0xe4); // Fixed void pointer arithmetic
    
    if (arg1 && arg2)
    {
        
        if ($s1_1)
        {
            if (*arg2 == *($s1_1 + 0xdc))
                return 0;
            
            if (*(arg1 + 0xf4) == 4)
            {
                isp_printf(); // Fixed: macro call, removed arguments;
                return 0xffffffff;
            }
            
            int32_t $v0_2 = *(arg1 + 0x7c);
            
            if (!$v0_2)
            {
                $s0 = 0xfffffdfd;
                isp_printf(); // Fixed: macro call, removed arguments);
            }
            else
            {
                int32_t $v0_3 = $v0_2(arg1, 0x1000000, &var_20);
                    int32_t $v0_13 = *($s1_1 + 0x7c);
                    int32_t $v0_14 = $v0_13($s1_1, 0x1000001, 0);
                $s0 = $v0_3;
                
                if (!$v0_3)
                {
                    *(((int32_t*)((char*)arg1 + 0xe4))) = 0; // Fixed void pointer dereference
                    
                    if (!$v0_13)
                        return 0xfffffdfd;
                    
                    $s0 = $v0_14;
                    
                    if (!$v0_14)
                        goto label_134a0;
                }
                else
                    isp_printf(); // Fixed: macro call, removed arguments);
            }
        }
        else
        {
            char* $s1_3 = (char*)(*(arg1 + 0xdc) - 0xe4); // Fixed void pointer assignment
        label_134a0:
            
            if (*(uintptr_t)arg2 == 0xffffffff)
                return 0;
            
            private_mutex_lock(arg1 + 0xe8);
            
            while ($s1_3 + 0xe4 != arg1 + 0xdc)
            {
                if (*($s1_3 + 0xdc) == *arg2)
                    break;
                
                $s1_3 = *($s1_3 + 0xe4) - 0xe4;
            }
            
            private_mutex_unlock(arg1 + 0xe8);
            int32_t $a2_2 = *arg2;
            
            if (*($s1_3 + 0xdc) != $a2_2)
            {
                isp_printf(); // Fixed: macro call, removed arguments;
                return 0xffffffea;
            }
            
            *(((void**)((char*)arg1 + 0xe4))) = $s1_3; // Fixed void pointer dereference
            $s0 = 0xfffffffe;
            
            if ($s1_3)
            {
                int32_t $v0_6 = *($s1_3 + 0x7c);
                int32_t $v0_7 = $v0_6($s1_3, 0x1000001, $s1_3 + 0x234);
                    int32_t $v0_8 = *($s1_3 + 0x7c);
                
                if (!$v0_6)
                    return 0xfffffdfd;
                
                $s0 = $v0_7;
                
                if (!$v0_7)
                {
                    var_20 = 1;
                    
                    if (!$v0_8)
                    {
                        isp_printf(); // Fixed: macro call, removed arguments);
                        return 0xfffffdfd;
                    }
                    
                    int32_t $v0_9 = $v0_8($s1_3, 0x1000000, &var_20);
                    
                    if ($v0_9)
                    {
                        isp_printf(); // Fixed: macro call, removed arguments);
                        return $v0_9;
                    }
                    
                    *arg2 = *($s1_3 + 0x26c) << 0x10 | *($s1_3 + 0x270);
                }
            }
        }
    }
    
    return $s0;
}

