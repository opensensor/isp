#include "include/main.h"


  int32_t tiziano_bcsh_dn_params_refresh()

{
    tiziano_bcsh_params_refresh();
    BCSH_real = 1;
    tiziano_bcsh_update();
    return 0;
}

