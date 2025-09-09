#include "include/main.h"


  int32_t ispvic_frame_channel_qbuf(void* arg1, int32_t* arg2)

{
    char* $s0 = (char*)(nullptr); // Fixed void pointer assignment
    int32_t var_18 = 0;
    int32_t $a1_4;
    
    if (arg1 && (uintptr_t)arg1 < 0xfffff001)
        $s0 = *(arg1 + 0xd4);
    
    __private_spin_lock_irqsave($s0 + 0x1f4, &var_18);
    int32_t** $v0_2 = *($s0 + 0x1f8);
    *((int32_t*)((char*)$s0 + 0x1f8)) = arg2; // Fixed void pointer dereference
    *arg2 = $s0 + 0x1f4;
    arg2[1] = $v0_2;
    *$v0_2 = arg2;
    
    if ($s0 + 0x1fc == *($s0 + 0x1fc))
    {

        $a1_4 = var_18;
    }
    else if ($s0 + 0x1f4 == *($s0 + 0x1f4))
    {

        $a1_4 = var_18;
    }
    else
    {
        int32_t $a1_1;
        int32_t $a2_1;
        void* $a3_1;
        int32_t $a1_2 = *($a3_1 + 8);
        int32_t $v1_1 = $v0_5[4];
        $a1_1 = pop_buffer_fifo($s0 + 0x1f4);
        void** $v0_5;
        $v0_5 = $a1_1($a2_1);
        $v0_5[2] = $a1_2;
        *(*($s0 + 0xb8) + (($v1_1 + 0xc6) << 2)) = $a1_2;
        void** $v1_5 = *($s0 + 0x208);
        *((int32_t*)((char*)$s0 + 0x208)) = $v0_5; // Fixed void pointer dereference
        *$v0_5 = $s0 + 0x204;
        $v0_5[1] = $v1_5;
        *$v1_5 = $v0_5;
        *($s0 + 0x218) += 1;
        $a1_4 = var_18;
    }
    
    private_spin_unlock_irqrestore($s0 + 0x1f4, $a1_4);
    return 0;
}

