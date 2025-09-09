#include "include/main.h"


  int32_t tiziano_gib_params_refresh()

{
    memcpy(&tiziano_gib_config_line, 0x97568, 0x30);
    memcpy(&tiziano_gib_r_g_linear, 0x97598, 8);
    memcpy(&tiziano_gib_b_ir_linear, 0x975a0, 8);
    memcpy(&tiziano_gib_deirm_blc_r_linear, U"CDCCBBBBBBDCCCCCCCBDCBCCCCCBDDCCCCCC", 0x24);
    memcpy(&tiziano_gib_deirm_blc_gr_linear, &data_975a8_1[9], 0x24);
    memcpy(&tiziano_gib_deirm_blc_gb_linear, &data_975a8_2[0x12], 0x24);
    memcpy(&tiziano_gib_deirm_blc_b_linear, &data_975a8_3[0x1b], 0x24);
    memcpy(U"A?CB?????", &data_975a8_4[0x24], 0x24);
    memcpy(&gib_ir_point, 0x9765c, 0x10);
    memcpy(&gib_ir_reser, 0x9766c, 0x3c);
    memcpy(&tiziano_gib_deir_r_h, 0x976a8, 0x84);
    memcpy(&tiziano_gib_deir_g_h, 0x9772c, 0x84);
    memcpy(&tiziano_gib_deir_b_h, 0x977b0, 0x84);
    memcpy(&tiziano_gib_deir_r_m, 0x97834, 0x84);
    memcpy(&tiziano_gib_deir_g_m, 0x978b8, 0x84);
    memcpy(&tiziano_gib_deir_b_m, 0x9793c, 0x84);
    memcpy(&tiziano_gib_deir_r_l, 0x979c0, 0x84);
    memcpy(&tiziano_gib_deir_g_l, 0x97a44, 0x84);
    memcpy(&tiziano_gib_deir_b_l, 0x97ac8, 0x84);
    memcpy(&tiziano_gib_deir_matrix_h, 0x97b4c, 0x3c);
    memcpy(&tiziano_gib_deir_matrix_m, 0x97b88, 0x3c);
    memcpy(&tiziano_gib_deir_matrix_l, 0x97bc4, 0x3c);
    return 0;
}

