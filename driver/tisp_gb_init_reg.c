#include "include/main.h"


  int32_t tisp_gb_init_reg()

{
    system_reg_write_gb(1, 0x1008, *(tisp_gb_dgain_shift + 4) << 2 | *tisp_gb_dgain_shift);
    
    if (!init.31768)
    {
        system_reg_write_gb(1, 0x1000, data_aa3e8 << 0x10 | tisp_gb_dgain_rgbir_l);
        system_reg_write_gb(1, 0x1004, data_aa3f0 << 0x10 | data_aa3ec);
        system_reg_write_gb(1, 0x100c, data_aa3d8 << 0x10 | tisp_gb_dgain_rgbir_s);
        system_reg_write_gb(1, 0x1010, data_aa3e0 << 0x10 | data_aa3dc);
    }
    
    tisp_gb_blc_again_interp(*tisp_gb_blc_ag, 0);
    tisp_gb_blc_again_interp(*(tisp_gb_blc_ag + 4), 1);
    init.31768 = 1;
    return 0;
}

