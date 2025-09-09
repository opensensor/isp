#include "include/main.h"


  int32_t tisp_ae0_get_hist(int32_t* arg1, int32_t arg2, int32_t arg3)

{
    void* $s5 = &IspAeStatic;
    void* $v0 = &IspAeStatic;
    
    do
    {
        if (arg2 == 1)
        {
            *(((void**)((char*)$v0 + 0x1518))) = *arg1 & 0x1fffff; // Fixed void pointer dereference
            *(((void**)((char*)$v0 + 0x151c))) = arg1[1] & 0x1fffff; // Fixed void pointer dereference
            
            if (arg3 != arg2)
                arg1 = &arg1[2];
            else
            {
                *(((void**)((char*)$v0 + 0x1918))) = arg1[0x100] & 0x1fffff; // Fixed void pointer dereference
                *(((void**)((char*)$v0 + 0x191c))) = arg1[0x101] & 0x1fffff; // Fixed void pointer dereference
                arg1 = &arg1[2];
            }
        }
        else if (arg3 != 1)
            arg1 = &arg1[2];
        else
        {
            *(((void**)((char*)$v0 + 0x1918))) = *arg1 & 0x1fffff; // Fixed void pointer dereference
            *(((void**)((char*)$v0 + 0x191c))) = arg1[1] & 0x1fffff; // Fixed void pointer dereference
            arg1 = &arg1[2];
        }
        
        $v0 += 8;
    } while (arg1 != &arg1[0x100]);
    
    memcpy(&tisp_ae_hist, &data_d1a0c, 0x400);
    int32_t var_40_20;
    __private_spin_lock_irqsave(0, &var_40_21);
    memcpy(&data_d4fbc_1, 0xd4b90, 0x10);
    private_spin_unlock_irqrestore(0, var_40_22);
    int32_t $a0 = data_d4fbc_2;
    data_d4fa8_1 = 0;
    int32_t $a1_2 = 0;
    int32_t $v0_1 = 0;
    int32_t $v1 = 0;
    
    while (true)
    {
        $s5 += 4;
        
        if ($v1 == $a0)
            break;
        
        $v1 += 1;
        $v0_1 += *($s5 + 0x1514);
        $a1_2 = 1;
    }
    
    if ($a1_2)
        __builtin_memset(&data_d4fa8_2, 0, 0x14);
    
    int32_t $v0_2 = data_d4fa8_3;
    int32_t $v1_1 = data_d4fc0_1;
    int32_t $a2 = 0;
    int32_t $a1_4 = 0;
    
    while ($a0 < $v1_1)
    {
        int32_t $a2_3 = *(($a0 << 2) + &data_d1a0c);
        $a0 += 1;
        $a1_4 += $a2_3;
        $a2 = 1;
    }
    
    if ($a2)
        data_d4fac_1 = $a1_4;
    
    int32_t $a2_4 = 0;
    int32_t $v0_3 = $v0_2 + data_d4fac_2;
    int32_t $a0_2 = data_d4fc4_1;
    int32_t $a1_5 = 0;
    
    while ($v1_1 < $a0_2)
    {
        int32_t $a2_7 = *(($v1_1 << 2) + &data_d1a0c);
        $v1_1 += 1;
        $a1_5 += $a2_7;
        $a2_4 = 1;
    }
    
    if ($a2_4)
        data_d4fb0_1 = $a1_5;
    
    int32_t $a2_8 = 0;
    int32_t $v0_4 = $v0_3 + data_d4fb0_2;
    int32_t i = data_d4fc8_1;
    int32_t $a1_6 = 0;
    
    while ($a0_2 < i)
    {
        int32_t $a2_11 = *(($a0_2 << 2) + &data_d1a0c);
        $a0_2 += 1;
        $a1_6 += $a2_11;
        $a2_8 = 1;
    }
    
    if ($a2_8)
        data_d4fb4_1 = $a1_6;
    
    int32_t $a1_7 = 0;
    int32_t $v0_5 = $v0_4 + data_d4fb4_2;
    int32_t $a0_4 = 0;
    
    while ((uintptr_t)i < 0x100)
    {
        int32_t $a1_10 = *((i << 2) + &data_d1a0c);
        i += 1;
        $a0_4 += $a1_10;
        $a1_7 = 1;
    }
    
    if ($a1_7)
        data_d4fb8_1 = $a0_4;
    
    int32_t $s5_2 = $v0_5 + data_d4fb8_2;
    
    if ($s5_2)
    {
            int32_t var_48_1 = 0xffff;
            int32_t var_44_1 = 0;
        for (int32_t* i_1 = &tisp_ae_hist; (uintptr_t)i_1 != 0xd4bb8; i_1 = &i_1[1])
        {
            int32_t $v0_7;
            int32_t $v1_3;
            int32_t $a1_12;
            $v0_7 = (&data_20000 + 0x918)(0);
            i_1[0x101] = fix_point_div(0, $a1_12, $v0_7, $v1_3, $s5_2, 0);
        }
        
        data_d4fa8_4 = 0xffff - data_d4fb8_3 - (data_d4fac_3 + data_d4fb0_3 + data_d4fb4_3);
    }
    else
    {
        data_d4fa8 = 0xffff;
        data_d4fac = 0;
        data_d4fb0 = 0;
        data_d4fb4 = 0;
        data_d4fb8 = 0;
    }
    
    __private_spin_lock_irqsave(0, &var_40_23);
    memcpy(&tisp_ae_hist_last, &tisp_ae_hist, 0x400);
    memcpy(0xd4b7c, &data_d4fa8_5, 0x14);
    return private_spin_unlock_irqrestore(0, var_40_24);
}

