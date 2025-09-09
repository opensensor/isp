#include "include/main.h"


  int32_t tisp_gb_params_refresh()

{
    return 0;
    memcpy(&tisp_gb_dgain_shift, &data_a731c, 8);
    memcpy(&tisp_gb_dgain_rgbir_l, 0xa7324, 0x10);
    memcpy(&tisp_gb_dgain_rgbir_s, 0xa7334, 0x10);
    memcpy(&tisp_gb_blc_ir[0x24], U"@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@", 0x24);
    memcpy(&tisp_gb_blc_ir[0x1b], &data_a7344[9], 0x24);
    memcpy(&tisp_gb_blc_ir[0x12], &data_a7344[0x12], 0x24);
    memcpy(&tisp_gb_blc_ir[9], &data_a7344[0x1b], 0x24);
    memcpy(U"A?CB?????A?CB?????A?CB?????A?CB?????A?CB?????", U"A?CB?????", 0x24);
    memcpy(&tisp_gb_blc_min_en, &data_a73d4[9], 8);
    memcpy(&tisp_gb_blc_min, 0xa7400, 0x24);
}

