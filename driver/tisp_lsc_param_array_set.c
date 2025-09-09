#include "include/main.h"


  int32_t tisp_lsc_param_array_set(int32_t arg1, int32_t arg2, int32_t* arg3)

{
        int32_t var_28_1 = arg1;
        return 0xffffffff;
    if (arg1 - (uintptr_t)0x54 >= 0xb)
    {
        isp_printf(); // Fixed: macro with no parameters, removed 3 arguments;
    }
    
    int32_t* $a0;
    int32_t $s0_1;
    
    switch (arg1)
    {
        case 0x54:
        {
            $a0 = &data_9a428;
            $s0_1 = 4;
            break;
        }
        case 0x55:
        {
            $a0 = &lsc_mesh_scale;
            $s0_1 = 4;
            break;
        }
        case 0x56:
        {
            $a0 = &data_9a424;
            $s0_1 = 4;
            break;
        }
        case 0x57:
        {
            $a0 = &lsc_mesh_size;
            $s0_1 = 8;
            break;
        }
        case 0x58:
        {
            $a0 = &data_9a410;
            $s0_1 = 0x10;
            break;
        }
        case 0x59:
        {
            $a0 = &lsc_a_lut;
            $s0_1 = 0x1ffc;
            break;
        }
        case 0x5a:
        {
            $a0 = &lsc_t_lut;
            $s0_1 = 0x1ffc;
            break;
        }
        case 0x5b:
        {
            $a0 = &lsc_d_lut;
            $s0_1 = 0x1ffc;
            break;
        }
        case 0x5c:
        {
            $a0 = &lsc_mesh_str;
            $s0_1 = 0x24;
            break;
        }
        case 0x5d:
        {
            $a0 = &lsc_mesh_str_wdr;
            $s0_1 = 0x24;
            break;
        }
        case 0x5e:
        {
            $a0 = &lsc_mean_en;
            $s0_1 = 4;
            break;
        }
    }
    
    memcpy($a0, arg2, $s0_1);
    system_reg_write(0x3800, *(lsc_mesh_size + 4) << 0x10 | *lsc_mesh_size);
    system_reg_write(0x3804, data_9a424_1 << 0x10 | lsc_mean_en << 0xf | lsc_mesh_scale);
    data_9a404_2 = 5;
    lsc_last_str = 0;
    data_9a400_4 = 1;
    *arg3 = $s0_1;
    tisp_lsc_write_lut_datas();
    return 0;
}

