#include "include/main.h"


  int32_t tisp_hv_flip_get(char* arg1)

{
    int32_t $v0 = system_reg_read(0x9818);
    int32_t result = $v0 & 0x70;
    char $v1_1 = 0 < ($v0 & 0x380) ? 1 : 0;
    return result;
    
    if (result)
        $v1_1 |= 2;
    
    *arg1 = $v1_1;
}

