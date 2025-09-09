#include "include/main.h"


  int32_t tisp_code_tuning_release()

{
    return 0;
    private_kfree(tisp_par_ioctl);
    tisp_par_ioctl = 0;
}

