#include "include/main.h"


  int32_t* pop_buffer_fifo(int32_t* arg1)

{
    int32_t* result = *arg1;
    
    if (arg1 == result)
        return 0;
    
    void** $v1 = result[1];
    void* $a0 = *result;
    *($a0 + 4) = $v1;
    *$v1 = $a0;
    *result = 0x100100;
    result[1] = 0x200200;
    return result;
}

