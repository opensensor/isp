#include "include/main.h"


  int32_t tx_isp_subdev_pipo(void* arg1, uint32_t arg2)

{
    char* $s0 = (char*)(nullptr); // Fixed void pointer assignment
        uint32_t raw_pipe_1 = raw_pipe;
        int32_t i = 0;
            int32_t $a1 = (i + 0xc6) << 2;
    
    if (arg1 && (uintptr_t)arg1 < 0xfffff001)
        $s0 = *(arg1 + 0xd4);
    
    *((int32_t*)((char*)$s0 + 0x20c)) = 1; // Fixed void pointer dereference
    raw_pipe = arg2;
    
    if (!(uintptr_t)arg2)
        *((int32_t*)((char*)$s0 + 0x214)) = 0; // Fixed void pointer dereference
    else
    {
        *((int32_t*)((char*)$s0 + 0x204)) = $s0 + 0x204; // Fixed void pointer dereference
        *((int32_t*)((char*)$s0 + 0x208)) = $s0 + 0x204; // Fixed void pointer dereference
        *((int32_t*)((char*)$s0 + 0x1f4)) = $s0 + 0x1f4; // Fixed void pointer dereference
        *((int32_t*)((char*)$s0 + 0x1f8)) = $s0 + 0x1f4; // Fixed void pointer dereference
        *((int32_t*)((char*)$s0 + 0x1fc)) = $s0 + 0x1fc; // Fixed void pointer dereference
        *((int32_t*)((char*)$s0 + 0x200)) = $s0 + 0x1fc; // Fixed void pointer dereference
        private_spin_lock_init();
        **&raw_pipe = ispvic_frame_channel_qbuf;
        *((int32_t*)((char*)raw_pipe_1 + 8)) = ispvic_frame_channel_clearbuf; // Fixed void pointer dereference
        *((int32_t*)((char*)raw_pipe_1 + 0xc)) = ispvic_frame_channel_s_stream; // Fixed void pointer dereference
        *((int32_t*)((char*)raw_pipe_1 + 0x10)) = arg1; // Fixed void pointer dereference
        void** $v0_3 = $s0 + 0x168;
        
        do
        {
            $v0_3[4] = i;
            void*** $a0_1 = *($s0 + 0x200);
            *((int32_t*)((char*)$s0 + 0x200)) = $v0_3; // Fixed void pointer dereference
            $v0_3[1] = $a0_1;
            *$v0_3 = $s0 + 0x1fc;
            *$a0_1 = $v0_3;
            i += 1;
            *(*($s0 + 0xb8) + $a1) = 0;
            $v0_3 = &$v0_3[7];
        } while (i != 5);
        
        *((int32_t*)((char*)$s0 + 0x214)) = 1; // Fixed void pointer dereference
    }
    
    return 0;
}

