#include "include/main.h"


  int32_t tisp_awb_deinit()

{
    if (tawb_custom_en == 1)
        tawb_custom_en = 0;
    
    return 0;
}

