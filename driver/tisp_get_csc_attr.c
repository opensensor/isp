#include "include/main.h"


  int32_t tisp_get_csc_attr(uint32_t* arg1)

{
    tisp_get_current_csc(arg1, &arg1[1]);
    return 0;
}

