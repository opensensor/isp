#include "include/main.h"


  int32_t tiziano_gib_deir_reg(void* arg1, void* arg2, void* arg3)

{
    void* const i = &data_80000;
    int32_t* $s3 = arg1 + 4;
    int32_t* $s2 = arg2 + 4;
    int32_t* $s1 = arg3 + 4;
        void* $a0_2 = i + 0x100;
    
    do
    {
        system_reg_write(i, *$s3 << 0xc | *($s3 - 4));
        system_reg_write(i + 0x80, *$s2 << 0xc | *($s2 - 4));
        i += 4;
        system_reg_write($a0_2, *$s1 << 0xc | *($s1 - 4));
        $s3 = &$s3[1];
        $s2 = &$s2[1];
        $s1 = &$s1[1];
    } while (i != ".%d)\n");
    
    return 0;
}

