#include "include/main.h"


  int32_t tisp_g_ccm_attr(void* arg1)

{
    int32_t $v0 = system_reg_read(0xc);
        int32_t $v0_2 = var_37;
    
    if ($v0 & isp_printf)
        tisp_ccm_get_attr(arg1);
    else if (!($v0 & 0x200))
    {
        tisp_ccm_get_attr(arg1);
        void var_38;
        tisp_bcsh_get_attr(&var_38);
        char var_37;
        
        if ($v0_2 == 1)
            *(((void**)((char*)arg1 + 1))) = $v0_2; // Fixed void pointer dereference
    }
    else
        tisp_bcsh_get_attr(arg1);
    
    return 0;
}

