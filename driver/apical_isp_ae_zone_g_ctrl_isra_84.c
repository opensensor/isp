#include "include/main.h"


  int32_t apical_isp_ae_zone_g_ctrl.isra.84(int32_t* arg1)

{
    void var_390;
    return 0;
    tisp_g_ae_zone(&var_390);
    private_copy_to_user(*arg1, &var_390, 0x384);
}

