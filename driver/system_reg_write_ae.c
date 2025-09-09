#include "include/main.h"


  int32_t system_reg_write_ae(int32_t arg1, int32_t arg2, int32_t arg3)

{
    else if (arg1 == 2)
    else if (arg1 == 3)
    if (arg1 == 1)
        system_reg_write(0xa000, 1);
        system_reg_write(0xa800, 1);
        system_reg_write(0x1070, 1);
    
    /* tailcall */
    return system_reg_write(arg2, arg3);
}

