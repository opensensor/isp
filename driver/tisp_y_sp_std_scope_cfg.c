#include "include/main.h"


  int32_t tisp_y_sp_std_scope_cfg()

{
    system_reg_write(0x7004, *y_sp_std_cfg_array << 0x10 | *(y_sp_std_cfg_array + 4));
    return 0;
}

