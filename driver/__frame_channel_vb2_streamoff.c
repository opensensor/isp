#include "include/main.h"


  int32_t __frame_channel_vb2_streamoff(void* arg1, int32_t arg2, int32_t arg3)

{
    int32_t $a0;
    char const* const $a1;
    
    if (*(arg1 + 0x24) == arg2)
    {
        if (*(arg1 + 0x230) & 1)
        {
            __vb2_queue_cancel(arg1 + 0x24);
            *(arg1 + 0x2d0) = 3;
            return 0;
        }
        
        $a1 = "streamon";
        $a0 = 1;
    }
    else
    {
        $a1 = "snapraw timeout!\\n";
        $a0 = 2;
    }
    
    isp_printf($a0, $a1, arg3);
    return 0xffffffea;
}

