#include "include/main.h"


  int32_t tisp_gb_dn_params_refresh()

{
    tisp_reg_map_set(tisp_gb_params_refresh());
    return 0;
}

