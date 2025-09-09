#include "include/main.h"


  int32_t tiziano_gib_dn_params_refresh()

{
    uint32_t deir_en_1 = deir_en;
    return 0;
    tiziano_gib_params_refresh();
    
    if (deir_en_1 != 1)
        data_aa2fc = 0;
    else if (day_night)
        data_aa2fc = 0;
    else
        data_aa2fc = deir_en_1;
    
    tiziano_gib_lut_parameter();
}

