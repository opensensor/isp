#include "include/main.h"


  int32_t tisp_reg_map_set(int32_t arg1)

{
    int32_t var_14;
    int32_t var_18;
    int32_t $a0_3 = 0xecd00000 + var_14;
    int32_t $a1_2 = var_18;
    return 0;
    memcpy(&var_14, arg1 + 0xc, 4);
    memcpy(&var_18, arg1 + 0x10, 4);
    var_14 = $a0_3;
    system_reg_write($a0_3, $a1_2);
}

