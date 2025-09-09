#include "include/main.h"


  int32_t tisp_dpc_refresh(int32_t arg1)

{
    tisp_dpc_par_refresh(arg1, 0x100, 1);
    return 0;
}

