#include "include/main.h"


  int32_t ispcore_frame_channel_streamoff(int32_t* arg1)

{
    void* $v0 = *arg1;
    void* $s0 = nullptr;
    
    if ($v0 && $v0 < 0xfffff001)
        $s0 = *($v0 + 0xd4);
    
    int32_t $v1_2 = *($s0 + 0x15c);
    void* $s2 = arg1[8];
    void* $s3 = *($s0 + 0x120);
    int32_t var_28_41 = 0;
    
    if ($v1_2 != 1)
    {
        uint32_t $s5_1 = *(arg1 + 7);
        
        if ($s5_1 == 4)
        {
            __private_spin_lock_irqsave($s2 + 0x9c, &var_28_42);
            int32_t $a1_2 = var_28_43;
            
            if (*($s2 + 0x74) == $s5_1)
            {
                private_spin_unlock_irqrestore($s2 + 0x9c, $a1_2);
                tisp_channel_stop(arg1[1]);
                *($s2 + 0x74) = 3;
                *(arg1 + 7) = 3;
                memset($s2, 0, 0x70);
                *($s3 + 0x9c) = 0;
                *($s3 + 0xac) = 0;
                *($s0 + 0x17c) = 0;
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

