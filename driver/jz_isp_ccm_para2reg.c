#include "include/main.h"


  int32_t jz_isp_ccm_para2reg(int32_t* arg1, int32_t* arg2)

{
    int32_t result;
    
    do
    {
        result = *arg2;
        
        if (result < 0)
            result &= 0x3fff;
        
        *arg1 = result;
        arg1 = &arg1[1];
        arg2 = &arg2[1];
    } while (arg1 != &arg1[9]);
    
    return result;
}

