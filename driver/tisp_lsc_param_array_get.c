#include "include/main.h"


  int32_t tisp_lsc_param_array_get(int32_t arg1, int32_t arg2, int32_t* arg3)

{
        int32_t var_18_1 = arg1;
        return 0xffffffff;
    if (arg1 - (uintptr_t)0x54 >= 0xb)
    {

    }
    
    int32_t* $a1_1;
    int32_t $s0_1;
    
    switch (arg1)
    {
        case 0x54:
        {
            $a1_1 = &data_9a428;
            $s0_1 = 4;
            break;
        }
        case 0x55:
        {
            $a1_1 = &lsc_mesh_scale;
            $s0_1 = 4;
            break;
        }
        case 0x56:
        {
            $a1_1 = &data_9a424;
            $s0_1 = 4;
            break;
        }
        case 0x57:
        {
            $a1_1 = &lsc_mesh_size;
            $s0_1 = 8;
            break;
        }
        case 0x58:
        {
            $a1_1 = &data_9a410;
            $s0_1 = 0x10;
            break;
        }
        case 0x59:
        {
            $a1_1 = &lsc_a_lut;
            $s0_1 = 0x1ffc;
            break;
        }
        case 0x5a:
        {
            $a1_1 = &lsc_t_lut;
            $s0_1 = 0x1ffc;
            break;
        }
        case 0x5b:
        {
            $a1_1 = &lsc_d_lut;
            $s0_1 = 0x1ffc;
            break;
        }
        case 0x5c:
        {
            $a1_1 = &lsc_mesh_str;
            $s0_1 = 0x24;
            break;
        }
        case 0x5d:
        {
            $a1_1 = &lsc_mesh_str_wdr;
            $s0_1 = 0x24;
            break;
        }
        case 0x5e:
        {
            $a1_1 = &lsc_mean_en;
            $s0_1 = 4;
            break;
        }
    }
    
    memcpy(arg2, $a1_1, $s0_1);
    *arg3 = $s0_1;
    return 0;
}

