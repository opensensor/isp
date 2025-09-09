#include "include/main.h"


  int32_t tisp_set_bcsh_hue(char arg1)

{
    uint32_t $s0 = arg1;
    tisp_bcsh_s_hue($s0);
    uint32_t var_10_52 = $s0;
    return isp_printf(0, "%s[%d] VIC failed to config DVP SONY mode!(10bits-sensor)\\n", 
        "tisp_set_bcsh_hue");
}

