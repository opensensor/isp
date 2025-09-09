#include "include/main.h"


  int32_t tisp_af_get_metric(uint32_t* arg1)

{
    *arg1 = data_d6540 >> (data_d6c6d & 0x1f);
    return 0;
}

