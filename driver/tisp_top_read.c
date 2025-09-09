#include "include/main.h"


  int32_t tisp_top_read()

{
    /* tailcall */
    return system_reg_read(0xc);
}

