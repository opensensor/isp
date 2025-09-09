#include "include/main.h"


  int32_t tisp_gib_deir_ir_update(int32_t arg1)

{
    int32_t gib_ir_mode_1 = *gib_ir_mode;
    int32_t $t0 = *(gib_ir_mode + 4);
    int32_t $v1 = data_aa2fc_5;
    *gib_ir_value = arg1;
    
    if ($v1 == 1 && gib_ir_mode_1 == $v1)
    {
        int32_t $a1_1 = *(gib_ir_value + 4);
        int32_t $v0_1 = arg1 - $a1_1;
        
        if ($a1_1 >= arg1)
            $v0_1 = $a1_1 - arg1;
        
        if ($t0 < $v0_1 || trig_set_deir == gib_ir_mode_1)
        {
            trig_set_deir = 0;
            tiziano_gib_deir_ir_interpolation(arg1);
            *(gib_ir_value + 4) = *gib_ir_value;
        }
    }
    
    return 0;
}

