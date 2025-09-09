#include "include/main.h"


  void isp_core_tuning_deinit(int32_t arg1)

{
    if (!arg1)
        return;
    
    /* tailcall */
    return private_kfree();
}

