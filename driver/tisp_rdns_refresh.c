#include "include/main.h"


  int32_t tisp_rdns_refresh(uint32_t arg1)

{
    tisp_rdns_par_refresh(arg1, 0x100, 1);
    return 0;
}

