#include "include/main.h"


  int32_t isp_tunning_poll(int32_t arg1, int32_t* arg2)

{
        int32_t $v0_1 = *arg2;
    if (arg2)
    {
        
        if ($v0_1)
            $v0_1(arg1, &dumpQueue, arg2);
    }
    
    return 0 < *tispPollValue ? 1 : 0;
}

