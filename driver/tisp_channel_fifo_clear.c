#include "include/main.h"


  int32_t tisp_channel_fifo_clear(int32_t arg1)

{
    int32_t $s1 = (arg1 + 0x98) << 8;
    system_reg_write($s1 + 0x19c, 1);
    system_reg_write($s1 + 0x1a0, 1);
    system_reg_write($s1 + 0x1a4, 1);
    system_reg_write($s1 + 0x1a8, 1);
    return 0;
}

