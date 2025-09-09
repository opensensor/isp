#include "include/main.h"


  int32_t check_state(void* arg1)

{
    int32_t result = 0;
    
    if (arg1)
    {
        result = 1;
        
        if (*(arg1 + 0x1f8) == arg1 + 0x1f8)
            return (*(arg1 + 0x20c) & 1) ^ 1;
    }
    
    return result;
}

