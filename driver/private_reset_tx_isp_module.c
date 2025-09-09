#include "include/main.h"


  int32_t private_reset_tx_isp_module(int32_t arg1)

{
    if (arg1)
        return 0;
    
    *0xb00000c4 |= 0x200000;
    
    for (int32_t i = 0x1f4; i; )
    {
        if (*0xb00000c4 & 0x100000)
        {
            *0xb00000c4 = (*0xb00000c4 & 0xffdfffff) | 0x400000;
            *0xb00000c4 &= 0xffbfffff;
            return 0;
        }
        
        i -= 1;
        private_msleep(2);
    }
    
    return 0xffffffff;
}

