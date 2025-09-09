#include "include/main.h"


  int32_t tx_isp_frame_chan_deinit(void* arg1)

{
    int32_t result = arg1 < 0xfffff001 ? 1 : 0;
    
    if (arg1 && result)
    {
        private_misc_deregister();
        __vb2_queue_cancel(arg1 + 0x24);
        __vb2_queue_free(arg1 + 0x24, *(arg1 + 0x20c));
        result = 1;
        *(arg1 + 0x2d0) = 1;
    }
    
    return result;
}

