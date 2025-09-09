#include "include/main.h"


  int32_t tisp_s_ccm_attr(void* arg1)

{
    void var_38_64;
    memcpy(&var_38_65, 0x7dc40, 0x24);
    int32_t $v0 = system_reg_read(0xc);
    
    if (!($v0 & isp_printf))
    {
        void* $a0_2 = arg1;
        
        if (!($v0 & 0x200))
        {
            void var_60_24;
            memcpy(&var_60_25, arg1, 0x28);
            void var_5c_11;
            memcpy(&var_5c_12, &var_38_66, 0x24);
            
            if (*(arg1 + 1))
            {
                char var_5f_1 = 1;
                *(arg1 + 1) = 0;
            }
            
            tisp_ccm_set_attr(arg1);
            $a0_2 = &var_60_26;
        }
        
        tisp_bcsh_set_attr($a0_2);
    }
    else
        tisp_ccm_set_attr(arg1);
    
    return 0;
}

