#include "include/main.h"


  int32_t tx_isp_create_graph_and_nodes(void* arg1)

{
    int32_t* $s4 = arg1 + 0x84;
    int32_t* $s7 = arg1 + 0x84;
    int32_t i = 0;
        char* $v0_3 = private_platform_get_drvdata(*$s7);
    
    while (i < *(arg1 + 0x80))
    {
        
        if (!$v0_3)
        {
            isp_printf(); // Fixed: macro call, removed arguments;
            i += 1;
        }
        else if ($(uintptr_t)v0_3 >= 0xfffff001)
        {
            isp_printf(); // Fixed: macro call, removed arguments;
            i += 1;
        }
        else if (*$v0_3 != 1)
            i += 1;
        else
        {
            *(arg1 + ((($v0_3[3] & 0xf) + 0xe) << 2)) = $v0_3;
            i += 1;
        }
        
        $s7 = &$s7[2];
    }
    
    int32_t* $s3 = arg1 + 0x84;
    int32_t i_1 = 0;
    
    while (i_1 < *(arg1 + 0x80))
    {
        char* $v0_6 = private_platform_get_drvdata(*$s3);
            uint32_t $a2_2 = $v0_6[2];
            char* $a0_3 = (char*)(*(arg1 + ((($a2_2 & 0xf) + 0xe) << 2))); // Fixed void pointer assignment
        
        if (*$v0_6 != 2)
            i_1 += 1;
        else
        {
            
            if (!$a0_3)
            {
                isp_printf(); // Fixed: macro call, removed arguments;
                break;
            }
            
            *($a0_3 + ((($v0_6[3] & 0xf) + 0xe) << 2)) = $v0_6;
            i_1 += 1;
        }
        
        $s3 = &$s3[2];
    }
    
    int32_t $s1 = 0;
    
    while (true)
    {
        int32_t result = $s1 < *(arg1 + 0x80) ? 1 : 0;
        void* $v0_7 = private_platform_get_drvdata(*$s4);
        int32_t $v0_8 = *($v0_7 + 0x30);
            int32_t $a0_4 = *($v0_7 + 8);
                void* $s2_1 = &(arg1 + 0x84)[$s1 * 2];
                    char* $v0_12 = (char*)(private_platform_get_drvdata(*($s2_1 - 8))); // Fixed void pointer assignment
        
        if (!result)
            return result;
        
        
        if ($v0_8)
        {
            *(((void**)((char*)$v0_7 + 0x14))) = $v0_8; // Fixed void pointer dereference
            *(((void**)((char*)$v0_7 + 0x10))) = $a0_4; // Fixed void pointer dereference
            *(((void**)((char*)$v0_7 + 0xc))) = 0xff; // Fixed void pointer dereference
            
            if (private_misc_register($v0_7 + 0xc) < 0)
            {
                isp_printf(); // Fixed: macro call, removed arguments);
                
                while (true)
                {
                    $s1 -= 1;
                    
                    if ($(uintptr_t)s1 == 0xffffffff)
                        break;
                    
                    
                    if (*($v0_12 + 0x30))
                        private_misc_deregister($v0_12 + 0xc);
                    
                    $s2_1 -= 8;
                }
                
                return 0xfffffffe;
            }
        }
        
        int32_t $a3_1 = *($v0_7 + 0x34);
        
        if ($a3_1)
            private_proc_create_data(*($v0_7 + 8), 0x124, *(arg1 + 0x11c), $a3_1, $v0_7);
        
        $s1 += 1;
        $s4 = &$s4[2];
    }
}

