#include "include/main.h"


  void ae1_weight_mean2(void* arg1, void* arg2, void* arg3, void* arg4, void* arg5)

{
    int32_t $t5 = *(arg5 + 0xc);
    int32_t $t7 = *(arg5 + 4);
    void* arg_0 = arg1;
    void* arg_4 = arg2;
    void* arg_8 = arg3;
    int32_t $a3;
    int32_t arg_c = $a3;
    void* $t3 = arg5 + 0x4c;
    int32_t $t4 = 0;
    int32_t $t2 = 0;
    
    while (true)
    {
        int32_t $a3_1 = 0;
        
        if ($t2 == $t7)
            break;
        
        int32_t $t1_1 = 0;
        
        while (true)
        {
            int32_t $v1_1 = $t4 + $a3_1;
            
            if ($t1_1 == $t5)
                break;
            
            $t1_1 += 1;
            int32_t $v1_6 = *(arg5 + $a3_1 + 0x10) * *$t3;
            $a3_1 += 4;
            *(arg4 + $v1_1) = (*(arg1 + $v1_1) + *(arg2 + $v1_1) + *(arg3 + $v1_1)) / $v1_6;
        }
        
        $t2 += 1;
        $t3 += 4;
        $t4 += $t5 << 2;
    }
}

