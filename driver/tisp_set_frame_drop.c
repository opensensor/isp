#include "include/main.h"


  int32_t tisp_set_frame_drop(int32_t arg1, int32_t* arg2, int32_t arg3)

{
    uint32_t $a1 = arg2[1];
    
    if ($a1 >= 0x20)
    {
        isp_printf(1, "/tmp/snap%d.%s", arg3);
        return 0xffffffff;
    }
    
    int32_t result = *arg2;
    int32_t $s0_1 = (arg1 + 0x98) << 8;
    
    if (!result)
    {
        system_reg_write($s0_1 + 0x130, 0);
        system_reg_write($s0_1 + 0x134, 1);
    }
    else
    {
        result = 0;
        system_reg_write($s0_1 + 0x130, $a1);
        system_reg_write($s0_1 + 0x134, arg2[2]);
    }
    
    return result;
}

