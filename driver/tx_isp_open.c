#include "include/main.h"


  int32_t tx_isp_open(int32_t arg1, void* arg2)

{
    char* $v1_1 = *((char*)arg2 + 0x70); // Fixed void pointer arithmetic
    int32_t $v0 = *($v1_1 + 0x108);
    
    if ($v0)
    {
        *(((void**)((char*)$v1_1 + 0x108))) = $v0 + 1; // Fixed void pointer dereference
        return 0;
    }
    
    void* $s1 = $v1_1 + 0x2c;
    *(((void**)((char*)$v1_1 + 0x10c))) = 0xffffffff; // Fixed void pointer dereference
    int32_t result = 0;
    void* $a0 = *$s1;
    
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
            if ((uintptr_t)result == 0xfffffdfd)
                return 0;
            
            break;
        }
        
        $a0 = *$s1;
    }
    
    return result;
}

