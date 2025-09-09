#include "include/main.h"


  int32_t JZ_Isp_Awb_Awbg2reg(int32_t* arg1, int32_t* arg2)

{
    int32_t $a3_1 = *arg1 << 2;
    int32_t $a2_1 = arg1[1] << 2;
        uint32_t _awb_ct_1 = _awb_ct;
        uint32_t _awb_ct_last_offset_1 = _awb_ct_last_offset;
        uint32_t $v1_1;
        uint32_t awb_gr_offset_1;
            uint32_t $t5_1 = data_a9e18;
            uint32_t $v1_3 = data_a9e20;
            uint32_t $v0_1 = data_a9e28;
    
    if (_awb_trend == 1)
    {
        
        $v1_1 = _awb_ct_last_offset_1 >= _awb_ct_1 ? _awb_ct_last_offset_1 - _awb_ct_1
            : _awb_ct_1 - _awb_ct_last_offset_1;
        
        
        if ($(uintptr_t)v1_1 >= 0xc8 || awb_moa == 1)
        {
            _awb_ct_last_offset = _awb_ct_1;
            
            if ((uintptr_t)_awb_ct_1 >= 0x1388)
            {
                awb_gr_offset = data_a9e14;
                awb_gb_offset = $t5_1;
            }
            else if ((uintptr_t)_awb_ct_1 >= 0xbb9)
            {
                awb_gr_offset = data_a9e1c;
                awb_gb_offset = $v1_3;
            }
            else
            {
                awb_gr_offset = data_a9e24;
                awb_gb_offset = $v0_1;
            }
            
            awb_gr_offset_1 = awb_gr_offset;
        }
        else
            awb_gr_offset_1 = awb_gr_offset;
        
        $a3_1 += awb_gr_offset_1 - 0x400;
        $a2_1 += awb_gb_offset - 0x400;
    }
    
    if ($(uintptr_t)a3_1 >= 0x4000)
        $a3_1 = 0x3fff;
    
    int32_t result = $(uintptr_t)a2_1 < 0x4000 ? 1 : 0;
    
    if (!result)
        $a2_1 = 0x3fff;
    
    *arg2 = $a3_1 | 0x4000000;
    arg2[1] = $a2_1 | 0x4000000;
    return result;
}

