#include "include/main.h"


  int32_t tisp_dmsc_refresh(int32_t arg1)

{
    tisp_dmsc_par_refresh(arg1, 0x100, 1);
    return 0;
}

