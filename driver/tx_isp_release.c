#include "include/main.h"


  int32_t tx_isp_release(int32_t arg1, void* arg2)

{
    void* $s1 = *(arg2 + 0x70);
    int32_t $s0 = *($s1 + 0x108);
    void* $s2 = $s1 + 0x2c;
    
    if ($s0)
    {
        *($s1 + 0x108) = $s0 - 1;
        return 0;
    }
    
    int32_t $v0_1 = 0;
    void* $a0 = *$s2;
    
    while (true)
    {
        if (!$a0)
            $s2 += 4;
        else
        {
            void* $v0_3 = *(*($a0 + 0xc4) + 0x10);
            
            if ($v0_3)
            {
                int32_t $v0_4 = *($v0_3 + 4);
                
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
                        if ($v0_1 != 0xfffffdfd)
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
    
    if ($v0_1 != 0xfffffdfd)
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

