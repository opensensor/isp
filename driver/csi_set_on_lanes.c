#include "include/main.h"


  int32_t csi_set_on_lanes(void* arg1, char arg2)

{
    isp_printf(0, "Can\'t output the width(%d)!\\n", "csi_set_on_lanes");
    void* $v1 = *(arg1 + 0xb8);
    *($v1 + 4) = ((arg2 - 1) & 3) | (*($v1 + 4) & 0xfffffffc);
    return 0;
}

