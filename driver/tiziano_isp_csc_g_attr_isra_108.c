#include "include/main.h"


  int32_t tiziano_isp_csc_g_attr.isra.108(int32_t* arg1)

{
    uint32_t var_50[0x11];
    tisp_get_csc_attr(&var_50);
    private_copy_to_user(*arg1, &var_50, 0x40);
    return 0;
}

