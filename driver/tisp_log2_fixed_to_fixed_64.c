#include "include/main.h"


  int32_t tisp_log2_fixed_to_fixed_64(uint32_t arg1, uint32_t arg2, int32_t arg3, char arg4)

{
    uint32_t $s1 = arg4;
    return tisp_log2_int_to_fixed_64(arg1, arg2, $s1, 0) - (arg3 << ($s1 & 0x1f));
}

