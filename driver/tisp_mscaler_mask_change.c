#include "include/main.h"


  int16_t* tisp_mscaler_mask_change(int32_t arg1)

{
    int32_t var_38_67 = arg1;
    isp_printf(1, "register is 0x%x, value is 0x%x\\n", "tisp_mscaler_mask_change");
    int32_t $v0_2 = ds0_attr << 1 | data_b2df0_1;
    int16_t $a0_1;
    int16_t $s2;
    
    if ($v0_2 == 2)
    {
        $a0_1 = data_b2dec_2;
        $s2 = data_b2de8_2;
    }
    else if ($v0_2 >= 3)
    {
        if ($v0_2 != 3)
        {
            $s2 = 0;
            $a0_1 = 0;
        }
        else
        {
            $a0_1 = data_b2e00_1;
            $s2 = data_b2dfc_1;
        }
    }
    else if ($v0_2 < 0)
    {
        $s2 = 0;
        $a0_1 = 0;
    }
    else
    {
        $a0_1 = data_b2e00_2;
        $s2 = data_b2dfc_2;
    }
    
    int32_t $v0_5 = ds1_attr << 1 | data_b2dbc_1;
    int16_t $a1_1;
    int16_t $s3;
    
    if ($v0_5 == 2)
    {
        $a1_1 = data_b2db8_2;
        $s3 = data_b2db4_2;
    }
    else if ($v0_5 >= 3)
    {
        if ($v0_5 != 3)
        {
            $s3 = 0;
            $a1_1 = 0;
        }
        else
        {
            $a1_1 = data_b2dcc_1;
            $s3 = data_b2dc8_1;
        }
    }
    else if ($v0_5 < 0)
    {
        $s3 = 0;
        $a1_1 = 0;
    }
    else
    {
        $a1_1 = data_b2dcc_2;
        $s3 = data_b2dc8_2;
    }
    
    int32_t $v0_8 = ds2_attr << 1 | data_b2d88_1;
    int16_t $a2_1;
    int16_t $s5;
    
    if ($v0_8 == 2)
    {
        $a2_1 = data_b2d84_2;
        $s5 = data_b2d80_2;
    }
    else if ($v0_8 >= 3)
    {
        if ($v0_8 != 3)
        {
            $s5 = 0;
            $a2_1 = 0;
        }
        else
        {
            $a2_1 = data_b2d98_1;
            $s5 = data_b2d94_1;
        }
    }
    else if ($v0_8 < 0)
    {
        $s5 = 0;
        $a2_1 = 0;
    }
    else
    {
        $a2_1 = data_b2d98_2;
        $s5 = data_b2d94_2;
    }
    
    int16_t* result;
    
    if (arg1 == 2)
    {
        void* $v1_19 = data_ca490_1;
        result = $v1_19 + 0x72;
        uint32_t $v1_20 = *(result - 0x72);
        
        while (true)
        {
            uint32_t $v1_21;
            
            if ($v1_20 != 1)
                $v1_21 = *(result - 0x3a);
            else
            {
                *(result - 0x70) = $a0_1 - *(result - 0x6a) - *(result - 0x70);
                $v1_21 = *(result - 0x3a);
            }
            
            uint32_t $v1_25;
            
            if ($v1_21 != 1)
                $v1_25 = *(result - 2);
            else
            {
                *(result - 0x38) = $a1_1 - *(result - 0x32) - *(result - 0x38);
                $v1_25 = *(result - 2);
            }
            
            if ($v1_25 != 1)
                result = &result[7];
            else
            {
                *result = $a2_1 - result[3] - *result;
                result = &result[7];
            }
            
            if (result == $v1_19 + 0xaa)
                break;
            
            $v1_20 = *(result - 0x72);
        }
    }
    else
    {
        result = 1;
        
        if (arg1 == 3)
        {
            void* $a3_2 = data_ca490_2;
            result = $a3_2 + 0x74;
            uint32_t $v1_32 = *(result - 0x74);
            
            while (true)
            {
                uint32_t $v1_33;
                
                if ($v1_32 != 1)
                    $v1_33 = *(result - 0x3c);
                else
                {
                    *(result - 0x70) = $s2 - *(result - 0x6e) - *(result - 0x70);
                    *(result - 0x72) = $a0_1 - *(result - 0x6c) - *(result - 0x72);
                    $v1_33 = *(result - 0x3c);
                }
                
                uint32_t $v1_40;
                
                if ($v1_33 != 1)
                    $v1_40 = *(result - 4);
                else
                {
                    *(result - 0x38) = $s3 - *(result - 0x36) - *(result - 0x38);
                    *(result - 0x3a) = $a1_1 - *(result - 0x34) - *(result - 0x3a);
                    $v1_40 = *(result - 4);
                }
                
                if ($v1_40 != 1)
                    result = &result[7];
                else
                {
                    *result = $s5 - result[1] - *result;
                    *(result - 2) = $a2_1 - result[2] - *(result - 2);
                    result = &result[7];
                }
                
                if ($a3_2 + 0xac == result)
                    break;
                
                $v1_32 = *(result - 0x74);
            }
        }
        else if (arg1 == 1)
        {
            int32_t i = 0;
            int32_t $v1_4 = 0;
            
            do
            {
                void* $v0_9 = data_ca490_3;
                void* $v1_5;
                
                if (*($v0_9 + $v1_4) != 1)
                    $v1_5 = i * 0xe;
                else
                {
                    uint32_t var_34_1_10 = *($v0_9 + 2);
                    uint32_t var_38_1_13 = *($v0_9 + 4);
                    isp_printf(1, "count is %d\\n", "tisp_mscaler_mask_change");
                    void* $v0_12 = data_ca490_4 + $v1_4;
                    *($v0_12 + 4) = $s2 - *($v0_12 + 6) - *($v0_12 + 4);
                    $v1_5 = i * 0xe;
                }
                
                int32_t $v0_13 = data_ca490_5;
                void* $a0_6 = $v1_5 + $v0_13;
                void* $v1_11 = i * 0xe;
                
                if (*($a0_6 + 0x38) == 1)
                {
                    *($a0_6 + 0x3c) = $s3 - *($a0_6 + 0x3e) - *($a0_6 + 0x3c);
                    $v1_11 = i * 0xe;
                }
                
                void* $v0_14 = $v1_11 + $v0_13;
                i += 1;
                
                if (*($v0_14 + 0x70) == 1)
                    *($v0_14 + 0x74) = $s5 - *($v0_14 + 0x76) - *($v0_14 + 0x74);
                
                result = 4;
                $v1_4 = i * 0xe;
            } while (i != 4);
        }
    }
    
    return result;
}

