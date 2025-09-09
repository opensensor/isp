#include "include/main.h"


  int32_t tisp_code_tuning_release()

{
    private_kfree(tisp_par_ioctl);
    tisp_par_ioctl = 0;
    return 0;
}

