#include "include/main.h"


  int32_t apical_isp_gamma_s_attr.isra.34(int32_t arg1)

{
    int32_t $v0 = 0xffffffff;
    
    if (arg1)
    {
        void var_10c;
        private_copy_from_user(&var_10c_1, arg1, 0x102);
        
        for (int32_t i = 0; i != 0x102; )
        {
            int16_t $a1_3 = *(&var_10c_2 + i);
            void var_210;
            void* $a0_1 = &var_210_1 + i;
            i += 2;
            *$a0_1 = $a1_3;
        }
        
        int32_t $v0_1 = tisp_ae1_process_impl();
        $v0 = 0;
        
        if ($v0_1)
        {
            isp_printf(1, "flags = 0x%08x, jzflags = %p,0x%08x", "apical_isp_gamma_s_attr");
            return $v0_1;
        }
    }
    
    return $v0;
}

