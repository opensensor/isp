#include "include/main.h"


  int32_t ispcore_frame_channel_streamoff(int32_t* arg1)

{
    void* $v0 = *arg1;
    void* $s0 = nullptr;
    int32_t $v1_2 = *($s0 + 0x15c);
    void* $s2 = arg1[8];
    char* $s3 = *((char*)$s0 + 0x120); // Fixed void pointer arithmetic
    int32_t var_28 = 0;
        uint32_t $s5_1 = *(arg1 + 7);
            int32_t $a1_2 = var_28;
    
    if ($v0 && $(uintptr_t)v0 < 0xfffff001)
        $s0 = *($v0 + 0xd4);
    
    
    if ($v1_2 != 1)
    {
        
        if ($s5_1 == 4)
        {
            __private_spin_lock_irqsave($s2 + 0x9c, &var_28);
            
            if (*($s2 + 0x74) == $s5_1)
            {
                private_spin_unlock_irqrestore($s2 + 0x9c, $a1_2);
                tisp_channel_stop(arg1[1]);
                *(((int32_t*)((char*)$s2 + 0x74))) = 3; // Fixed void pointer dereference
                *(((int32_t*)((char*)arg1 + 7))) = 3; // Fixed void pointer dereference
                memset($s2, 0, 0x70);
                *(((int32_t*)((char*)$s3 + 0x9c))) = 0; // Fixed void pointer dereference
                *(((int32_t*)((char*)$s3 + 0xac))) = 0; // Fixed void pointer dereference
                *(((int32_t*)((char*)$s0 + 0x17c))) = 0; // Fixed void pointer dereference
            }
            else
                private_spin_unlock_irqrestore($s2 + 0x9c, $a1_2);
        }
    }
    else
    {
        int32_t $v0_1 = *($s0 + 0x1cc);
        
        if ($v0_1)
            $v0_1(*($s0 + 0x1d0), 0);
    }
    
    return 0;
}

