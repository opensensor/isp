#include "include/main.h"


  int32_t tisp_set_user_csc(int32_t arg1)

{
    memcpy(0x9ab30, arg1, 0x3c);
    /* tailcall */
    return tisp_set_csc_version(4);
}

