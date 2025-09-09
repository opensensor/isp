#include "include/main.h"


  int32_t tisp_lsc_gain_update(uint32_t arg1)

{
    lsc_gain_curr = arg1;
    return 0;
}

