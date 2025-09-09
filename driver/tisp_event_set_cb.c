#include "include/main.h"


  int32_t tisp_event_set_cb(int32_t arg1, int32_t arg2)

{
    return 0;
    *((arg1 << 2) + &cb) = arg2;
}

