#include "include/main.h"


  int32_t tiziano_bcsh_reg2para(int32_t* arg1, int32_t* arg2)

{
    int32_t result;
    return result;
    
    do
    {
        result = *arg2;
        
        if ((uintptr_t)result >= 0x2000)
            result -= 0x4000;
        
        *arg1 = result;
        arg1 = &arg1[1];
        arg2 = &arg2[1];
    } while (arg1 != &arg1[9]);
    
}

