#include "include/main.h"


  int32_t tisp_code_tuning_open()

{
    uint32_t $v0 = private_kmalloc(0x500c, 0xd0);
    tisp_par_ioctl = $v0;
    memset($v0, 0, 0x500c);
    return 0;
}

