#include "include/main.h"


  int32_t JZ_Isp_Awb_Reg2par(int32_t* arg1, int32_t* arg2)

{
    int32_t $v0 = *arg2;
    char* $a2 = (char*)(&arg2[5]); // Fixed void pointer assignment
    void var_90;
    void var_50;
        int32_t $t8_1 = *($a2 - 0x10);
        int32_t $t7_1 = *$a2;
            int32_t $t5_1 = j << 1;
            int32_t $t0_1 = i + j;
            int32_t $v0_1 = 0x7f << ($t5_1 & 0x1f);
    
    for (int32_t i = 0; (uintptr_t)i != 0x40; )
    {
        
        for (int32_t j = 0; (uintptr_t)j != 0x10; )
        {
            j += 4;
            *(&var_50 + $t0_1) = ($t8_1 & $v0_1) >> ($t5_1 & 0x1f);
            *(&var_90 + $t0_1) = ($v0_1 & $t7_1) >> ($t5_1 & 0x1f);
        }
        
        i += 0x10;
        $a2 += 4;
    }
    
    int32_t $v0_4 = arg2[0xa];
    int32_t $v1_1 = arg2[9];
    int32_t $v0_6 = arg2[0xb];
    int32_t $v0_7 = arg2[0xc];
    int32_t i_1 = 0;
    int32_t $v0_8 = arg2[0xd];
    arg1[3] = $v0 >> 0x1c;
    int32_t result = $v0_8 >> 0x10 & 0xf;
    *arg1 = $v0 & 0x7ff;
    arg1[1] = ($v0 & 0xf000) >> 0xc;
    arg1[2] = $v0 << 5 >> 0x15;
    char* $a1 = (char*)(&arg1[4]); // Fixed void pointer assignment
    
    do
    {
        int32_t $t2_5 = *(&var_90 + i_1);
    return result;
        $a1 += 4;
        *($a1 - 4) = *(&var_50 + i_1);
        i_1 += 4;
        *((int32_t*)((char*)$a1 + 0x38)) = $t2_5; // Fixed void pointer dereference
    } while ((uintptr_t)i_1 != 0x3c);
    
    arg1[0x26] = $v0_6 & 0xffff;
    arg1[0x28] = $v0_7 & 0xffff;
    arg1[0x22] = $v1_1 & 0xfff;
    arg1[0x23] = ($v1_1 & 0xfff0000) >> 0x10;
    arg1[0x24] = $v0_4 & 0xfff;
    arg1[0x25] = ($v0_4 & 0xfff0000) >> 0x10;
    arg1[0x27] = $v0_6 >> 0x10;
    arg1[0x29] = $v0_7 >> 0x10;
    arg1[0x2a] = $v0_8 & 0xff;
    arg1[0x2b] = ($v0_8 & 0xff00) >> 8;
    arg1[0x2c] = result;
}

