#include "include/main.h"


  int32_t subdev_sensor_ops_enum_input(void* arg1, int32_t* arg2)

{
    int32_t $v0 = 0;
    char* $s0_1 = (char*)(*(arg1 + 0xdc) - 0xe4); // Fixed void pointer assignment
        int32_t $a0_1 = $v0;
            void* $v1_3 = $s0_1 + 0xec;
            int32_t i = 0x20;
            void* $v0_2 = &arg2[1];
                uint32_t $at_1 = *$v1_3;
    if (!arg1 || !arg2)
        return 0xffffffea;
    
    private_mutex_lock(arg1 + 0xe8);
    
    while (true)
    {
        
        if ($s0_1 + 0xe4 == arg1 + 0xdc)
            break;
        
        *(((void**)((char*)$s0_1 + 0xdc))) = $v0; // Fixed void pointer dereference
        $v0 += 1;
        
        if ($a0_1 == *arg2)
        {
            arg2[9] = *($s0_1 + 0xe0);
            
            do
            {
                i -= 1;
                *$v0_2 = $at_1;
                $v0_2 += 1;
                
                if (!$at_1)
                    break;
                
                $v1_3 += 1;
            } while (i);
            
            break;
        }
        
        $s0_1 = *($s0_1 + 0xe4) - 0xe4;
    }
    
    private_mutex_unlock(arg1 + 0xe8);
    
    if (*($s0_1 + 0xdc) == *arg2)
        return 0;
    
    return 0xffffffea;
}

