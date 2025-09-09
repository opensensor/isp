#include "include/main.h"


  int32_t tisp_hldc_con_par_cfg()

{
    system_reg_write(0x9000, data_d043c_1 << 0x10 | data_d0438_1);
    system_reg_write(0x9004, data_d0444_1 << 0x10 | data_d0440_1);
    system_reg_write(0x9008, data_d0460_1 << 0x10 | data_d045c_1);
    system_reg_write(0x900c, data_d0448_1);
    system_reg_write(0x9010, data_d0464_1);
    system_reg_write(0x9014, data_d044c_1);
    system_reg_write(0x9018, data_d046c_1);
    system_reg_write(0x901c, data_d0454_1 << 0x10 | data_d0450_1);
    system_reg_write(0x9020, data_d0470_1 << 0x10 | data_d0468_1);
    system_reg_write(0x9024, data_d0458_1);
    system_reg_write(0x9028, data_d0474_1);
    return 0;
}

