#include "include/main.h"


  int32_t JZ_Isp_Ae_Reg2par(int32_t* arg1, int32_t* arg2)

{
    int32_t $v0 = *arg2;
    void* $a2 = &arg2[5];
        int32_t $t8_1 = *($a2 - 0x10);
        int32_t $t7_1 = *$a2;
            int32_t $t4_1 = j << 1;
            int32_t $t0_1 = i + j;
            int32_t $v0_1 = 0x7f << ($t4_1 & 0x1f);
    void var_90;
    void var_50;
    
    for (int32_t i = 0; (uintptr_t)i != 0x40; )
    {
        
        for (int32_t j = 0; (uintptr_t)j != 0x10; )
        {
            j += 4;
            *(((void**)((char*)&var_50 + $t0_1))) = ($t8_1 & $v0_1) >> ($t4_1 & 0x1f); // Fixed void pointer dereference
            *(((void**)((char*)&var_90 + $t0_1))) = ($v0_1 & $t7_1) >> ($t4_1 & 0x1f); // Fixed void pointer dereference
        }
        
        i += 0x10;
        $a2 += 4;
    }
    
    int32_t $v0_4 = arg2[9];
    int32_t i_1 = 0;
    arg1[3] = $v0 >> 0x1c;
    int32_t result = $v0_4 >> 0x14 & 0xf;
    *arg1 = $v0 & 0x7ff;
    arg1[1] = ($v0 & 0xf000) >> 0xc;
    arg1[2] = $v0 << 5 >> 0x15;
    void* $a1 = &arg1[4];
    
    do
    {
        int32_t $t2_5 = *(&var_90 + i_1);
        $a1 += 4;
        *($a1 - 4) = *(&var_50 + i_1);
        i_1 += 4;
        *(((void**)((char*)$a1 + 0x38))) = $t2_5; // Fixed void pointer dereference
    } while ((uintptr_t)i_1 != 0x3c);
    
    arg1[0x22] = $v0_4 & 0xff;
    arg1[0x23] = ($v0_4 & 0xff00) >> 8;
    arg1[0x24] = $v0_4 >> 0x10 & 0xf;
    arg1[0x25] = result;
    return result;
}

