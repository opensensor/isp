#include "include/main.h"


  int32_t ispvic_frame_channel_clearbuf(void* arg1)

{
    char* $s0 = (char*)(nullptr); // Fixed void pointer assignment
    int32_t var_18 = 0;
    int32_t $a1_1;
        int32_t* $v0_2 = *($s0 + 0x1f4);
        char* $a2_1 = (char*)(*$v0_2); // Fixed void pointer assignment
    
    if (arg1 && (uintptr_t)arg1 < 0xfffff001)
        $s0 = *(arg1 + 0xd4);
    
    __private_spin_lock_irqsave($s0 + 0x1f4, &var_18);
    
    while (true)
    {
        $a1_1 = var_18;
        
        if ($s0 + 0x1f4 == $v0_2)
            break;
        
        void** $a1_2 = $v0_2[1];
        *((int32_t*)((char*)$a2_1 + 4)) = $a1_2; // Fixed void pointer dereference
        *$a1_2 = $a2_1;
        *$v0_2 = 0x100100;
        $v0_2[1] = 0x200200;
    }
    
    *((int32_t*)((char*)$s0 + 0x210)) = 0; // Fixed void pointer dereference
    *((int32_t*)((char*)$s0 + 0x214)) = 0; // Fixed void pointer dereference
    private_spin_unlock_irqrestore($s0 + 0x1f4, $a1_1);
    return 0;
}

