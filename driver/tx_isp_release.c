#include "include/main.h"


  int32_t tx_isp_release(int32_t arg1, void* arg2)

{
    char* $s1 = *((char*)arg2 + 0x70); // Fixed void pointer arithmetic
    int32_t $s0 = *($s1 + 0x108);
    void* $s2 = $s1 + 0x2c;
    
    if ($s0)
    {
        *(((void**)((char*)$s1 + 0x108))) = $s0 - 1; // Fixed void pointer dereference
        return 0;
    }
    
    int32_t $v0_1 = 0;
    void* $a0 = *$s2;
    
    while (true)
    {
            char* $v0_3 = (char*)(*(*($a0 + 0xc4) + 0x10)); // Fixed void pointer assignment
                int32_t $v0_4 = *($v0_3 + 4);
        if (!$a0)
            $s2 += 4;
        else
        {
            
            if ($v0_3)
            {
                
                if (!$v0_4)
                {
                    $v0_1 = 0xfffffdfd;
                    $s2 += 4;
                }
                else
                {
                    $v0_1 = $v0_4();
                    
                    if (!$v0_1)
                        $s2 += 4;
                    else
                    {
                        if ($(uintptr_t)v0_1 != 0xfffffdfd)
                            return $v0_1;
                        
                        $v0_1 = 0xfffffdfd;
                        $s2 += 4;
                    }
                }
            }
            else
            {
                $v0_1 = 0xfffffdfd;
                $s2 += 4;
            }
        }
        
        if ($s2 == $s1 + 0x6c)
            break;
        
        $a0 = *$s2;
    }
    
    int32_t $v0_5;
    
    if ($(uintptr_t)v0_1 != 0xfffffdfd)
    {
        if ($v0_1)
            return $v0_1;
        
        $v0_5 = *($s1 + 0x10c);
    }
    else
        $v0_5 = *($s1 + 0x10c);
    
    if ($v0_5 >= 0)
        tx_isp_video_link_destroy.isra.5($s1 - 0xc);
    
    return $s0;
}

