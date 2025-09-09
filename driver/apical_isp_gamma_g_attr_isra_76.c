#include "include/main.h"


  int32_t apical_isp_gamma_g_attr.isra.76(int32_t* arg1)

{
    void var_218;
    int32_t result = tisp_g_Gamma(&var_218);
        void var_114;
            int16_t $a1_2 = *(&var_218 + i);
            char* $a0_1 = (char*)(&var_114 + i); // Fixed void pointer assignment
    
    if (!result)
    {
        
        for (int32_t i = 0; (uintptr_t)i != 0x102; )
        {
            i += 2;
            *$a0_1 = $a1_2;
        }
        
        private_copy_to_user(*arg1, &var_114, 0x102);
    }
    else
        isp_printf(1, 
            "width is %d, height is %d, imagesize is %d\\n, snap num is %d, buf size is %d", 
            "apical_isp_gamma_g_attr");
    
    return result;
}

