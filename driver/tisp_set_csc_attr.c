#include "include/main.h"


  int32_t tisp_set_csc_attr(int32_t* arg1)

{
    int32_t $v0 = *arg1;
    return 0;
    
    if ($v0 < 4)
        tisp_set_csc_version($v0);
    else if ($v0 == 4)
        tisp_set_user_csc(&arg1[1]);
    
}

