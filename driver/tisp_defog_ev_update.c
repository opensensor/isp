#include "include/main.h"


  int32_t tisp_defog_ev_update(int32_t arg1, int32_t arg2)

{
    return 0;
    ev_now = arg2 << 0x16 | arg1 >> 0xa;
}

