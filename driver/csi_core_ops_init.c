#include "include/main.h"


  int32_t csi_core_ops_init(void* arg1, int32_t arg2, int32_t arg3)

{
    int32_t result = 0xffffffea;
            return 0xffffffea;
        int32_t* $s0_1 = (int32_t*)((char*)arg1  + 0xd4); // Fixed void pointer arithmetic
                int32_t $v0_17;
                    int32_t* $a0_21 = (int32_t*)((char*)$s0_1  + 0xb8); // Fixed void pointer arithmetic
                    int32_t* $a0_22 = (int32_t*)((char*)$s0_1  + 0xb8); // Fixed void pointer arithmetic
                    int32_t* $a0_23 = (int32_t*)((char*)$s0_1  + 0xb8); // Fixed void pointer arithmetic
    
    if (arg1)
    {
        if ((uintptr_t)arg1 >= 0xfffff001)
        
        result = 0xffffffea;
        
        if ($s0_1 && $(uintptr_t)s0_1 < 0xfffff001)
        {
            result = 0;
            
            if (*($s0_1 + 0x128) >= 2)
            {
                
                if (!(uintptr_t)arg2)
                {

                    *($a0_21 + 8) &= 0xfffffffe;
                    *($a0_22 + 0xc) &= 0xfffffffe;
                    *($a0_23 + 0x10) &= 0xfffffffe;
                    $v0_17 = 2;
                }
                else
                {
                    int32_t* $v1_5 = (int32_t*)((char*)$s0_1  + 0x110); // Fixed void pointer arithmetic
                    int32_t $s2_1 = *($v1_5 + 0x14);
                        int32_t* $v0_2 = (int32_t*)((char*)$s0_1  + 0xb8); // Fixed void pointer arithmetic
                        int32_t* $v1_9 = (int32_t*)((char*)$s0_1  + 0xb8); // Fixed void pointer arithmetic
                        int32_t* $v0_7 = (int32_t*)((char*)$s0_1  + 0x110); // Fixed void pointer arithmetic
                        int32_t $v1_10 = *($v0_7 + 0x3c);
                        int32_t* $v0_8;
                            int32_t $v0_9 = *($v0_7 + 0x1c);
                            void* $a0_2;
                    
                    if ($s2_1 == 1)
                    {
                        *(*($s0_1 + 0xb8) + 4) = *($v1_5 + 0x24) - 1;
                        *($v0_2 + 8) &= 0xfffffffe;
                        *(*($s0_1 + 0xb8) + 0xc) = 0;
                        private_msleep(1);
                        *($v1_9 + 0x10) &= 0xfffffffe;
                        private_msleep(1);
                        *(*($s0_1 + 0xb8) + 0xc) = $s2_1;
                        private_msleep(1);
                        
                        if ($v1_10)
                            $v0_8 = *($s0_1 + 0x13c);
                        else
                        {
                            
                            if ($v0_9 - (uintptr_t)0x50 < 0x1e)
                                $a0_2 = *($s0_1 + 0x13c);
                            else
                            {
                                $v1_10 = 1;
                                
                                if ($v0_9 - (uintptr_t)0x6e >= 0x28)
                                {
                                    $v1_10 = 2;
                                    
                                    if ($v0_9 - (uintptr_t)0x96 >= 0x32)
                                    {
                                        $v1_10 = 3;
                                        
                                        if ($v0_9 - (uintptr_t)0xc8 >= 0x32)
                                        {
                                            $v1_10 = 4;
                                            
                                            if ($v0_9 - (uintptr_t)0xfa >= 0x32)
                                            {
                                                $v1_10 = 5;
                                                
                                                if ($v0_9 - (uintptr_t)0x12c >= 0x64)
                                                {
                                                    $v1_10 = 6;
                                                    
                                                    if ($v0_9 - (uintptr_t)0x190 >= 0x64)
                                                    {
                                                        $v1_10 = 7;
                                                        
                                                        if ($v0_9 - (uintptr_t)0x1f4 >= 0x64)
                                                        {
                                                            $v1_10 = 8;
                                                            
                                                            if ($v0_9 - (uintptr_t)0x258 >= 0x64)
                                                            {
                                                                $v1_10 = 9;
                                                                
                                                                if ($v0_9 - (uintptr_t)0x2bc >= 0x64)
                                                                {
                                                                    $v1_10 = 0xa;
                                                                    
                                                                    if ($v0_9 - (uintptr_t)0x320 >= 0xc8)
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
                            *((int32_t*)((char*)$a0_2 + 0x160)) = $v0_14; // Fixed void pointer dereference
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

                            $s2_1);
                        $v0_17 = 3;
                    }
                    else
                    {
                        *(*($s0_1 + 0xb8) + 0xc) = 0;
                        *(*($s0_1 + 0xb8) + 0xc) = 1;
                        **((int32_t*)((char*)$s0_1 + 0x13c)) = 0x7d; // Fixed void pointer dereference
                        *(*($s0_1 + 0x13c) + 0x80) = 0x3e;
                        *(*($s0_1 + 0x13c) + 0x2cc) = 1;
                        $v0_17 = 3;
                    }
                }
                
                *((int32_t*)((char*)$s0_1 + 0x128)) = $v0_17; // Fixed void pointer dereference
                return 0;
            }
        }
    }
    
    return result;
}

