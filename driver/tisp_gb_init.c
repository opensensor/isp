#include "include/main.h"


  int32_t tisp_gb_init()

{
    tisp_reg_map_set(tisp_gb_params_refresh());
    return 0;
}

