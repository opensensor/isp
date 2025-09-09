#include "include/main.h"


  int32_t tisp_set_bcsh_fixed_contrast(char* arg1)

{
    tisp_bcsh_set_mjpeg_contrast(*arg1, arg1[1], arg1[2]);
    return 0;
}

