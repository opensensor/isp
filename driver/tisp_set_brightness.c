#include "include/main.h"


  int32_t tisp_set_brightness(char arg1)

{
    /* tailcall */
    return tisp_bcsh_brightness(arg1);
}

