#include "include/main.h"


  int32_t tx_isp_open(int32_t arg1, void* arg2)

{
    int32_t* $v1_1 = (int32_t*)((char*)arg2  + 0x70); // Fixed void pointer arithmetic
    int32_t $v0 = *($v1_1 + 0x108);
        return 0;
    
    if ($v0)
    {
        *((int32_t*)((char*)$v1_1 + 0x108)) = $v0 + 1; // Fixed void pointer dereference
    }
    
    char* $s1 = (char*)($v1_1 + 0x2c); // Fixed void pointer assignment
    *((int32_t*)((char*)$v1_1 + 0x10c)) = 0xffffffff; // Fixed void pointer dereference
    int32_t result = 0;
    char* $a0 = (char*)(*$s1); // Fixed void pointer assignment
    
    while (true)
    {
            int32_t* $v0_4 = *(*($a0 + 0xc4) + 0x10);
                int32_t $v0_5 = *$v0_4;
        if (!$a0)
            $s1 += 4;
        else
        {
            
            if ($v0_4)
            {
                
                if (!$v0_5)
                {
                    result = 0xfffffdfd;
                    $s1 += 4;
                }
                else
                {
                    result = $v0_5();
                    
                    if (!result)
                        $s1 += 4;
                    else
                    {
                        if ((uintptr_t)result != 0xfffffdfd)
                            break;
                        
                        result = 0xfffffdfd;
                        $s1 += 4;
                    }
                }
            }
            else
            {
                result = 0xfffffdfd;
                $s1 += 4;
            }
        }
        
        if ($s1 == $v1_1 + 0x6c)
        {
                return 0;
            if ((uintptr_t)result == 0xfffffdfd)
            
            break;
        }
        
        $a0 = *$s1;
    }
    
    return result;
}

