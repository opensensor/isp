#include "include/main.h"


  int32_t frame_channel_open(int32_t arg1, void* arg2)

{
    void* $s0 = *(arg2 + 0x70);
    
    if (!$s0 || $s0 >= 0xfffff001)
        return 0xffffffea;
    
    private_mutex_lock($s0 + 0x2c4);
    
    if (*($s0 + 0x2d0) < 2)
    {
        private_mutex_unlock($s0 + 0x2c4);
        isp_printf(2, 
            "\\t\\t\\t please use this cmd: \\n\\t"echo snapraw savenum > /proc/jz/isp/isp-w02"\\n", 
            *($s0 + 0x2c0));
        return 0xffffffff;
    }
    
    private_mutex_unlock($s0 + 0x2c4);
    memset($s0 + 0x23c, 0, 0x70);
    *($s0 + 0x2e0) = 0;
    *($s0 + 0x2e4) = 0;
    private_init_completion($s0 + 0x2d4);
    __vb2_queue_free($s0 + 0x24, *($s0 + 0x20c));
    *($s0 + 0x2d0) = 3;
    return 0;
}

