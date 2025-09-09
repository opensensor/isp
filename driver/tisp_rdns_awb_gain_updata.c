#include "include/main.h"


  int32_t tisp_rdns_awb_gain_updata(int32_t arg1, int32_t arg2)

{
    uint32_t $s2 = arg2 >> 4;
    uint32_t $s1 = arg1 >> 4;
    system_reg_write(0x3000, $s2 << 0x10 | $s1);
    int32_t $v0 = fix_point_div_32(6, 1, $s1);
    system_reg_write(0x3004, fix_point_div_32(6, 1, $s2) << 0x10 | $v0);
    /* tailcall */
    return system_reg_write(0x30ac, 1);
}

