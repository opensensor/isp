#include "include/main.h"


  int32_t tx_isp_csi_slake_subdev(void* arg1)

{
    int32_t result = 0xffffffea;
        char* $s0_1 = *((char*)arg1 + 0xd4); // Fixed void pointer arithmetic
            int32_t $v1_2 = *($s0_1 + 0x128);
    
    if (arg1)
    {
        if ((uintptr_t)arg1 >= 0xfffff001)
            return 0xffffffea;
        
        result = 0xffffffea;
        
        if ($s0_1 && $(uintptr_t)s0_1 < 0xfffff001)
        {
            int32_t entry_$a2;
            
            if ($v1_2 == 4)
            {
                entry_$a2 = csi_video_s_stream(arg1, 0, entry_$a2);
                $v1_2 = *($s0_1 + 0x128);
            }
            
            void* $s2_1 = $s0_1 + 0x12c;
            
            if ($v1_2 == 3)
            {
                csi_core_ops_init(arg1, 0, entry_$a2);
                $s2_1 = $s0_1 + 0x12c;
            }
            
            private_mutex_lock($s2_1);
            
            if (*($s0_1 + 0x128) == 2)
            {
                char* $v0 = *((char*)arg1 + 0xbc); // Fixed void pointer arithmetic
                    int32_t $s0_2 = *(arg1 + 0xc0);
                    int32_t $s1_2 = $s0_2 - 1;
                    int32_t* $s0_4 = $v0 + ($s0_2 << 2);
                *(((int32_t*)((char*)$s0_1 + 0x128))) = 1; // Fixed void pointer dereference
                
                if ($v0 && $(uintptr_t)v0 < 0xfffff001)
                {
                    
                    while (true)
                    {
                        $s0_4 = &$s0_4[-1];
                        
                        if ($s1_2 < 0)
                            break;
                        
                        private_clk_disable(*$s0_4);
                        $s1_2 -= 1;
                    }
                }
            }
            
            private_mutex_unlock($s2_1);
            return 0;
        }
    }
    
    return result;
}

