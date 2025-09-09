#include "include/main.h"


  int32_t tisp_reg_map_get(int32_t arg1, int32_t arg2, int32_t* arg3)

{
    int32_t arg_0 = arg1;
    int32_t var_18_61 = system_reg_read(0xecd00000 + arg1);
    memcpy(arg2 + 0xc, &arg_0, 4);
    memcpy(arg2 + 0x10, &var_18_62, 4);
    *arg3 = 8;
    return 0;
}

