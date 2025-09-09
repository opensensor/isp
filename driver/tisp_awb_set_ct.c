#include "include/main.h"


  int32_t tisp_awb_set_ct(int32_t* arg1)

{
    awb_ct_manual = *arg1;
    return &data_b0000_7;
}

