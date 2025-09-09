#include "include/main.h"


  int16_t* tisp_mscaler_mask_change(int32_t arg1)

{
    int32_t var_38 = arg1;
    int32_t $v0_2 = ds0_attr << 1 | data_b2df0;
    int16_t $a0_1;
    int16_t $s2;
    isp_printf(); // Fixed: macro with no parameters, removed 4 arguments;
    
    if ($v0_2 == 2)
    {
        $a0_1 = data_b2dec;
        $s2 = data_b2de8;
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
            $a0_1 = data_b2e00;
            $s2 = data_b2dfc;
        }
    }
    else if ($v0_2 < 0)
    {
        $s2 = 0;
        $a0_1 = 0;
    }
    else
    {
        $a0_1 = data_b2e00;
        $s2 = data_b2dfc;
    }
    
    int32_t $v0_5 = ds1_attr << 1 | data_b2dbc_1;
    int16_t $a1_1;
    int16_t $s3;
    
    if ($v0_5 == 2)
    {
        $a1_1 = data_b2db8;
        $s3 = data_b2db4;
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
            $a1_1 = data_b2dcc;
            $s3 = data_b2dc8;
        }
    }
    else if ($v0_5 < 0)
    {
        $s3 = 0;
        $a1_1 = 0;
    }
    else
    {
        $a1_1 = data_b2dcc;
        $s3 = data_b2dc8;
    }
    
    int32_t $v0_8 = ds2_attr << 1 | data_b2d88_1;
    int16_t $a2_1;
    int16_t $s5;
    
    if ($v0_8 == 2)
    {
        $a2_1 = data_b2d84;
        $s5 = data_b2d80;
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
            $a2_1 = data_b2d98;
            $s5 = data_b2d94;
        }
    }
    else if ($v0_8 < 0)
    {
        $s5 = 0;
        $a2_1 = 0;
    }
    else
    {
        $a2_1 = data_b2d98;
        $s5 = data_b2d94;
    }
    
    int16_t* result;
    
    if (arg1 == 2)
    {
        char* $v1_19 = (char*)(data_ca490); // Fixed void pointer assignment
        uint32_t $v1_20 = *(result - 0x72);
            uint32_t $v1_21;
        result = $v1_19 + 0x72;
        
        while (true)
        {
            
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
            char* $a3_2 = (char*)(data_ca490); // Fixed void pointer assignment
            uint32_t $v1_32 = *(result - 0x74);
                uint32_t $v1_33;
        result = 1;
        
        if (arg1 == 3)
        {
            result = $a3_2 + 0x74;
            
            while (true)
            {
                
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
                char* $v0_9 = (char*)(data_ca490); // Fixed void pointer assignment
                void* $v1_5;
                    uint32_t var_34_1 = *($v0_9 + 2);
                    uint32_t var_38_1 = *($v0_9 + 4);
                    char* $v0_12 = (char*)(data_ca490 + $v1_4); // Fixed void pointer assignment
            
            do
            {
                
                if (*($v0_9 + $v1_4) != 1)
                    $v1_5 = i * 0xe;
                else
                {
                    isp_printf(); // Fixed: macro with no parameters, removed 3 arguments;
                    *((int32_t*)((char*)$v0_12 + 4)) = $s2 - *($v0_12 + 6) - *($v0_12 + 4); // Fixed void pointer dereference
                    $v1_5 = i * 0xe;
                }
                
                int32_t $v0_13 = data_ca490_1;
                char* $a0_6 = (char*)($v1_5 + $v0_13); // Fixed void pointer assignment
                char* $v1_11 = (char*)(i * 0xe); // Fixed void pointer assignment
                
                if (*($a0_6 + 0x38) == 1)
                {
                    *((int32_t*)((char*)$a0_6 + 0x3c)) = $s3 - *($a0_6 + 0x3e) - *($a0_6 + 0x3c); // Fixed void pointer dereference
                    $v1_11 = i * 0xe;
                }
                
                char* $v0_14 = (char*)($v1_11 + $v0_13); // Fixed void pointer assignment
                i += 1;
                
                if (*($v0_14 + 0x70) == 1)
                    *((int32_t*)((char*)$v0_14 + 0x74)) = $s5 - *($v0_14 + 0x76) - *($v0_14 + 0x74); // Fixed void pointer dereference
                
                result = 4;
                $v1_4 = i * 0xe;
            } while (i != 4);
        }
    }
    
    return result;
}

