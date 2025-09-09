#include "include/main.h"


  int32_t tisp_set_csc_version(char arg1)

{
    uint32_t $a2_2 = arg1;
        return 0xffffffff;
    
    if ($a2_2 >= 5)
    {

    }
    
    switch ($a2_2)
    {
        case 0:
        {
            memcpy(&csc_param_current, 0x9ac20, 0x3c);
            csc_version_now = 0;
            break;
        }
        case 1:
        {
            memcpy(&csc_param_current, 0x9abe4, 0x3c);
            csc_version_now = 1;
            break;
        }
        case 2:
        {
            memcpy(&csc_param_current, 0x9aba8, 0x3c);
            csc_version_now = 2;
            break;
        }
        case 3:
        {
            memcpy(&csc_param_current, 0x9ab6c, 0x3c);
            csc_version_now = 3;
            break;
        }
        case 4:
        {
            memcpy(&csc_param_current, 0x9ab30, 0x3c);
            csc_version_now = 4;
            break;
        }
    }
    
    system_reg_write(0x6000, 0x1f);
    system_reg_write(0x6004, 0);
    int32_t $v0_3 = data_cd464_1;
    int32_t $a0 = data_cd468_1;
    int32_t $v1_2 = $v0_3 >> 0x1f;
    int32_t $v0_7 = $a0 >> 0x1f;
    int32_t csc_param_current_1 = csc_param_current;
    int32_t $a1_1 = csc_param_current_1 >> 0x1f;
    system_reg_write(0x6010, 
        ((($v1_2 ^ $v0_3) - $v1_2) & 0x3ff) << 0xa | ((($v0_7 ^ $a0) - $v0_7) & 0x3ff) << 0x14
            | ((($a1_1 ^ csc_param_current_1) - $a1_1) & 0x3ff));
    int32_t $v0_12 = data_cd470_1;
    int32_t $a0_2 = data_cd474_1;
    int32_t $v1_5 = $v0_12 >> 0x1f;
    int32_t $v0_16 = $a0_2 >> 0x1f;
    int32_t $v1_7 = data_cd46c_1;
    int32_t $a1_5 = $v1_7 >> 0x1f;
    system_reg_write(0x6014, 
        ((($v1_5 ^ $v0_12) - $v1_5) & 0x3ff) << 0xa | ((($v0_16 ^ $a0_2) - $v0_16) & 0x3ff) << 0x14
            | ((($a1_5 ^ $v1_7) - $a1_5) & 0x3ff));
    int32_t $v0_21 = data_cd47c_1;
    int32_t $a0_4 = data_cd480_1;
    int32_t $v1_9 = $v0_21 >> 0x1f;
    int32_t $v0_25 = $a0_4 >> 0x1f;
    int32_t $v1_11 = data_cd478_1;
    int32_t $a1_9 = $v1_11 >> 0x1f;
    system_reg_write(0x6018, 
        ((($v1_9 ^ $v0_21) - $v1_9) & 0x3ff) << 0xa | ((($v0_25 ^ $a0_4) - $v0_25) & 0x3ff) << 0x14
            | ((($a1_9 ^ $v1_11) - $a1_9) & 0x3ff));
    system_reg_write(0x6020, data_cd488_1 << 8 | data_cd484_1);
    system_reg_write(0x6030, 
        data_cd494_1 | data_cd490_1 << 0x18 | data_cd498_1 << 8 | data_cd48c_1 << 0x10);
    return 1;
}

