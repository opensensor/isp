#include "include/main.h"


  int32_t tisp_log2_fixed_to_fixed()

{
    int32_t $v0;
    int32_t $t1_1;
    int32_t $t2;
    $v0 = dump_vic_reg();
    return $v0 - ($t1_1 << ($t2 & 0x1f));
}

