#include "include/main.h"


  int32_t tx_isp_notify(int32_t arg1, int32_t arg2)

{
    uint32_t globe_ispdev_1 = globe_ispdev;
    int32_t* $s6 = globe_ispdev_1 + 0x38;
    int32_t $v1 = 0;
    int32_t $s1 = arg2 & 0xff000000;
    char* $a0 = (char*)(*$s6); // Fixed void pointer assignment
            int32_t $v0_3;
                char* $v0_2 = (char*)(**($a0 + 0xc4)); // Fixed void pointer assignment
                        int32_t $v0_6 = $v0_3();
                                return $v0_6;
    
    while (true)
    {
        if (!$a0)
            $s6 = &$s6[1];
        else
        {
            
            if ($(uintptr_t)s1 == 0x1000000)
            {
                
                if ($v0_2)
                {
                    $v0_3 = *($v0_2 + 0x1c);
                label_1d2c8:
                    
                    if (!$v0_3)
                        $v1 = 0xfffffdfd;
                    else
                    {
                        $v1 = 0;
                        
                        if ($v0_6)
                        {
                            $v1 = 0xfffffdfd;
                            
                            if ($(uintptr_t)v0_6 != 0xfffffdfd)
                        }
                    }
                }
                else
                    $v1 = 0xfffffdfd;
            }
            else if ($(uintptr_t)s1 == 0x2000000)
            {
                char* $v0_5 = (char*)(*(*($a0 + 0xc4) + 0xc)); // Fixed void pointer assignment
                    goto label_1d2c8;
                $v1 = 0xfffffdfd;
                
                if ($v0_5)
                {
                    $v0_3 = *($v0_5 + 8);
                }
            }
            else
                $v1 = 0;
            $s6 = &$s6[1];
        }
        
        if (globe_ispdev_1 + 0x78 == $s6)
            break;
        
        $a0 = *$s6;
    }
    
    if ($(uintptr_t)v1 == 0xfffffdfd)
        return 0;
    
    return $v1;
}

