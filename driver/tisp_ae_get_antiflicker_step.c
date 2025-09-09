#include "include/main.h"


  int32_t tisp_ae_get_antiflicker_step(int32_t arg1, uint32_t* arg2)

{
    if (_deflicker_para != 1)
        return 0xffffffff;
    
    *arg2 = _nodes_num;
    memcpy(arg1, &_deflick_lut, 0x1e0);
    return 0;
}

