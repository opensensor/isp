#include "include/main.h"


  int32_t tisp_sharpen_refresh(int32_t arg1)

{
    tisp_sharpen_par_refresh(arg1, 0x100, 1);
    return 0;
}

