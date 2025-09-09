#include "include/main.h"


  int32_t tisp_gib_param_array_set(int32_t arg1, int32_t arg2, int32_t* arg3)

{
        int32_t var_18_1 = arg1;
        return 0xffffffff;
    if (arg1 - (uintptr_t)0x3e >= 0x16)
    {

    }
    
    int32_t $s0_1;
    
    switch (arg1)
    {
        case 0x3e:
        {
            memcpy(&tiziano_gib_config_line, arg2, 0x30);
            $s0_1 = 0x30;
            break;
        }
        case 0x3f:
        {
            $s0_1 = 8;
            memcpy(&tiziano_gib_r_g_linear);
            break;
        }
        case 0x40:
        {
            $s0_1 = 8;
            memcpy(&tiziano_gib_b_ir_linear);
            break;
        }
        case 0x41:
        {
            $s0_1 = 0x24;
            memcpy(&tiziano_gib_deirm_blc_r_linear);
            break;
        }
        case 0x42:
        {
            $s0_1 = 0x24;
            memcpy(&tiziano_gib_deirm_blc_gr_linear);
            break;
        }
        case 0x43:
        {
            $s0_1 = 0x24;
            memcpy(&tiziano_gib_deirm_blc_gb_linear);
            break;
        }
        case 0x44:
        {
            $s0_1 = 0x24;
            memcpy(&tiziano_gib_deirm_blc_b_linear);
            break;
        }
        case 0x45:
        {
            $s0_1 = 0x24;
            memcpy(U"A?CB?????");
            break;
        }
        case 0x46:
        {
            memcpy(&gib_ir_point, arg2, 0x10);
            $s0_1 = 0x10;
            break;
        }
        case 0x47:
        {
            $s0_1 = 0x3c;
            memcpy(&gib_ir_reser);
            break;
        }
        case 0x48:
        {
            $s0_1 = 0x84;
            memcpy(&tiziano_gib_deir_r_h);
            break;
        }
        case 0x49:
        {
            $s0_1 = 0x84;
            memcpy(&tiziano_gib_deir_g_h);
            break;
        }
        case 0x4a:
        {
            $s0_1 = 0x84;
            memcpy(&tiziano_gib_deir_b_h);
            break;
        }
        case 0x4b:
        {
            $s0_1 = 0x84;
            memcpy(&tiziano_gib_deir_r_m);
            break;
        }
        case 0x4c:
        {
            $s0_1 = 0x84;
            memcpy(&tiziano_gib_deir_g_m);
            break;
        }
        case 0x4d:
        {
            $s0_1 = 0x84;
            memcpy(&tiziano_gib_deir_b_m);
            break;
        }
        case 0x4e:
        {
            $s0_1 = 0x84;
            memcpy(&tiziano_gib_deir_r_l);
            break;
        }
        case 0x4f:
        {
            $s0_1 = 0x84;
            memcpy(&tiziano_gib_deir_g_l);
            break;
        }
        case 0x50:
        {
            $s0_1 = 0x84;
            memcpy(&tiziano_gib_deir_b_l);
            break;
        }
        case 0x51:
        {
            $s0_1 = 0x3c;
            memcpy(&tiziano_gib_deir_matrix_h);
            break;
        }
        case 0x52:
        {
            $s0_1 = 0x3c;
            memcpy(&tiziano_gib_deir_matrix_m);
            break;
        }
        case 0x53:
        {
            $s0_1 = 0x3c;
            memcpy(&tiziano_gib_deir_matrix_l);
            break;
        }
    }
    
    tiziano_gib_lut_parameter();
    trig_set_deir = 1;
    *arg3 = $s0_1;
    return 0;
}

