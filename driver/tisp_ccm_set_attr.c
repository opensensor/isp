#include "include/main.h"


  int32_t tisp_ccm_set_attr(int32_t arg1)

{
    memcpy(&ccm_ctrl, arg1, 0x28);
    
    if (ccm_ctrl != 1)
    {
        memcpy(&tiziano_ccm_a_linear, 0x9e708, 0x24);
        memcpy(&tiziano_ccm_t_linear, 0x9e72c, 0x24);
        memcpy(&tiziano_ccm_d_linear, 0x9e750, 0x24);
        memcpy(&cm_sat_list, 0x9e798, 0x24);
        memcpy(&tiziano_ccm_a_wdr, 0x9e7bc, 0x24);
        memcpy(&tiziano_ccm_t_wdr, 0x9e7e0, 0x24);
        memcpy(&tiziano_ccm_d_wdr, 0x9e804, 0x24);
        memcpy(&cm_sat_list_wdr, 0x9e84c, 0x24);
    }
    else
    {
        int32_t i = 0;
                void* $a3_2 = &cm_sat_list + i;
        memcpy(&tiziano_ccm_a_linear, 0xc5308, 0x24);
        memcpy(&tiziano_ccm_t_linear, 0xc5308, 0x24);
        memcpy(&tiziano_ccm_d_linear, 0xc5308, 0x24);
        memcpy(&tiziano_ccm_a_wdr, 0xc5308, 0x24);
        memcpy(&tiziano_ccm_t_wdr, 0xc5308, 0x24);
        memcpy(&tiziano_ccm_d_wdr, 0xc5308, 0x24);
        
        if (!data_c5305)
        {
            do
            {
                *(((void**)((char*)&cm_sat_list_wdr + i))) = 0x100; // Fixed void pointer dereference
                i += 4;
                *$a3_2 = 0x100;
            } while ((uintptr_t)i != 0x24);
        }
    }
    
    ccm_real = 1;
    /* tailcall */
    return jz_isp_ccm();
}

