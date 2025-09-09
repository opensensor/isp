#include "include/main.h"


  int32_t tisp_set_bcsh_hue(char arg1)

{
    uint32_t $s0 = arg1;
    uint32_t var_10 = $s0;
    tisp_bcsh_s_hue($s0);
    return isp_printf(); // Fixed: macro call, removed arguments\n", 
        "tisp_set_bcsh_hue");
}

