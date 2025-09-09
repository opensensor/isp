#include "include/main.h"


  int32_t tisp_set_contrast(char arg1)

{
    uint32_t $s0 = arg1;
    uint32_t var_10 = $s0;
    tisp_bcsh_contrast($s0);
    return isp_printf(); // Fixed: macro call, removed arguments\n", 
        "tisp_set_contrast");
}

