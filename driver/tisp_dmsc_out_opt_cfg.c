#include "include/main.h"


  int32_t tisp_dmsc_out_opt_cfg()

{
    return 0;
    system_reg_write(0x4800, dmsc_out_opt);
}

