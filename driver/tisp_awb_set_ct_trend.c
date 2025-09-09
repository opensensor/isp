#include "include/main.h"


  int32_t tisp_awb_set_ct_trend(int32_t arg1)

{
    int32_t arg_0 = arg1;
    int32_t arg_4 = $a1;
    int32_t arg_8 = $a2;
    int32_t arg_c = $a3;
    int32_t* i = &arg_0;
    int32_t* $v0 = &_awb_trend;
        int32_t $a1_1 = *i;
    int32_t $a1;
    int32_t $a2;
    int32_t $a3;
    void arg_18;
    
    do
    {
        i = &i[1];
        $v0[1] = $a1_1;
        $v0 = &$v0[1];
    } while (i != &arg_18);
    
    awb_moa = 1;
    return 0;
}

