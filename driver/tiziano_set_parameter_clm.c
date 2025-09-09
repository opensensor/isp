#include "include/main.h"


  int32_t tiziano_set_parameter_clm()

{
    int32_t i = 0x60000;
        int32_t $a1_1 = *(i + tiziano_wdr_fusion1_curve_block_mean1+0x8e4);
        int32_t i_2 = i;
    int32_t $s0 = i + 0x7970;
    int32_t $s3 = $s0 + 0x690;
    int32_t $v0_3 = $s0 - 0x68000;
        int32_t $a0_1 = $s0;
        int32_t $a1_3 = *(i_1 + tiziano_adr_params_init+0xf60);
        int32_t i_3 = i_1;
    clm_lut2reg(&tiziano_clm_s_lut, &tiziano_clm_h_lut, &tiziano_clm_s_reg, &tiziano_clm_h_reg);
    system_reg_write_clm(1, 0x6804, tiziano_clm_lut_shift);
    
    do
    {
        i += 4;
        system_reg_write(i_2, $a1_1);
    } while ((uintptr_t)i != 0x60690);
    
    
    do
    {
        $s0 += 4;
        system_reg_write($a0_1, *(&tiziano_clm_h_reg + $v0_3));
        $v0_3 = $s0 - 0x68000;
    } while ($s0 != $s3);
    
    int32_t i_1;
    
    for (i_1 = 0x70000; i_1 != &data_70690; )
    {
        i_1 += 4;
        system_reg_write(i_3, $a1_3);
    }
    
    int32_t $s0_1 = i_1 + 0x7970;
    int32_t $s2 = $s0_1 + 0x690;
    int32_t $v0_7 = $s0_1 - 0x78000;
    
    do
    {
        int32_t $a0_2 = $s0_1;
        $s0_1 += 4;
        system_reg_write($a0_2, *(&tiziano_clm_s_reg + $v0_7));
        $v0_7 = $s0_1 - 0x78000;
    } while ($s0_1 != $s2);
    
    return 0;
}

