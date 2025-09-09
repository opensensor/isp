#include "include/main.h"


  int32_t exception_handle()

{
    int32_t $v1 = system_reg_write(0x24, system_reg_read(0x24) | 1)(0xe0000, 4);
    int32_t $v0_1;
    
    do
        $v0_1 = $v1(0x28);
     while (!($v0_1 & 1));
    int32_t $v0_3;
    int32_t $t9_1;
    $v0_3 = $v1(0x20);
    int32_t $v1_1;
    int32_t $t9_2;
    $v1_1 = $t9_1(0x20, $v0_3 | 4);
    /* tailcall */
    return $t9_2(0x20, $v1_1 & 0xfffffffb)(0x800, 1)(0xe0000, 1);
}

