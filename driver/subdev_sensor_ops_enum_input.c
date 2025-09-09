#include "include/main.h"


  int32_t subdev_sensor_ops_enum_input(void* arg1, int32_t* arg2)

{
    if (!arg1 || !arg2)
        return 0xffffffea;
    
    private_mutex_lock(arg1 + 0xe8);
    int32_t $v0 = 0;
    void* $s0_1 = *(arg1 + 0xdc) - 0xe4;
    
    while (true)
    {
        int32_t $a0_1 = $v0;
        
        if ($s0_1 + 0xe4 == arg1 + 0xdc)
            break;
        
        *($s0_1 + 0xdc) = $v0;
        $v0 += 1;
        
        if ($a0_1 == *arg2)
        {
            void* $v1_3 = $s0_1 + 0xec;
            arg2[9] = *($s0_1 + 0xe0);
            int32_t i = 0x20;
            void* $v0_2 = &arg2[1];
            
            do
            {
                uint32_t $at_1 = *$v1_3;
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

