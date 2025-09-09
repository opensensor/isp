#include "include/main.h"


  int32_t ispcore_sensor_ops_ioctl(void* arg1)

{
    int32_t result = 0;
    int32_t* $s1 = arg1 + 0x38;
    void* $a0 = *$s1;
    
    while (true)
    {
        if (!$a0)
            $s1 = &$s1[1];
        else
        {
            void* $v0_1 = *(*($a0 + 0xc4) + 0xc);
            int32_t $v0_2;
            
            if ($v0_1)
                $v0_2 = *($v0_1 + 8);
            
            if (!$v0_1 || !$v0_2)
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
                    if (result != 0xfffffdfd)
                        break;
                    
                    result = 0xfffffdfd;
                    $s1 = &$s1[1];
                }
            }
        }
        
        if ($s1 == arg1 + 0x78)
        {
            if (result == 0xfffffdfd)
                return 0;
            
            break;
        }
        
        $a0 = *$s1;
    }
    
    return result;
}

