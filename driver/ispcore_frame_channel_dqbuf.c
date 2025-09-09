#include "include/main.h"


  int32_t ispcore_frame_channel_dqbuf(void* arg1, int32_t arg2)

{
    if (!arg1)
        return 0;
    
    tx_isp_send_event_to_remote(arg1, 0x3000006, arg2);
    return 0;
}

