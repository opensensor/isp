#include "include/main.h"


  int32_t tisp_gb_param_array_get(int32_t arg1, int32_t arg2, int32_t* arg3)

{
        int32_t var_18_1 = arg1;
        return 0xffffffff;
    if (arg1 - (uintptr_t)0x3f5 >= 0xa)
    {

    }
    
    int32_t* $a1_1;
    int32_t $s0_1;
    
    switch (arg1)
    {
        case 0x3f5:
        {
            $a1_1 = &tisp_gb_dgain_shift;
            $s0_1 = 8;
            break;
        }
        case 0x3f6:
        {
            $a1_1 = &tisp_gb_dgain_rgbir_l;
            $s0_1 = 0x10;
            break;
        }
        case 0x3f7:
        {
            $a1_1 = &tisp_gb_dgain_rgbir_s;
            $s0_1 = 0x10;
            break;
        }
        case 0x3f8:
        {
            $a1_1 = &tisp_gb_blc_ir[0x24];
            $s0_1 = 0x24;
            break;
        }
        case 0x3f9:
        {
            $a1_1 = &tisp_gb_blc_ir[0x1b];
            $s0_1 = 0x24;
            break;
        }
        case 0x3fa:
        {
            $a1_1 = &tisp_gb_blc_ir[0x12];
            $s0_1 = 0x24;
            break;
        }
        case 0x3fb:
        {
            $a1_1 = &tisp_gb_blc_ir[9];
            $s0_1 = 0x24;
            break;
        }
        case 0x3fc:
        {
            $a1_1 = U"A?CB?????A?CB?????A?CB?????A?CB?????A?CB?????";
            $s0_1 = 0x24;
            break;
        }
        case 0x3fd:
        {
            $a1_1 = &tisp_gb_blc_min_en;
            $s0_1 = 8;
            break;
        }
        case 0x3fe:
        {
            $a1_1 = &tisp_gb_blc_min;
            $s0_1 = 0x24;
            break;
        }
    }
    
    memcpy(arg2, $a1_1, $s0_1);
    *arg3 = $s0_1;
    return 0;
}

