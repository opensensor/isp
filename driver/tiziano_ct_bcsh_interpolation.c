#include "include/main.h"


  int32_t tiziano_ct_bcsh_interpolation(int32_t arg1)

{
    int32_t $ra;
    int32_t var_4_4 = $ra;
    
    if (arg1 < 0x1357)
    {
        uint32_t $v1_3;
        
        if (arg1 >= 0xf0b)
            $v1_3 = 1;
        else if (arg1 >= 0xea7)
            $v1_3 = 2;
        else if (arg1 < 0xb23)
            $v1_3 = 4;
        else
            $v1_3 = 3;
        
        ct_flag.31949 = $v1_3;
    }
    else
        ct_flag.31949 = 0;
    
    uint32_t ct_flag.31949_1;
    
    if (BCSH_real)
        ct_flag.31949_1 = ct_flag.31949;
    else
    {
        uint32_t ct_flag.31949_2 = ct_flag.31949;
        
        if (!ct_flag.31949_2)
        {
            if (!ct_flag_last.31950)
                return &data_c0000_2;
            
            ct_flag.31949_1 = ct_flag.31949;
        }
        else if (ct_flag.31949_2 == 2 || ct_flag.31949_2 == 4)
        {
            if (ct_flag_last.31950 == ct_flag.31949_2)
                return &data_c0000_3;
            
            ct_flag.31949_1 = ct_flag.31949;
        }
        else
            ct_flag.31949_1 = ct_flag.31949;
    }
    
    ct_flag_last.31950 = ct_flag.31949_1;
    
    switch (ct_flag.31949_1)
    {
        case 0:
        {
            memcpy(&tisp_BCSH_as32CCMMatrix, &tisp_BCSH_as32CCMMatrix_d, 0x24);
            break;
        }
        case 1:
        {
            int32_t $v1_9 = 0xf0a - arg1;
            
            if (arg1 >= 0xf0b)
                $v1_9 = arg1 - 0xf0a;
            
            for (int32_t i = 0; i != 0x24; )
            {
                int32_t $v1_10 = *(&tisp_BCSH_as32CCMMatrix_t + i);
                int32_t $v0_5 = *(&tisp_BCSH_as32CCMMatrix_d + i);
                int32_t $v0_10;
                
                $v0_10 = $v0_5 >= $v1_10 ? ($v0_5 - $v1_10) * $v1_9 / 0x44c + $v1_10
                    : $v1_10 - ($v1_10 - $v0_5) * $v1_9 / 0x44c;
                
                void* $v1_11 = &tisp_BCSH_as32CCMMatrix + i;
                i += 4;
                *$v1_11 = $v0_10;
            }
            break;
        }
        case 2:
        {
            memcpy(&tisp_BCSH_as32CCMMatrix, &tisp_BCSH_as32CCMMatrix_t, 0x24);
            break;
        }
        case 3:
        {
            int32_t $v1_12 = 0xb22 - arg1;
            
            if (arg1 >= 0xb23)
                $v1_12 = arg1 - 0xb22;
            
            for (int32_t i_1 = 0; i_1 != 0x24; )
            {
                int32_t $v1_13 = *(&tisp_BCSH_as32CCMMatrix_a + i_1);
                int32_t $v0_16 = *(&tisp_BCSH_as32CCMMatrix_t + i_1);
                int32_t $v0_21;
                
                $v0_21 = $v0_16 >= $v1_13 ? ($v0_16 - $v1_13) * $v1_12 / 0x384 + $v1_13
                    : $v1_13 - ($v1_13 - $v0_16) * $v1_12 / 0x384;
                
                void* $v1_14 = &tisp_BCSH_as32CCMMatrix + i_1;
                i_1 += 4;
                *$v1_14 = $v0_21;
            }
            break;
        }
        case 4:
        {
            memcpy(&tisp_BCSH_as32CCMMatrix, &tisp_BCSH_as32CCMMatrix_a, 0x24);
            break;
        }
    }
    
    /* tailcall */
    return memcpy(0xc5490, &tisp_BCSH_as32CCMMatrix, 0x24);
}

