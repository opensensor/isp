#include "include/main.h"


  int32_t tisp_awb_algo_init(uint32_t arg1)

{
    tawb_custom_en = arg1;
    return &data_b0000;
}

