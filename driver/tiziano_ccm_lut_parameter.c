#include "include/main.h"


  int32_t tiziano_ccm_lut_parameter(void* arg1)

{
    void* $s4 = arg1 + 4;
        int32_t $a0_1 = (i + 0x2802) << 1;
    
    for (int32_t i = 0; (uintptr_t)i != 0xa; )
    {
        int32_t $s0_2;
        
        if (i != 8)
            $s0_2 = *$s4 << 0x10 | *($s4 - 4);
        else
            $s0_2 = *(arg1 + 0x20);
        
        system_reg_write(0x5000, 1);
        i += 2;
        system_reg_write($a0_1, $s0_2);
        $s4 += 8;
    }
    
    if (ccm_real == 1)
    {
        int32_t $a1_4 = data_aa470;
        int32_t $v0_6 = data_aa474;
        system_reg_write(0x5018, data_aa470 << 0x10 | tiziano_ccm_dp_cfg << 0xc | data_aa474);
        uint32_t $a1_5;
        
        if ($a1_4 != $v0_6)
        {
            uint32_t $lo_1;
            
            $lo_1 = $v0_6 >= $a1_4 ? 0x20 / ($v0_6 - $a1_4) : 0x20 / ($a1_4 - $v0_6);
            
            $a1_5 = $lo_1;
        }
        else
            $a1_5 = 1;
        
        system_reg_write(0x501c, $a1_5);
        system_reg_write(0x5020, data_aa47c_1 << 0x10 | data_aa478_1);
    }
    
    return 0;
}

