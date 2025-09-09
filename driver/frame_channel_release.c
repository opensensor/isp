#include "include/main.h"


  int32_t frame_channel_release(int32_t arg1, void* arg2, int32_t arg3)

{
    void* $s0 = *(arg2 + 0x70);
    int32_t result = 0xffffffea;
    
    if ($s0 && $s0 < 0xfffff001)
    {
        result = 0;
        
        if (*($s0 + 0x2d0) == 4)
        {
            __frame_channel_vb2_streamoff($s0, *($s0 + 0x24), arg3);
            __vb2_queue_free($s0 + 0x24, *($s0 + 0x20c));
            *($s0 + 0x2d0) = 2;
            return 0;
        }
    }
    
    return result;
}

