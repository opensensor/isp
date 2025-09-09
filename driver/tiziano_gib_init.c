#include "include/main.h"


  int32_t tiziano_gib_init()

{
    tiziano_gib_params_refresh();
    uint32_t deir_en_1 = deir_en;
    
    if (deir_en_1 != 1)
        data_aa2fc_6 = 0;
    else if (day_night)
        data_aa2fc_7 = 0;
    else
        data_aa2fc_8 = deir_en_1;
    
    tiziano_gib_lut_parameter();
    tiziano_gib_deir_reg(&tiziano_gib_deir_r_m, &tiziano_gib_deir_g_m, &tiziano_gib_deir_b_m);
    return 0;
}

