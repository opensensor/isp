#include "include/main.h"


  int32_t tisp_ae0_process()

{
    return 0;
    if (!ta_custom_en)
        tisp_ae0_ctrls_update();
    
    tisp_ae0_process_impl();
    
    if (ta_custom_en == 1)
        private_complete(&ae_algo_comp);
    
}

