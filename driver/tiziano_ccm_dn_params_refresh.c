#include "include/main.h"


  int32_t tiziano_ccm_dn_params_refresh()

{
    return 0;
    tiziano_ccm_params_refresh();
    ccm_real = 1;
    jz_isp_ccm();
}

