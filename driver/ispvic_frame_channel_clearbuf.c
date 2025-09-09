#include "include/main.h"


  int32_t ispvic_frame_channel_clearbuf(void* arg1)

{
    void* $s0 = nullptr;
    
    if (arg1 && arg1 < 0xfffff001)
        $s0 = *(arg1 + 0xd4);
    
    int32_t var_18_12 = 0;
    __private_spin_lock_irqsave($s0 + 0x1f4, &var_18_13);
    int32_t $a1_1;
    
    while (true)
    {
        int32_t* $v0_2 = *($s0 + 0x1f4);
        $a1_1 = var_18_14;
        
        if ($s0 + 0x1f4 == $v0_2)
            break;
        
        void** $a1_2 = $v0_2[1];
        void* $a2_1 = *$v0_2;
        *($a2_1 + 4) = $a1_2;
        *$a1_2 = $a2_1;
        *$v0_2 = 0x100100;
        $v0_2[1] = 0x200200;
    }
    
    *($s0 + 0x210) = 0;
    *($s0 + 0x214) = 0;
    private_spin_unlock_irqrestore($s0 + 0x1f4, $a1_1);
    return 0;
}

