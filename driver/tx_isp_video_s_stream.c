#include "include/main.h"


  int32_t tx_isp_video_s_stream(void* arg1, int32_t arg2)

{
    int32_t* $s4 = arg1 + 0x38;
    
    for (int32_t i = 0; i != 0x10; )
    {
        void* $a0 = *$s4;
        
        if ($a0)
        {
            int32_t* $v0_3 = *(*($a0 + 0xc4) + 4);
            
            if (!$v0_3)
                i += 1;
            else
            {
                int32_t $v0_4 = *$v0_3;
                
                if (!$v0_4)
                    i += 1;
                else
                {
                    int32_t result = $v0_4($a0, arg2);
                    
                    if (!result)
                        i += 1;
                    else
                    {
                        if (result != 0xfffffdfd)
                        {
                            void* $s0_1 = arg1 + (i << 2);
                            
                            while (arg1 != $s0_1)
                            {
                                void* $a0_1 = *($s0_1 + 0x38);
                                
                                if (!$a0_1)
                                    $s0_1 -= 4;
                                else
                                {
                                    int32_t* $v0_6 = *(*($a0_1 + 0xc4) + 4);
                                    
                                    if (!$v0_6)
                                        $s0_1 -= 4;
                                    else
                                    {
                                        int32_t $v0_7 = *$v0_6;
                                        
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

