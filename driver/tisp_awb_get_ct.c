#include "include/main.h"


  uint32_t tisp_awb_get_ct(uint32_t* arg1)

{
    uint32_t _awb_ct_1 = _awb_ct;
    *arg1 = _awb_ct_1;
    return _awb_ct_1;
}

