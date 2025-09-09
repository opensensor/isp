#include "include/main.h"


  int32_t tisp_s_ccm_attr(void* arg1)

{
    int32_t $v0 = system_reg_read(0xc);
        void* $a0_2 = arg1;
                char var_5f_1 = 1;
    void var_38;
    memcpy(&var_38, 0x7dc40, 0x24);
    
    if (!($v0 & isp_printf))
    {
        
        if (!($v0 & 0x200))
        {
            void var_60;
            memcpy(&var_60, arg1, 0x28);
            void var_5c;
            memcpy(&var_5c, &var_38, 0x24);
            
            if (*(arg1 + 1))
            {
                *(((int32_t*)((char*)arg1 + 1))) = 0; // Fixed void pointer dereference
            }
            
            tisp_ccm_set_attr(arg1);
            $a0_2 = &var_60_6;
        }
        
        tisp_bcsh_set_attr($a0_2);
    }
    else
        tisp_ccm_set_attr(arg1);
    
    return 0;
}

