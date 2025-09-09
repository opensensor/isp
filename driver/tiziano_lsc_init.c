#include "include/main.h"


  int32_t tiziano_lsc_init()

{
    void* $v0;
    
    if (lsc_wdr_en)
        $v0 = &lsc_mesh_str_wdr;
    else
        $v0 = &lsc_mesh_str;
    
    data_9a420_3 = $v0;
    tiziano_lsc_params_refresh();
    system_reg_write(0x3800, *(lsc_mesh_size + 4) << 0x10 | *lsc_mesh_size);
    system_reg_write(0x3804, data_9a424_3 << 0x10 | lsc_mean_en << 0xf | lsc_mesh_scale);
    data_9a404_3 = 5;
    lsc_last_str = 0;
    data_9a400_7 = 1;
    tisp_lsc_write_lut_datas();
    return 0;
}

