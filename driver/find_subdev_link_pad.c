#include "include/main.h"


  int32_t find_subdev_link_pad(void* arg1, int32_t* arg2)

{
    int32_t* $a3 = arg1 + 0x38;
    void* $v0 = *$a3;
            char* $v1_1 = *($v0 + 8);
            char* $a2_1 = *arg2;
            uint32_t $t0_1 = *$v1_1;
                uint32_t temp1_1 = $t0_1;
    
    while (true)
    {
        if (!$v0)
            $a3 = &$a3[1];
        else
        {
            uint32_t $at_1;
            
            while (true)
            {
                $at_1 = *$a2_1;
                $v1_1 = &$v1_1[1];
                $a2_1 = &$a2_1[1];
                
                if ($at_1 != $t0_1)
                    break;
                
                $t0_1 = *$v1_1;
                
                if (!temp1_1)
                {
                    $t0_1 = $at_1;
                    break;
                }
            }
            
            $a3 = &$a3[1];
            
            if (!($t0_1 - $at_1))
            {
                uint32_t $v1_2 = arg2[1];
                uint32_t $v1_3;
                
                if ($v1_2 == 1)
                {
                    $v1_3 = *(arg2 + 5);
                    
                    if ($v1_3 < *($v0 + 0xca))
                        return $v1_3 * 0x24 + *($v0 + 0xd0);
                }
                else if ($v1_2 == 2)
                {
                    $v1_3 = *(arg2 + 5);
                    
                    if ($v1_3 < *($v0 + 0xc8))
                        return $v1_3 * 0x24 + *($v0 + 0xcc);
                }
                isp_printf(); // Fixed: macro call, removed arguments;
                return 0;
            }
        }
        
        if ($a3 == arg1 + 0x78)
            break;
        
        $v0 = *$a3;
    }
    
    return 0;
}

