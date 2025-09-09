#include "include/main.h"


  int32_t tisp_get_ae_state(char* arg1)

{
    /* tailcall */
    return tisp_ae_state_get(arg1);
}

