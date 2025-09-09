#include "include/main.h"


  int32_t tisp_top_sel()

{
    /* tailcall */
    return system_reg_write(0xc, system_reg_read(0xc) | 0x80000000);
}

