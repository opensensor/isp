#include "include/main.h"


  int32_t tisp_gb_blc_again_interp(int32_t arg1, int32_t arg2)

{
    int32_t $s4 = arg1 & 0xffff;
    uint32_t $s2 = arg1 >> 0x10;
    int32_t $s0 = tisp_simple_intp($s2, $s4, &tisp_gb_blc_ir[0x24]);
    int32_t $v0_1 = tisp_simple_intp($s2, $s4, &tisp_gb_blc_ir[0x1b]);
    int32_t $v0_2 = tisp_simple_intp($s2, $s4, &tisp_gb_blc_ir[0x12]);
    int32_t $v0_3 = tisp_simple_intp($s2, $s4, &tisp_gb_blc_ir[9]);
    int32_t $v0_4 = tisp_simple_intp($s2, $s4, U"A?CB?????A?CB?????A?CB?????A?CB?????A?CB?????");
    int32_t $v0_5 = tisp_simple_intp($s2, $s4, &tisp_gb_blc_min);
    int32_t $s4_1;
    int32_t $s6_1;
    int32_t $s7_1;
    
    if (!arg2)
    {
        *tisp_gb_blc_ag = arg1;
        $s4_1 = 0x1020;
        $s6_1 = 0x101c;
        $s7_1 = 0x1018;
    }
    else if (arg2 != 1)
    {
        $s4_1 = 0x1020;
        $s6_1 = 0x101c;
        $s7_1 = 0x1018;
    }
    else
    {
        *(((void**)((char*)tisp_gb_blc_ag + 4))) = arg1; // Fixed void pointer dereference
        $s4_1 = 0x102c;
        $s6_1 = 0x1028;
        $s7_1 = 0x1024;
    }
    
    int32_t $v0_6;
    int32_t $a2;
    $v0_6 = system_reg_read(8);
    int32_t $v0_7 = $v0_6 & 0x1f;
    int32_t $v1_1;
    int32_t $s1;
    int32_t $s2_1;
    int32_t $fp;
    
    if ($(uintptr_t)v0_7 >= 0x18)
    {
    label_32958:
        isp_printf(); // Fixed: macro call, removed arguments;
        $fp = 0;
        $s1 = 0;
        $v1_1 = 0;
        $s2_1 = 0;
        $s0 = 0;
    }
    else
        switch ($v0_7)
        {
            case 0:
            {
                $fp = $v0_4;
                $s1 = $v0_3;
                $v1_1 = $v0_2;
                $s2_1 = $v0_1;
                break;
            }
            case 1:
            {
                $fp = $v0_4;
                $s1 = $s0;
            label_328c8:
                $v1_1 = $v0_1;
                $s2_1 = $v0_2;
                $s0 = $v0_3;
                break;
            }
            case 2:
            {
                $fp = $v0_4;
                $s1 = $v0_2;
            label_32868:
                $v1_1 = $v0_3;
                $s2_1 = $s0;
                $s0 = $v0_1;
                break;
            }
            case 3:
            {
                $fp = $v0_4;
                $s1 = $v0_1;
            label_32884:
                $v1_1 = $s0;
                $s2_1 = $v0_3;
                $s0 = $v0_2;
                break;
            }
            case 4:
            case 5:
            case 6:
            case 7:
            {
                goto label_32958;
            }
            case 8:
            {
                $fp = $v0_2;
                $s1 = $v0_3;
                $v1_1 = $v0_4;
                $s2_1 = $v0_1;
                break;
            }
            case 9:
            {
                $fp = $v0_1;
                $s1 = $s0;
                $v1_1 = $v0_4;
                $s2_1 = $v0_2;
                $s0 = $v0_3;
                break;
            }
            case 0xa:
            {
                $fp = $v0_1;
                $s1 = $v0_3;
                $v1_1 = $v0_2;
                $s2_1 = $v0_4;
                break;
            }
            case 0xb:
            {
                $fp = $v0_2;
                $s1 = $s0;
                $v1_1 = $v0_1;
                $s2_1 = $v0_4;
                $s0 = $v0_3;
                break;
            }
            case 0xc:
            {
                $fp = $v0_2;
                $s1 = $v0_4;
                goto label_32868;
            }
            case 0xd:
            {
                $fp = $v0_1;
                $s1 = $v0_4;
                goto label_32884;
            }
            case 0xe:
            {
                $fp = $v0_1;
                $s1 = $v0_2;
                $v1_1 = $v0_3;
                $s2_1 = $s0;
                $s0 = $v0_4;
                break;
            }
            case 0xf:
            {
                $fp = $v0_2;
                $s1 = $v0_1;
                $v1_1 = $s0;
                $s2_1 = $v0_3;
                $s0 = $v0_4;
                break;
            }
            case 0x10:
            {
                $fp = $v0_3;
                $s1 = $v0_4;
                $v1_1 = $v0_2;
                $s2_1 = $v0_1;
                break;
            }
            case 0x11:
            {
                $fp = $s0;
                $s1 = $v0_4;
                goto label_328c8;
            }
            case 0x12:
            {
                $fp = $v0_3;
                $s1 = $v0_2;
                $v1_1 = $v0_4;
                $s2_1 = $s0;
                $s0 = $v0_1;
                break;
            }
            case 0x13:
            {
                $fp = $s0;
                $s1 = $v0_1;
                $v1_1 = $v0_4;
                $s2_1 = $v0_3;
                $s0 = $v0_2;
                break;
            }
            case 0x14:
            {
                $fp = $v0_3;
                $s1 = $v0_1;
                $v1_1 = $s0;
                $s2_1 = $v0_4;
                $s0 = $v0_2;
                break;
            }
            case 0x15:
            {
                $fp = $s0;
                $s1 = $v0_2;
                $v1_1 = $v0_3;
                $s2_1 = $v0_4;
                $s0 = $v0_1;
                break;
            }
            case 0x16:
            {
                $fp = $v0_3;
                $s1 = $s0;
                $v1_1 = $v0_1;
                $s2_1 = $v0_2;
                $s0 = $v0_4;
                break;
            }
            case 0x17:
            {
                $fp = $s0;
                $s1 = $v0_3;
                $v1_1 = $v0_2;
                $s2_1 = $v0_1;
                $s0 = $v0_4;
                break;
            }
        }
    
    system_reg_write(0x1014, *(tisp_gb_blc_min_en + 4) << 0x10 | *tisp_gb_blc_min_en);
    system_reg_write($s7_1, $s2_1 << 0x10 | $s0);
    system_reg_write($s6_1, $s1 << 0x10 | $v1_1);
    system_reg_write($s4_1, $v0_5 << 0x10 | $fp);
    return 0;
}

