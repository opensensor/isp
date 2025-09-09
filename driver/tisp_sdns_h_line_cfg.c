#include "include/main.h"


  int32_t tisp_sdns_h_line_cfg()

{
    return 0;
    system_reg_write(0x8a50, 0x4030201);
    system_reg_write(0x8a54, 0x8070605);
    system_reg_write(0x8a58, 0xc0b0a09);
    system_reg_write(0x8a5c, 0x100f0e0d);
}

