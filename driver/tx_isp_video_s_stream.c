#include "include/main.h"


  int32_t tx_isp_video_s_stream(void* arg1, int32_t arg2)

{
    int32_t* $s4 = arg1 + 0x38;
        char* $a0 = (char*)(*$s4); // Fixed void pointer assignment
            int32_t* $v0_3 = *(*($a0 + 0xc4) + 4);
                int32_t $v0_4 = *$v0_3;
                    int32_t result = $v0_4($a0, arg2);
                            char* $s0_1 = (char*)(arg1 + (i << 2)); // Fixed void pointer assignment
                                int32_t* $a0_1 = (int32_t*)((char*)$s0_1  + 0x38); // Fixed void pointer arithmetic
                                    int32_t* $v0_6 = *(*($a0_1 + 0xc4) + 4);
                                        int32_t $v0_7 = *$v0_6;
    
    for (int32_t i = 0; (uintptr_t)i != 0x10; )
    {
        
        if ($a0)
        {
            
            if (!$v0_3)
                i += 1;
            else
            {
                
                if (!$v0_4)
                    i += 1;
                else
                {
                    
                    if (!result)
                        i += 1;
                    else
                    {
                        if ((uintptr_t)result != 0xfffffdfd)
                        {
                            
                            while (arg1 != $s0_1)
                            {
                                
                                if (!$a0_1)
                                    $s0_1 -= 4;
                                else
                                {
                                    
                                    if (!$v0_6)
                                        $s0_1 -= 4;
                                    else
                                    {
                                        
                                        if (!$v0_7)
                                            $s0_1 -= 4;
                                        else
                                        {
                                            $v0_7($a0_1, arg2 < 1 ? 1 : 0);
                                            $s0_1 -= 4;
                                        }
                                    }
                                }
                            }
                            
                            return result;
                        }
                        
                        i += 1;
                    }
                }
            }
        }
        else
            i += 1;
        
        $s4 = &$s4[1];
    }
    
    return 0;
}

