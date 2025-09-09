#include "include/main.h"


  int32_t tx_isp_send_event_to_remote(void* arg1)

{
    if (arg1)
    {
        void* $a0 = *(arg1 + 0xc);
        
        if ($a0)
        {
            int32_t $t9_1 = *($a0 + 0x1c);
            
            if ($t9_1)
❓                /* jump -> $t9_1 */
        }
    }
    
    return 0xfffffdfd;
}

