#include "include/main.h"


  int32_t tiziano_defog_dn_params_refresh()

{
    tiziano_defog_params_refresh();
    tiziano_defog_params_init();
    tiziano_defog_set_reg_params();
    return 0;
}

