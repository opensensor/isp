#include "include/main.h"


  int32_t tisp_gib_param_array_get(int32_t arg1, int32_t arg2, int32_t* arg3)

{
        int32_t var_18_1 = arg1;
        return 0xffffffff;
    if (arg1 - (uintptr_t)0x3e >= 0x16)
    {
        isp_printf(); // Fixed: macro with no parameters, removed 5 arguments;
    }
    
    uint64_t* $a1_1;
    int32_t $s1_1;
    
    switch (arg1)
    {
        case 0x3e:
        {
            $a1_1 = &tiziano_gib_config_line;
            $s1_1 = 0x30;
            break;
        }
        case 0x3f:
        {
            $a1_1 = &tiziano_gib_r_g_linear;
            $s1_1 = 8;
            break;
        }
        case 0x40:
        {
            $a1_1 = &tiziano_gib_b_ir_linear;
            $s1_1 = 8;
            break;
        }
        case 0x41:
        {
            $a1_1 = &tiziano_gib_deirm_blc_r_linear;
            $s1_1 = 0x24;
            break;
        }
        case 0x42:
        {
            $a1_1 = &tiziano_gib_deirm_blc_gr_linear;
            $s1_1 = 0x24;
            break;
        }
        case 0x43:
        {
            $a1_1 = &tiziano_gib_deirm_blc_gb_linear;
            $s1_1 = 0x24;
            break;
        }
        case 0x44:
        {
            $a1_1 = &tiziano_gib_deirm_blc_b_linear;
            $s1_1 = 0x24;
            break;
        }
        case 0x45:
        {
            $a1_1 = U"A?CB?????";
            $s1_1 = 0x24;
            break;
        }
        case 0x46:
        {
            $a1_1 = &gib_ir_point;
            $s1_1 = 0x10;
            break;
        }
        case 0x47:
        {
            $a1_1 = &gib_ir_reser;
            $s1_1 = 0x3c;
            break;
        }
        case 0x48:
        {
            $a1_1 = &tiziano_gib_deir_r_h;
            $s1_1 = 0x84;
            break;
        }
        case 0x49:
        {
            $a1_1 = &tiziano_gib_deir_g_h;
            $s1_1 = 0x84;
            break;
        }
        case 0x4a:
        {
            $a1_1 = &tiziano_gib_deir_b_h;
            $s1_1 = 0x84;
            break;
        }
        case 0x4b:
        {
            $a1_1 = &tiziano_gib_deir_r_m;
            $s1_1 = 0x84;
            break;
        }
        case 0x4c:
        {
            $a1_1 = &tiziano_gib_deir_g_m;
            $s1_1 = 0x84;
            break;
        }
        case 0x4d:
        {
            $a1_1 = &tiziano_gib_deir_b_m;
            $s1_1 = 0x84;
            break;
        }
        case 0x4e:
        {
            $a1_1 = &tiziano_gib_deir_r_l;
            $s1_1 = 0x84;
            break;
        }
        case 0x4f:
        {
            $a1_1 = &tiziano_gib_deir_g_l;
            $s1_1 = 0x84;
            break;
        }
        case 0x50:
        {
            $a1_1 = &tiziano_gib_deir_b_l;
            $s1_1 = 0x84;
            break;
        }
        case 0x51:
        {
            $a1_1 = &tiziano_gib_deir_matrix_h;
            $s1_1 = 0x3c;
            break;
        }
        case 0x52:
        {
            $a1_1 = &tiziano_gib_deir_matrix_m;
            $s1_1 = 0x3c;
            break;
        }
        case 0x53:
        {
            $a1_1 = &tiziano_gib_deir_matrix_l;
            $s1_1 = 0x3c;
            break;
        }
    }
    
    memcpy(arg2, $a1_1, $s1_1);
    *arg3 = $s1_1;
    return 0;
}

