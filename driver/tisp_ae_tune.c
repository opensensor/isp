#include "include/main.h"


  int32_t tisp_ae_tune(int32_t* arg1, int32_t* arg2, int32_t* arg3, int32_t arg4, int32_t arg5, int32_t arg6)

{
    int32_t $s1 = *arg2;
    int32_t $v0 = *arg1;
    int32_t $s5 = arg1[1];
    int32_t $a1_2 = *arg3;
    int32_t $s7 = arg4 << (arg5 & 0x1f);
    int32_t $s3 = 0x80 << (arg5 & 0x1f);
    int32_t result = *arg3 + fix_point_div_32(arg5, fix_point_mult3_32(arg5, $s7, $s5), $s3);
    
    if (arg6 < $v0 + $s1)
        $v0 = arg6 - $s1;
    
    
    if (arg6 < $s5 + $a1_2)
        $s5 = arg6 - $a1_2;
    
    *arg2 = $s1 + fix_point_div_32(arg5, fix_point_mult3_32(arg5, $s7, $v0), $s3);
    *arg3 = result;
    return result;
}

