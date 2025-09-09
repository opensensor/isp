#include "include/main.h"


  int32_t tisp_g_ncuinfo(int32_t arg1)

{
    if (arg1)
        return 0;
    
    int32_t entry_$a2;
    isp_printf(2, "%s[%d] VIC failed to config DVP mode!(10bits-sensor)\\n", entry_$a2);
    return 0xffffffff;
}

