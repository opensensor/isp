#include "include/main.h"


  int32_t csi_core_ops_init(void* arg1, int32_t arg2, int32_t arg3)

{
    int32_t result = 0xffffffea;
    
    if (arg1)
    {
        if (arg1 >= 0xfffff001)
            return 0xffffffea;
        
        void* $s0_1 = *(arg1 + 0xd4);
        result = 0xffffffea;
        
        if ($s0_1 && $s0_1 < 0xfffff001)
        {
            result = 0;
            
            if (*($s0_1 + 0x128) >= 2)
            {
                int32_t $v0_17;
                
                if (!arg2)
                {
                    isp_printf(0, "%s[%d] VIC do not support this format %d\\n", arg3);
                    void* $a0_21 = *($s0_1 + 0xb8);
                    *($a0_21 + 8) &= 0xfffffffe;
                    void* $a0_22 = *($s0_1 + 0xb8);
                    *($a0_22 + 0xc) &= 0xfffffffe;
                    void* $a0_23 = *($s0_1 + 0xb8);
                    *($a0_23 + 0x10) &= 0xfffffffe;
                    $v0_17 = 2;
                }
                else
                {
                    void* $v1_5 = *($s0_1 + 0x110);
                    int32_t $s2_1 = *($v1_5 + 0x14);
                    
                    if ($s2_1 == 1)
                    {
                        *(*($s0_1 + 0xb8) + 4) = *($v1_5 + 0x24) - 1;
                        void* $v0_2 = *($s0_1 + 0xb8);
                        *($v0_2 + 8) &= 0xfffffffe;
                        *(*($s0_1 + 0xb8) + 0xc) = 0;
                        private_msleep(1);
                        void* $v1_9 = *($s0_1 + 0xb8);
                        *($v1_9 + 0x10) &= 0xfffffffe;
                        private_msleep(1);
                        *(*($s0_1 + 0xb8) + 0xc) = $s2_1;
                        private_msleep(1);
                        void* $v0_7 = *($s0_1 + 0x110);
                        int32_t $v1_10 = *($v0_7 + 0x3c);
                        int32_t* $v0_8;
                        
                        if ($v1_10)
                            $v0_8 = *($s0_1 + 0x13c);
                        else
                        {
                            int32_t $v0_9 = *($v0_7 + 0x1c);
                            void* $a0_2;
                            
                            if ($v0_9 - 0x50 < 0x1e)
                                $a0_2 = *($s0_1 + 0x13c);
                            else
                            {
                                $v1_10 = 1;
                                
                                if ($v0_9 - 0x6e >= 0x28)
                                {
                                    $v1_10 = 2;
                                    
                                    if ($v0_9 - 0x96 >= 0x32)
                                    {
                                        $v1_10 = 3;
                                        
                                        if ($v0_9 - 0xc8 >= 0x32)
                                        {
                                            $v1_10 = 4;
                                            
                                            if ($v0_9 - 0xfa >= 0x32)
                                            {
                                                $v1_10 = 5;
                                                
                                                if ($v0_9 - 0x12c >= 0x64)
                                                {
                                                    $v1_10 = 6;
                                                    
                                                    if ($v0_9 - 0x190 >= 0x64)
                                                    {
                                                        $v1_10 = 7;
                                                        
                                                        if ($v0_9 - 0x1f4 >= 0x64)
                                                        {
                                                            $v1_10 = 8;
                                                            
                                                            if ($v0_9 - 0x258 >= 0x64)
                                                            {
                                                                $v1_10 = 9;
                                                                
                                                                if ($v0_9 - 0x2bc >= 0x64)
                                                                {
                                                                    $v1_10 = 0xa;
                                                                    
                                                                    if ($v0_9 - 0x320 >= 0xc8)
                                                                        $v1_10 = 0xb;
                                                                }
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                                
                                $a0_2 = *($s0_1 + 0x13c);
                            }
                            
                            int32_t $v0_14 = (*($a0_2 + 0x160) & 0xfffffff0) | $v1_10;
                            *($a0_2 + 0x160) = $v0_14;
                            *(*($s0_1 + 0x13c) + 0x1e0) = $v0_14;
                            *(*($s0_1 + 0x13c) + 0x260) = $v0_14;
                            $v0_8 = *($s0_1 + 0x13c);
                        }
                        
                        *$v0_8 = 0x7d;
                        *(*($s0_1 + 0x13c) + 0x128) = 0x3f;
                        *(*($s0_1 + 0xb8) + 0x10) = 1;
                        private_msleep(0xa);
                        $v0_17 = 3;
                    }
                    else if ($s2_1 != 2)
                    {
                        isp_printf(1, "%s[%d] VIC failed to config DVP mode!(10bits-sensor)\\n", 
                            $s2_1);
                        $v0_17 = 3;
                    }
                    else
                    {
                        *(*($s0_1 + 0xb8) + 0xc) = 0;
                        *(*($s0_1 + 0xb8) + 0xc) = 1;
                        **($s0_1 + 0x13c) = 0x7d;
                        *(*($s0_1 + 0x13c) + 0x80) = 0x3e;
                        *(*($s0_1 + 0x13c) + 0x2cc) = 1;
                        $v0_17 = 3;
                    }
                }
                
                *($s0_1 + 0x128) = $v0_17;
                return 0;
            }
        }
    }
    
    return result;
}

