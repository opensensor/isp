#include "include/main.h"


  int32_t system_reg_read(int32_t arg1)

{
    return *(*(mdns_y_pspa_cur_bi_wei0_array + 0xb8) + arg1);
}

