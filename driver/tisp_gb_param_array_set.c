#include "include/main.h"


  int32_t tisp_gb_param_array_set(int32_t arg1, int32_t arg2, int32_t* arg3)

{
        int32_t var_10_1 = arg1;
    if (arg1 - (uintptr_t)0x3f5 >= 0xa)
    {
        isp_printf(); // Fixed: macro call, removed arguments;
        return 0xffffffff;
    }
    
    int32_t $v0_1;
    
    switch (arg1)
    {
        case 0x3f5:
        {
            memcpy(&tisp_gb_dgain_shift);
            $v0_1 = 8;
            break;
        }
        case 0x3f6:
        {
            memcpy(&tisp_gb_dgain_rgbir_l);
            $v0_1 = 0x10;
            break;
        }
        case 0x3f7:
        {
            memcpy(&tisp_gb_dgain_rgbir_s);
            $v0_1 = 0x10;
            break;
        }
        case 0x3f8:
        {
            memcpy(&tisp_gb_blc_ir[0x24]);
            $v0_1 = 0x24;
            break;
        }
        case 0x3f9:
        {
            memcpy(&tisp_gb_blc_ir[0x1b]);
            $v0_1 = 0x24;
            break;
        }
        case 0x3fa:
        {
            memcpy(&tisp_gb_blc_ir[0x12]);
            $v0_1 = 0x24;
            break;
        }
        case 0x3fb:
        {
            memcpy(&tisp_gb_blc_ir[9]);
            $v0_1 = 0x24;
            break;
        }
        case 0x3fc:
        {
            memcpy(U"A?CB?????A?CB?????A?CB?????A?CB?????A?CB?????");
            $v0_1 = 0x24;
            break;
        }
        case 0x3fd:
        {
            memcpy(&tisp_gb_blc_min_en);
            $v0_1 = 8;
            break;
        }
        case 0x3fe:
        {
            tisp_reg_map_set(memcpy(&tisp_gb_blc_min, arg2, 0x24));
            $v0_1 = 0x24;
            break;
        }
    }
    
    *arg3 = $v0_1;
    return 0;
}

