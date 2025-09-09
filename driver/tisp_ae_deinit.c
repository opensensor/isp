#include "include/main.h"


  int32_t tisp_ae_deinit()

{
    if (ta_custom_en == 1)
        ta_custom_en = 0;
    
    return &data_d0000_3;
}

