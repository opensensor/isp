#include "include/main.h"


  int32_t tisp_gib_gain_interpolation(uint32_t arg1)

{
    int32_t $s3 = arg1 & 0xffff;
    uint32_t $s2 = arg1 >> 0x10;
    int32_t $s0 = tisp_simple_intp($s2, $s3, &tiziano_gib_deirm_blc_r_linear);
    int32_t $v0_1 = tisp_simple_intp($s2, $s3, &tiziano_gib_deirm_blc_gr_linear);
    int32_t $v0_2 = tisp_simple_intp($s2, $s3, &tiziano_gib_deirm_blc_gb_linear);
    int32_t $v0_3 = tisp_simple_intp($s2, $s3, &tiziano_gib_deirm_blc_b_linear);
    int32_t $v0_4 = tisp_simple_intp($s2, $s3, U"A?CB?????");
    int32_t $v0_5;
    int32_t $a2;
    int32_t $v0_6 = $v0_5 & 0x1f;
    int32_t $s1;
    int32_t $s2_1;
    int32_t $s3_1;
    int32_t $s4;
    $v0_5 = system_reg_read(8);
    
    if ($(uintptr_t)v0_6 >= 0x18)
    {
    label_318a8:
        isp_printf(); // Fixed: macro with no parameters, removed 3 arguments;
        $s1 = 0;
        $s3_1 = 0;
        $s2_1 = 0;
        $s4 = 0;
        $s0 = 0;
    }
    else
        switch ($v0_6)
        {
            case 0:
            {
                $s1 = $v0_4;
                $s3_1 = $v0_3;
                $s2_1 = $v0_2;
                $s4 = $v0_1;
                break;
            }
            case 1:
            {
                $s1 = $v0_4;
                $s3_1 = $s0;
            label_31818:
                $s2_1 = $v0_1;
                $s4 = $v0_2;
                $s0 = $v0_3;
                break;
            }
            case 2:
            {
                $s1 = $v0_4;
                $s3_1 = $v0_2;
            label_317b8:
                $s2_1 = $v0_3;
                $s4 = $s0;
                $s0 = $v0_1;
                break;
            }
            case 3:
            {
                $s1 = $v0_4;
                $s3_1 = $v0_1;
            label_317d4:
                $s2_1 = $s0;
                $s4 = $v0_3;
                $s0 = $v0_2;
                break;
            }
            case 4:
            case 5:
            case 6:
            case 7:
            {
                goto label_318a8;
            }
            case 8:
            {
                $s1 = $v0_2;
                $s3_1 = $v0_3;
                $s2_1 = $v0_4;
                $s4 = $v0_1;
                break;
            }
            case 9:
            {
                $s1 = $v0_1;
                $s3_1 = $s0;
                $s2_1 = $v0_4;
                $s4 = $v0_2;
                $s0 = $v0_3;
                break;
            }
            case 0xa:
            {
                $s1 = $v0_1;
                $s3_1 = $v0_3;
                $s2_1 = $v0_2;
                $s4 = $v0_4;
                break;
            }
            case 0xb:
            {
                $s1 = $v0_2;
                $s3_1 = $s0;
                $s2_1 = $v0_1;
                $s4 = $v0_4;
                $s0 = $v0_3;
                break;
            }
            case 0xc:
            {
                goto label_317b8;
                $s1 = $v0_2;
                $s3_1 = $v0_4;
            }
            case 0xd:
            {
                goto label_317d4;
                $s1 = $v0_1;
                $s3_1 = $v0_4;
            }
            case 0xe:
            {
                $s1 = $v0_1;
                $s3_1 = $v0_2;
                $s2_1 = $v0_3;
                $s4 = $s0;
                $s0 = $v0_4;
                break;
            }
            case 0xf:
            {
                $s1 = $v0_2;
                $s3_1 = $v0_1;
                $s2_1 = $s0;
                $s4 = $v0_3;
                $s0 = $v0_4;
                break;
            }
            case 0x10:
            {
                $s1 = $v0_3;
                $s3_1 = $v0_4;
                $s2_1 = $v0_2;
                $s4 = $v0_1;
                break;
            }
            case 0x11:
            {
                goto label_31818;
                $s1 = $s0;
                $s3_1 = $v0_4;
            }
            case 0x12:
            {
                $s1 = $v0_3;
                $s3_1 = $v0_2;
                $s2_1 = $v0_4;
                $s4 = $s0;
                $s0 = $v0_1;
                break;
            }
            case 0x13:
            {
                $s1 = $s0;
                $s3_1 = $v0_1;
                $s2_1 = $v0_4;
                $s4 = $v0_3;
                $s0 = $v0_2;
                break;
            }
            case 0x14:
            {
                $s1 = $v0_3;
                $s3_1 = $v0_1;
                $s2_1 = $s0;
                $s4 = $v0_4;
                $s0 = $v0_2;
                break;
            }
            case 0x15:
            {
                $s1 = $s0;
                $s3_1 = $v0_2;
                $s2_1 = $v0_3;
                $s4 = $v0_4;
                $s0 = $v0_1;
                break;
            }
            case 0x16:
            {
                $s1 = $v0_3;
                $s3_1 = $s0;
                $s2_1 = $v0_1;
                $s4 = $v0_2;
                $s0 = $v0_4;
                break;
            }
            case 0x17:
            {
                $s1 = $s0;
                $s3_1 = $v0_3;
                $s2_1 = $v0_2;
                $s4 = $v0_1;
                $s0 = $v0_4;
                break;
            }
        }
    
    system_reg_write_gib(1, 0x1060, $s0);
    system_reg_write_gib(1, 0x1064, $s2_1 << 0x10 | $s4);
    system_reg_write_gib(1, 0x1068, $s1 << 0x10 | $s3_1);
    tisp_gib_blc_ag = arg1;
    return 0;
}

