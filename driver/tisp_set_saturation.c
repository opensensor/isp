#include "include/main.h"


  int32_t tisp_set_saturation(char arg1)

{
    uint32_t $s0 = arg1;
    tisp_bcsh_saturation($s0);
    uint32_t var_10_56 = $s0;
    return isp_printf(0, "sensor type is BT601!\\n", "tisp_set_saturation");
}

