#include "include/main.h"


  int32_t tiziano_adr_get_data(void* arg1)

{
    void* $t0 = arg1;
        void* $v0_3 = &adr_block_hist + i * 5;
        int32_t* $v1_1 = $t0;
        void* $a3_1 = $t0;
            void* $t4_7 = &adr_block_y + i + j;
    
    for (int32_t i = 0; (uintptr_t)i != 0x18; )
    {
        
        for (int32_t j = 0; (uintptr_t)j != 0xc0; )
        {
            $v0_3 += 0x78;
            *($v0_3 - 0x78) = *($a3_1 + 8);
            $v1_1 = &$v1_1[4];
            *($v0_3 - 0x74) = *($a3_1 + 0xa);
            $a3_1 += 0x10;
            *($v0_3 - 0x70) = *($v1_1 - 4);
            *($v0_3 - 0x6c) = *($v1_1 - 2);
            *($v0_3 - 0x68) = $v1_1[1];
            j += 0x18;
            *$t4_7 = *$v1_1 & 0xfffffff;
        }
        
        i += 4;
        $t0 += 0x80;
    }
    
    void* $v0_4 = &adr_hist;
    int32_t* i_1 = arg1 + 0x308;
    
    do
    {
        int32_t $a2_1 = *i_1;
    int32_t $v0_5 = *(arg1 + 0x858);
    int32_t result = (*(arg1 + 0x85c) & 0x3ff) << 0xb | $v0_5 >> 0x15;
        i_1 = &i_1[2];
        *$v0_4 = $a2_1 & 0x1fffff;
        $v0_4 += 0xc;
        *($v0_4 - 8) = (*(i_1 - 4) & 0x3ff) << 0xb | *(i_1 - 8) >> 0x15;
        *($v0_4 - 4) = *(i_1 - 4) >> 0xa & 0x1fffff;
    } while (arg1 + 0x858 != i_1);
    
    data_d00d8 = $v0_5 & 0x1fffff;
    data_d00dc = result;
    return result;
}

