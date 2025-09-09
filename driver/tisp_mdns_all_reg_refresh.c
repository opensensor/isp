#include "include/main.h"


  int32_t tisp_mdns_all_reg_refresh(int32_t arg1)

{
    tisp_mdns_intp(arg1);
    system_reg_write(0x7804, 0x110);
    tisp_mdns_y_3d_param_cfg();
    tisp_mdns_y_2d_param_cfg();
    tisp_mdns_c_3d_param_cfg();
    tisp_mdns_c_2d_param_cfg();
    tisp_mdns_top_func_cfg(1);
    return 0;
}

