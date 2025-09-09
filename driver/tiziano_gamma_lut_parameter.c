#include "include/main.h"


  int32_t tiziano_gamma_lut_parameter()

{
    void* const $s1 = &data_40000_2;
    
    for (int32_t i = 2; i != 0x102; )
    {
        void* tiziano_gamma_lut_now_1 = tiziano_gamma_lut_now;
        system_reg_write($s1, 
            *(tiziano_gamma_lut_now_1 + i) << 0xc | *(tiziano_gamma_lut_now_1 + i - 2));
        void* tiziano_gamma_lut_now_2 = tiziano_gamma_lut_now;
        system_reg_write($s1 + 0x8000, 
            *(tiziano_gamma_lut_now_2 + i) << 0xc | *(tiziano_gamma_lut_now_2 + i - 2));
        void* tiziano_gamma_lut_now_3 = tiziano_gamma_lut_now;
        uint32_t $a1_6 = *(tiziano_gamma_lut_now_3 + i) << 0xc | *(tiziano_gamma_lut_now_3 + i - 2);
        i += 2;
        system_reg_write($s1 + isp_printf, $a1_6);
        $s1 += 4;
    }
    
    return 0;
}

