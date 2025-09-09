#include "include/main.h"


  int32_t sensor_early_init(uint32_t arg1)

{
    int32_t result = 0xffffffea;
    
    if (arg1)
    {
        result = 0;
        
        if (!g_ispcore)
            g_ispcore = arg1;
    }
    
    return result;
}

