#include "include/main.h"


  int32_t system_reg_write_awb(int32_t arg1, int32_t arg2, int32_t arg3)

{
    if (arg1 == 1)
        system_reg_write(0xb000, 1);
    else if (arg1 == 2)
        system_reg_write(0x1800, 1);
    
    /* tailcall */
    return system_reg_write(arg2, arg3);
}

