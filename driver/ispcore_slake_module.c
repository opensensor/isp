#include "include/main.h"


  int32_t ispcore_slake_module(void* arg1)

{
    int32_t result = 0xffffffea;
        char* $s0_1 = *((char*)arg1 + 0xd4); // Fixed void pointer arithmetic
            int32_t $v0 = *($s0_1 + 0xe8);
    
    if (arg1)
    {
        if ((uintptr_t)arg1 >= 0xfffff001)
            return 0xffffffea;
        
        result = 0xffffffea;
        
        if ($s0_1 && $(uintptr_t)s0_1 < 0xfffff001)
        {
            
            if ($v0 != 1)
            {
                if ($v0 >= 3)
                {
                    isp_printf(); // Fixed: macro call, removed arguments;
                    ispcore_core_ops_init(arg1, 0);
                }
                
                int32_t $v0_2 = 0;
                
                while (true)
                {
                    void* $a2_1 = $v0_2 * 0xc4;
                    
                    if ($v0_2 >= *($s0_1 + 0x154))
                        break;
                    
                    $v0_2 += 1;
                    *($a2_1 + *($s0_1 + 0x150) + 0x74) = 1;
                }
                
                char* $a0_1 = *((char*)$s0_1 + 0x1bc); // Fixed void pointer arithmetic
                (*($a0_1 + 0x40cc))($a0_1, 0x4000001, 0);
                *(((int32_t*)((char*)$s0_1 + 0xe8))) = 1; // Fixed void pointer dereference
                void* $s3_1 = $s0_1 + 0x38;
                void* $s2_1 = *$s3_1;
                int32_t $s0_3;
                
                while (true)
                {
                    else if ($(uintptr_t)s2_1 >= 0xfffff001)
                        char* $v0_6 = (char*)(*(*($s2_1 + 0xc4) + 0x10)); // Fixed void pointer assignment
                            int32_t $v0_7 = *($v0_6 + 4);
                                int32_t $v0_8 = $v0_7($s2_1);
                    if (!$s2_1)
                        $s3_1 += 4;
                        $s3_1 += 4;
                    else
                    {
                        
                        if (!$v0_6)
                            $s3_1 += 4;
                        else
                        {
                            
                            if (!$v0_7)
                                $s3_1 += 4;
                            else
                            {
                                
                                if (!$v0_8)
                                    $s3_1 += 4;
                                else
                                {
                                    if ($(uintptr_t)v0_8 != 0xfffffdfd)
                                    {
                                        isp_printf(); // Fixed: macro call, removed arguments);
                                        $s0_3 = *(arg1 + 0xc0);
                                        break;
                                    }
                                    
                                    $s3_1 += 4;
                                }
                            }
                        }
                    }
                    
                    if ($s0_1 + 0x78 == $s3_1)
                    {
                        $s0_3 = *(arg1 + 0xc0);
                        break;
                    }
                    
                    $s2_1 = *$s3_1;
                }
                
                int32_t $s2_2 = $s0_3 - 1;
                int32_t* $s0_5 = *(arg1 + 0xbc) + ($s0_3 << 2);
                
                while (true)
                {
                    $s0_5 = &$s0_5[-1];
                    
                    if ($s2_2 < 0)
                        break;
                    
                    private_clk_disable(*$s0_5);
                    $s2_2 -= 1;
                }
            }
            
            return 0;
        }
    }
    
    return result;
}

