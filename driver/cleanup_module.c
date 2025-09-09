#include "include/main.h"


  int32_t cleanup_module()

{
    /* tailcall */
    return tx_isp_exit();
}

