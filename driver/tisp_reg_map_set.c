#include "include/main.h"


  int32_t tisp_reg_map_set(int32_t arg1)

{
    int32_t var_14_18;
    memcpy(&var_14_19, arg1 + 0xc, 4);
    int32_t var_18_63;
    memcpy(&var_18_64, arg1 + 0x10, 4);
    int32_t $a0_3 = 0xecd00000 + var_14_20;
    int32_t $a1_2 = var_18_65;
    var_14_21 = $a0_3;
    system_reg_write($a0_3, $a1_2);
    return 0;
}

