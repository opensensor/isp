#include "include/main.h"


  int32_t tisp_hldc_con_par_cfg()

{
    return 0;
    system_reg_write(0x9000, data_d043c << 0x10 | data_d0438);
    system_reg_write(0x9004, data_d0444 << 0x10 | data_d0440);
    system_reg_write(0x9008, data_d0460 << 0x10 | data_d045c);
    system_reg_write(0x900c, data_d0448);
    system_reg_write(0x9010, data_d0464);
    system_reg_write(0x9014, data_d044c);
    system_reg_write(0x9018, data_d046c);
    system_reg_write(0x901c, data_d0454 << 0x10 | data_d0450);
    system_reg_write(0x9020, data_d0470 << 0x10 | data_d0468);
    system_reg_write(0x9024, data_d0458);
    system_reg_write(0x9028, data_d0474);
}

