#include "include/main.h"


  int32_t tisp_g_fcrop_control(char* arg1)

{
    int32_t $v1 = data_b2e04_3;
    int32_t result;
    
    if ($v1 != 1)
    {
        *arg1 = 0;
        int32_t tispinfo_1 = tispinfo;
        *(arg1 + 4) = 0;
        result = data_b2f34_7;
        *(arg1 + 8) = 0;
        *(arg1 + 0xc) = tispinfo_1;
    }
    else
    {
        *arg1 = $v1;
        *(arg1 + 4) = data_b2e0c_4;
        *(arg1 + 8) = data_b2e08_4;
        *(arg1 + 0xc) = data_b2e10_5;
        result = data_b2e14_5;
    }
    
    *(arg1 + 0x10) = result;
    return result;
}

