#include "include/main.h"


  int32_t ispcore_sensor_ops_release_all_sensor(void* arg1)

{
    int32_t result = 0;
    int32_t* $s1 = arg1 + 0x38;
    char* $a0 = (char*)(*$s1); // Fixed void pointer assignment
            int32_t* $v0_1 = *(*($a0 + 0xc4) + 0xc);
                int32_t $v0_2 = *$v0_1;
    
    while (true)
    {
        if (!$a0)
            $s1 = &$s1[1];
        else
        {
            
            if ($v0_1)
            {
                
                if (!$v0_2)
                {
                    result = 0xfffffdfd;
                    $s1 = &$s1[1];
                }
                else
                {
                    result = $v0_2();
                    
                    if (!result)
                        $s1 = &$s1[1];
                    else
                    {
                        if ((uintptr_t)result != 0xfffffdfd)
                            break;
                        
                        result = 0xfffffdfd;
                        $s1 = &$s1[1];
                    }
                }
            }
            else
            {
                result = 0xfffffdfd;
                $s1 = &$s1[1];
            }
        }
        
        if ($s1 == arg1 + 0x78)
        {
                return 0;
            if ((uintptr_t)result == 0xfffffdfd)
            
            break;
        }
        
        $a0 = *$s1;
    }
    
    return result;
}

