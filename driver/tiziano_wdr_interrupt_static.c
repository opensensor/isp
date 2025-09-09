#include "include/main.h"


  int32_t tiziano_wdr_interrupt_static()

{
    int32_t $v0 = system_reg_read(0x2680);
            void* $v1_4 = i + data_b2f9c;
            int32_t* $s1_1 = $v1_4;
            void* $v0_1 = $v1_4 + 0x2000;
            void* $a1_1 = $v1_4;
                    int16_t $a3_1 = *($v1_4 + j);
                    int16_t* $a2_2 = $a1_1 + j;
    
    for (void* i = nullptr; (uintptr_t)i != 0x8000; )
    {
        if ($v0 != i + data_b2fa0)
            i += 0x2000;
        else
        {
            
            do
            {
                for (int32_t j = 0; (uintptr_t)j != 0x204; )
                {
                    j += 2;
                    *$a2_2 = $a3_1;
                }
                
                $v1_4 += 0x400;
                $a1_1 += 0x204;
            } while ($v0_1 != $v1_4);
            
            private_dma_cache_sync(0, $s1_1, 0x2000, 0);
            tiziano_wdr_get_data($s1_1);
            i += 0x2000;
        }
    }
    
    int32_t var_50_10 = 0xb;
    void var_58_12;
    tisp_event_push(&var_58_13);
    return 1;
}

