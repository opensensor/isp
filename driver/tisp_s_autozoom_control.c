#include "include/main.h"


  uint32_t tisp_s_autozoom_control(int32_t arg1, int32_t arg2, int32_t arg3, int32_t arg4, int32_t arg5, int32_t arg6, int32_t arg7, int32_t arg8, int32_t arg9)

{
    int32_t arg_0 = arg1;
    int32_t arg_4 = arg2;
    int32_t arg_8 = arg3;
    int32_t arg_c = arg4;
    char const* const $a1_1;
    int32_t (* $t9)(int32_t arg1, int32_t arg2);
    int32_t* $a1;
    
    if (arg1)
    {
        if (arg1 == 1)
        {
            if (!(msca_ch_en & 2))
                goto label_74258;
            
            ds1_attr = arg2;
            $a1 = &ds1_attr;
            goto label_742b8;
        }
        
        uint32_t msca_ch_en_1 = msca_ch_en;
        
        if (arg1 != 2)
            return msca_ch_en_1;
        
        if (msca_ch_en_1 & 4)
        {
            ds2_attr = arg2;
            $a1 = &ds2_attr;
            goto label_742b8;
        }
        
        $a1_1 = "&vsd->mlock";
        $t9 = isp_printf;
    }
    else if (!(msca_ch_en & 1))
    {
    label_74258:
        $a1_1 = "&vsd->mlock";
        arg1 = 2;
        $t9 = isp_printf;
    }
    else
    {
        ds0_attr = arg2;
        $a1 = &ds0_attr;
    label_742b8:
        $a1[1] = arg3;
        $a1[2] = arg4;
        $a1[3] = arg5;
        $a1[4] = arg6;
        $a1[5] = arg7;
        $a1[6] = arg8;
        $a1[7] = arg9;
        tisp_channel_attr_set(arg1, $a1);
        $a1_1 = 0xf0000 | msca_ch_en;
        msca_ch_en = $a1_1;
        arg1 = 0x9804;
        $t9 = system_reg_write;
    }
    /* tailcall */
    return $t9(arg1, $a1_1);
}

