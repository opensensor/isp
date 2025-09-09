#include "include/main.h"


  int32_t tiziano_lsc_params_refresh()

{
    memcpy(&data_9a428_1, 0x97c00, 4);
    memcpy(&lsc_mesh_scale, 0x97c04, 4);
    memcpy(&data_9a424_1, 0x97c08, 4);
    memcpy(&lsc_mesh_size, 0x97c0c, 8);
    memcpy(&data_9a410_1, 0x97c14, 0x10);
    memcpy(&lsc_a_lut, 0x97c24, 0x1ffc);
    memcpy(&lsc_t_lut, "TISP_PARAM_MDNS_STA_MAX_NUM_ARRAY", 0x1ffc);
    memcpy(&lsc_d_lut, 0x9bc1c, 0x1ffc);
    memcpy(&lsc_mesh_str, 0x9dc18, 0x24);
    memcpy(&lsc_mesh_str_wdr, 0x9dc3c, 0x24);
    memcpy(&lsc_mean_en, 0x9dc60, 4);
    return 0;
}

