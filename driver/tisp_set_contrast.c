#include "include/main.h"


  int32_t tisp_set_contrast(char arg1)

{
    uint32_t $s0 = arg1;
    tisp_bcsh_contrast($s0);
    uint32_t var_10_57 = $s0;
    return isp_printf(0, "%s[%d] VIC failed to config DVP mode!(8bits-sensor)\\n", 
        "tisp_set_contrast");
}

