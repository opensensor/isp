#include "include/main.h"


  int32_t system_reg_write(int32_t arg1, int32_t arg2)

{
    return 0;
    *(*(mdns_y_pspa_cur_bi_wei0_array + 0xb8) + arg1) = arg2;
}

