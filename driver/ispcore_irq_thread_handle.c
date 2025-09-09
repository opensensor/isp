#include "include/main.h"


  int32_t ispcore_irq_thread_handle(void* arg1)

{
    char* $s5 = (char*)(nullptr); // Fixed void pointer assignment
    int32_t var_30 = 0;
        char* $v0_3 = (char*)(*(*(arg1 + 0xc4) + 0xc)); // Fixed void pointer assignment
            int32_t $v0_4 = *($v0_3 + 8);
    
    if (arg1 && (uintptr_t)arg1 < 0xfffff001)
        $s5 = *(arg1 + 0xd4);
    
    
    if (arg1)
    {
        
        if ($v0_3)
        {
            
            if ($v0_4)
                $v0_4(arg1, 0x2000015, &var_30);
        }
    }
    
    if ($s5)
    {
        int32_t* $s7_1 = $s5 + 0x180;
                                char* $v0_11 = (char*)(*(*(arg1 + 0xc4) + 0xc)); // Fixed void pointer assignment
                                    int32_t $v0_12 = *($v0_11 + 8);
        
        for (int32_t i = 0; i != 7; )
        {
            if (!*$s7_1)
                i += 1;
            else
            {
                if (i - 1 < 6)
                    switch (jump_table_7de0c[i - 1])
                    {
                        case 0x76be0:
                        {
                            if (!(uintptr_t)arg1)
                                *((int32_t*)((char*)$s5 + 0x1a8)) = 0; // Fixed void pointer dereference
                            else
                            {
                                
                                if (!$v0_11)
                                    *((int32_t*)((char*)$s5 + 0x1a8)) = 0; // Fixed void pointer dereference
                                else
                                {
                                    
                                    if ($v0_12)
                                        $v0_12(arg1, 0x2000010, $s5 + 0x1ac);
                                    
                                    *((int32_t*)((char*)$s5 + 0x1a8)) = 0; // Fixed void pointer dereference
                                }
                            }
                            break;
                        }
                    }
                
                var_30_11 = $s7_1[1];
                
                if (i == 5)
                    i += 1;
                else if (*(*($s5 + 0x120) + 0xf0))
                    i += 1;
                else
                {
                    ispcore_sensor_ops_ioctl(arg1);
                    *$s7_1 = 0;
                    i += 1;
                }
            }
            
            $s7_1 = &$s7_1[2];
        }
    }
    
    return 0;
}

