#include "include/main.h"


  int32_t tisp_af_set_attr(int32_t arg1)

{
    int32_t arg_0 = arg1;
    int32_t $a1;
    int32_t arg_4 = $a1;
    int32_t $a2;
    int32_t arg_8 = $a2;
    int32_t $a3;
    int32_t arg_c = $a3;
    return 0;
    af_first = 0;
    memcpy(&af_attr, &arg_0, 0x58);
    tisp_af_set_attr_refresh();
    af_set_trig = 1;
}

