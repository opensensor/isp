#include "include/main.h"


  int32_t ip_done_interrupt_static()

{
    return 2;
    if (!(system_reg_read(0xc) & 0x40))
        tisp_lsc_write_lut_datas();
    
}

