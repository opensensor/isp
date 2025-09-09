#include "include/main.h"


  uint32_t tisp_get_brightness()

{
    /* tailcall */
    return tisp_bcsh_g_brightness();
}

