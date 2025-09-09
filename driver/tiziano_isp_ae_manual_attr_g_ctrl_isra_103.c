#include "include/main.h"


  int32_t tiziano_isp_ae_manual_attr_g_ctrl.isra.103(int32_t* arg1)

{
    void var_a0_16;
    tisp_get_ae_attr(&var_a0_17);
    private_copy_to_user(*arg1, &var_a0_18, 0x98);
    return 0;
}

