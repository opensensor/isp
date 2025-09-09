#include "include/main.h"


  int32_t tisp_bcsh_s_hue(char arg1)

{
    uint32_t $s0 = arg1;
    bcsh_hue = ($s0 * 0x78 - 1) / 0x100 + 1;
    tiziano_bcsh_update();
    data_9a6fc = $s0;
    return 0;
}

