#include "include/main.h"


  int32_t dump_vic_reg()

{
    int32_t result;
    
    for (int32_t i = 0; i != 0x1b4; )
    {
        int32_t i_1 = i;
        i += 4;
        result = isp_printf(1, "register is 0x%x, value is 0x%x\\n", i_1);
    }
    
    return result;
}

