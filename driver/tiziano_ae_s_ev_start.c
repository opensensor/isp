#include "include/main.h"


  int32_t tiziano_ae_s_ev_start(uint32_t arg1)

{
    ae_ev_init_strict = arg1;
    ae_ev_init_en = 1;
    return &data_d0000;
}

