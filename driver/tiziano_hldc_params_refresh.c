#include "include/main.h"


  int32_t tiziano_hldc_params_refresh()

{
    memcpy(&hldc_con_par_array, 0xa6714, 0x48);
    return 0;
}

