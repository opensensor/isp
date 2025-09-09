#include "include/main.h"


  int32_t tx_isp_subdev_pipo(void* arg1, uint32_t arg2)

{
    void* $s0 = nullptr;
    
    if (arg1 && arg1 < 0xfffff001)
        $s0 = *(arg1 + 0xd4);
    
    *($s0 + 0x20c) = 1;
    raw_pipe = arg2;
    
    if (!arg2)
        *($s0 + 0x214) = 0;
    else
    {
        *($s0 + 0x204) = $s0 + 0x204;
        *($s0 + 0x208) = $s0 + 0x204;
        *($s0 + 0x1f4) = $s0 + 0x1f4;
        *($s0 + 0x1f8) = $s0 + 0x1f4;
        *($s0 + 0x1fc) = $s0 + 0x1fc;
        *($s0 + 0x200) = $s0 + 0x1fc;
        private_spin_lock_init();
        **&raw_pipe = ispvic_frame_channel_qbuf;
        uint32_t raw_pipe_1 = raw_pipe;
        *(raw_pipe_1 + 8) = ispvic_frame_channel_clearbuf;
        *(raw_pipe_1 + 0xc) = ispvic_frame_channel_s_stream;
        *(raw_pipe_1 + 0x10) = arg1;
        int32_t i = 0;
        void** $v0_3 = $s0 + 0x168;
        
        do
        {
            $v0_3[4] = i;
            void*** $a0_1 = *($s0 + 0x200);
            *($s0 + 0x200) = $v0_3;
            $v0_3[1] = $a0_1;
            *$v0_3 = $s0 + 0x1fc;
            *$a0_1 = $v0_3;
            int32_t $a1 = (i + 0xc6) << 2;
            i += 1;
            *(*($s0 + 0xb8) + $a1) = 0;
            $v0_3 = &$v0_3[7];
        } while (i != 5);
        
        *($s0 + 0x214) = 1;
    }
    
    return 0;
}

