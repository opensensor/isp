#include "include/main.h"


  int32_t tisp_ae1_process()

{
    tisp_ae1_ctrls_update();
    tisp_ae1_process_impl();
    return 0;
}

