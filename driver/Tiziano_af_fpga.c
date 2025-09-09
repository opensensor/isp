#include "include/main.h"


  int32_t Tiziano_af_fpga(void* arg1, void* arg2, void* arg3, void* arg4, void* arg5, void* arg6, int32_t* arg7, void* arg8, int32_t* arg9)

{
    int32_t $v1 = *(arg1 + 4);
    int32_t $v1_1 = *(arg1 + 0xc);
    int32_t $s2 = arg9[4];
    int32_t $t5 = *arg9;
    int32_t $fp = arg9[1];
    int32_t $s7 = arg9[2];
    int32_t $s6 = arg9[3];
    int32_t $v0_1 = 1 << ($s2 & 0x1f);
    char* arg_0 = (char*)(arg1); // Fixed void pointer assignment
    char* arg_4 = (char*)(arg2); // Fixed void pointer assignment
    char* arg_8 = (char*)(arg3); // Fixed void pointer assignment
    char* arg_c = (char*)(arg4); // Fixed void pointer assignment
    int32_t $t0 = 0;
    int32_t $s0 = 0;
    int32_t $s1 = 0;
    int32_t i = 0;
    char* var_5c = (char*)(&fv_value); // Fixed void pointer assignment
            int32_t $a3_1 = i * 0x3c + (j << 2);
            int32_t $t8_1 = *(arg5 + $a3_1);
            int32_t $s3_1 = *(arg3 + $a3_1);
            int32_t $s4_2 = (var_60 + 0xfd4)($s2, *(arg4 + $a3_1) << ($s2 & 0x1f), $t5)
            int32_t $s3_2 = (var_60 + 0xfd4)($s2, $t8_1 << ($s2 & 0x1f), $fp)
            int32_t $v0_23 = *(arg6 + $a3_1);
    void* void* var_60 = (void*)&data_20000; // Fixed function pointer assignment
    
    for (; i != $v1; i += 1)
    {
        for (int32_t j = 0; j != $v1_1; j += 1)
        {
                + (var_60 + 0xfd4)($s2, *(arg2 + $a3_1) << ($s2 & 0x1f), $v0_1 - $t5);
                + (var_60 + 0xfd4)($s2, $s3_1 << ($s2 & 0x1f), $v0_1 - $fp);
            *(var_5c + $a3_1) = ((var_60 + 0xfd4)($s2, $s4_2 << ($s2 & 0x1f), $s7)
                + (var_60 + 0xfd4)($s2, $s3_2 << ($s2 & 0x1f), $s6)) >> ($s2 & 0x1f);
            $s1 += ($s4_2 >> ($s2 & 0x1f)) * $v0_23;
            $s0 += ($s3_2 >> ($s2 & 0x1f)) * $v0_23;
            $t0 += $v0_23;
        }
    }
    
    int32_t var_88_8;
    __private_spin_lock_irqsave(0, &var_88_9);
    memcpy(&fv_value_last, &fv_value, 0x384);
    private_spin_unlock_irqrestore(0, var_88_10);
    uint32_t $lo = $s1 / $t0;
    uint32_t $lo_1 = $s0 / $t0;
    uint32_t $s5_3 = (fix_point_mult2_32($s2, $lo << ($s2 & 0x1f), $s7)
        + fix_point_mult2_32($s2, $lo_1 << ($s2 & 0x1f), $s6)) >> ($s2 & 0x1f);
    uint32_t $v1_8 = (fix_point_mult2_32($s2, $s1 >> 3 << ($s2 & 0x1f), $s7)
        + fix_point_mult2_32($s2, $s0 >> 3 << ($s2 & 0x1f), $s6)) >> ($s2 & 0x1f);
    char* i_1 = (char*)(arg8 + 4); // Fixed void pointer assignment
    
    do
    {
        int32_t $a1_15 = *i_1;
    int32_t $s2_1 = $v1 * $v1_1;
        i_1 += 4;
        *(i_1 - 8) = $a1_15;
    } while (arg8 + 0x3c != i_1);
    
    *((int32_t*)((char*)arg8 + 0x38)) = $s5_3; // Fixed void pointer dereference
    *arg7 = $lo * $s2_1;
    arg7[1] = $lo_1 * $s2_1;
    arg7[2] = $s5_3 * $s2_1;
    AFParam_Fv_Alt = $v1_8;
    return &data_d0000;
}

