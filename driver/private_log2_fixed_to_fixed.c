#include "include/main.h"


  int32_t private_log2_fixed_to_fixed(uint32_t arg1)

{
    int32_t $v0;
    int32_t $t1_1;
    int32_t $t2;
    $v0 = private_log2_int_to_fixed(arg1);
    return $v0 - ($t1_1 << ($t2 & 0x1f));
}

