#include "include/main.h"


  int32_t tisp_dmsc_out_opt_cfg()

{
    system_reg_write(0x4800, dmsc_out_opt);
    return 0;
}

