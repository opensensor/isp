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
    void* arg_0 = arg1;
    void* arg_4 = arg2;
    void* arg_8 = arg3;
    void* arg_c = arg4;
    void* const var_60_16 = &data_20000_16;
    int32_t $t0 = 0;
    int32_t $s0 = 0;
    int32_t $s1 = 0;
    int32_t i = 0;
    void* var_5c_9 = &fv_value;
    
    for (; i != $v1; i += 1)
    {
        for (int32_t j = 0; j != $v1_1; j += 1)
        {
            int32_t $a3_1 = i * 0x3c + (j << 2);
            int32_t $t8_1 = *(arg5 + $a3_1);
            int32_t $s3_1 = *(arg3 + $a3_1);
            int32_t $s4_2 = (var_60_17 + 0xfd4)($s2, *(arg4 + $a3_1) << ($s2 & 0x1f), $t5)
                + (var_60_18 + 0xfd4)($s2, *(arg2 + $a3_1) << ($s2 & 0x1f), $v0_1 - $t5);
            int32_t $s3_2 = (var_60_19 + 0xfd4)($s2, $t8_1 << ($s2 & 0x1f), $fp)
                + (var_60_20 + 0xfd4)($s2, $s3_1 << ($s2 & 0x1f), $v0_1 - $fp);
            *(var_5c_10 + $a3_1) = ((var_60_21 + 0xfd4)($s2, $s4_2 << ($s2 & 0x1f), $s7)
                + (var_60_22 + 0xfd4)($s2, $s3_2 << ($s2 & 0x1f), $s6)) >> ($s2 & 0x1f);
            int32_t $v0_23 = *(arg6 + $a3_1);
            $s1 += ($s4_2 >> ($s2 & 0x1f)) * $v0_23;
            $s0 += ($s3_2 >> ($s2 & 0x1f)) * $v0_23;
            $t0 += $v0_23;
        }
    }
    
    int32_t var_88_17;
    __private_spin_lock_irqsave(0, &var_88_18);
    memcpy(&fv_value_last, &fv_value, 0x384);
    private_spin_unlock_irqrestore(0, var_88_19);
    uint32_t $lo = $s1 / $t0;
    uint32_t $lo_1 = $s0 / $t0;
    uint32_t $s5_3 = (fix_point_mult2_32($s2, $lo << ($s2 & 0x1f), $s7)
        + fix_point_mult2_32($s2, $lo_1 << ($s2 & 0x1f), $s6)) >> ($s2 & 0x1f);
    uint32_t $v1_8 = (fix_point_mult2_32($s2, $s1 >> 3 << ($s2 & 0x1f), $s7)
        + fix_point_mult2_32($s2, $s0 >> 3 << ($s2 & 0x1f), $s6)) >> ($s2 & 0x1f);
    void* i_1 = arg8 + 4;
    
    do
    {
        int32_t $a1_15 = *i_1;
        i_1 += 4;
        *(i_1 - 8) = $a1_15;
    } while (arg8 + 0x3c != i_1);
    
    *(arg8 + 0x38) = $s5_3;
    int32_t $s2_1 = $v1 * $v1_1;
    *arg7 = $lo * $s2_1;
    arg7[1] = $lo_1 * $s2_1;
    arg7[2] = $s5_3 * $s2_1;
    AFParam_Fv_Alt = $v1_8;
    return &data_d0000_5;
}

