#include "include/main.h"


  int32_t tiziano_bcsh_update()

{
    char* tisp_BCSH_au32SminListS_now_1 = (char*)(tisp_BCSH_au32SminListS_now); // Fixed void pointer assignment
    char* tisp_BCSH_au32SmaxListS_now_1 = (char*)(tisp_BCSH_au32SmaxListS_now); // Fixed void pointer assignment
    char* tisp_BCSH_au32EvList_now_1 = (char*)(tisp_BCSH_au32EvList_now); // Fixed void pointer assignment
    char* tisp_BCSH_au32SminListM_now_1 = (char*)(tisp_BCSH_au32SminListM_now); // Fixed void pointer assignment
    char* tisp_BCSH_au32SmaxListM_now_1 = (char*)(tisp_BCSH_au32SmaxListM_now); // Fixed void pointer assignment
    char* tisp_BCSH_au32OffsetRGB_now_1 = (char*)(tisp_BCSH_au32OffsetRGB_now); // Fixed void pointer assignment
    uint32_t $v0_1 = data_9a614 >> 0xa;
        char* $a3_1 = (char*)(tisp_BCSH_au32EvList_now_1 + 4); // Fixed void pointer assignment
            int32_t i_2 = 0;
            int32_t i;
                int32_t $ra_1 = *($a3_1 - 4);
                int32_t $t0_1 = i_2 << 2;
                    int32_t $a2_2 = *$a3_1;
                    char* $t4_1 = (char*)($a3_1 - tisp_BCSH_au32EvList_now_1); // Fixed void pointer assignment
                        int32_t $a0_4 = *(tisp_BCSH_au32SminListS_now_1 + $t0_1);
                        int32_t $t3_1 = *(tisp_BCSH_au32SminListS_now_1 + $t4_1);
                        int32_t $a1_2 = $ra_1 < $v0_1 ? 1 : 0;
                        int32_t $t9_1 = $a2_2 < $ra_1 ? 1 : 0;
                        int32_t $v1_19;
                            int32_t $v1_20 = $v0_1 - $ra_1;
                            int32_t $a2_4 = $a2_2 - $ra_1;
    data_9a610;
    
    if (*tisp_BCSH_au32EvList_now_1 < $v0_1)
    {
        
        if ($v0_1 < *(tisp_BCSH_au32EvList_now_1 + 0x20))
        {
            
            do
            {
                i = i_2 + 1;
                
                if ($v0_1 >= $ra_1)
                {
                    
                    if ($a2_2 >= $v0_1)
                    {
                        
                        if ($t3_1 >= $a0_4)
                        {
                            
                            if (!$a1_2)
                                $v1_20 = $ra_1 - $v0_1;
                            
                            
                            if ($t9_1)
                                $a2_4 = $ra_1 - $a2_2;
                            
                            $v1_19 = $v1_20 * ($t3_1 - $a0_4) / $a2_4 + $a0_4;
                        }
                        else
                        {
                            int32_t $v1_17 = $v0_1 - $ra_1;
                            int32_t $a2_3 = $a2_2 - $ra_1;
                            
                            if (!$a1_2)
                                $v1_17 = $ra_1 - $v0_1;
                            
                            
                            if ($t9_1)
                                $a2_3 = $ra_1 - $a2_2;
                            
                            $v1_19 = $a0_4 - $v1_17 * ($a0_4 - $t3_1) / $a2_3;
                        }
                        
                        tisp_BCSH_au32Svalue = $v1_19;
                        int32_t $ra_2 = *(tisp_BCSH_au32SmaxListS_now_1 + $t0_1);
                        int32_t $t3_4 = *(tisp_BCSH_au32SmaxListS_now_1 + $t4_1);
                        int32_t $v1_24 = *($a3_1 - 4);
                        int32_t $s2_1 = *$a3_1;
                        int32_t $v1_26;
                        
                        if ($t3_4 >= $ra_2)
                        {
                            int32_t $a2_5 = $v1_24 - $v0_1;
                            int32_t $v1_27 = $v1_24 - $s2_1;
                            
                            if ($v1_24 < $v0_1)
                                $a2_5 = $v0_1 - $v1_24;
                            
                            
                            if ($s2_1 >= $v1_24)
                                $v1_27 = $s2_1 - $v1_24;
                            
                            $v1_26 = $a2_5 * ($t3_4 - $ra_2) / $v1_27 + $ra_2;
                        }
                        else
                        {
                            int32_t $a1_7 = $v0_1 - $v1_24;
                            int32_t $v1_25 = $s2_1 - $v1_24;
                            
                            if ($v1_24 >= $v0_1)
                                $a1_7 = $v1_24 - $v0_1;
                            
                            
                            if ($s2_1 < $v1_24)
                                $v1_25 = $v1_24 - $s2_1;
                            
                            $v1_26 = $ra_2 - $a1_7 * ($ra_2 - $t3_4) / $v1_25;
                        }
                        
                        data_aa688_1 = $v1_26;
                        int32_t $t9_4 = *(tisp_BCSH_au32SminListM_now_1 + $t0_1);
                        int32_t $t3_10 = *(tisp_BCSH_au32SminListM_now_1 + $t4_1);
                        int32_t $v1_29 = *($a3_1 - 4);
                        int32_t $s1_4 = *$a3_1;
                        int32_t $v1_31;
                        
                        if ($t3_10 >= $t9_4)
                        {
                            int32_t $a2_7 = $v1_29 - $v0_1;
                            int32_t $v1_32 = $v1_29 - $s1_4;
                            
                            if ($v1_29 < $v0_1)
                                $a2_7 = $v0_1 - $v1_29;
                            
                            
                            if ($s1_4 >= $v1_29)
                                $v1_32 = $s1_4 - $v1_29;
                            
                            $v1_31 = $a2_7 * ($t3_10 - $t9_4) / $v1_32 + $t9_4;
                        }
                        else
                        {
                            int32_t $a1_12 = $v0_1 - $v1_29;
                            int32_t $v1_30 = $s1_4 - $v1_29;
                            
                            if ($v1_29 >= $v0_1)
                                $a1_12 = $v1_29 - $v0_1;
                            
                            
                            if ($s1_4 < $v1_29)
                                $v1_30 = $v1_29 - $s1_4;
                            
                            $v1_31 = $t9_4 - $a1_12 * ($t9_4 - $t3_10) / $v1_30;
                        }
                        
                        data_aa68c_1 = $v1_31;
                        int32_t $t7_4 = *(tisp_BCSH_au32SmaxListM_now_1 + $t0_1);
                        int32_t $t4_3 = *(tisp_BCSH_au32SmaxListM_now_1 + $t4_1);
                        int32_t $v1_34 = *($a3_1 - 4);
                        int32_t $t9_5 = *$a3_1;
                        int32_t $v1_36;
                        
                        if ($t4_3 >= $t7_4)
                        {
                            int32_t $t3_16 = $v0_1 - $v1_34;
                            int32_t $v1_37 = $v1_34 - $t9_5;
                            
                            if ($v1_34 >= $v0_1)
                                $t3_16 = $v1_34 - $v0_1;
                            
                            
                            if ($t9_5 >= $v1_34)
                                $v1_37 = $t9_5 - $v1_34;
                            
                            $v1_36 = $t3_16 * ($t4_3 - $t7_4) / $v1_37 + $t7_4;
                        }
                        else
                        {
                            int32_t $a1_17 = $v0_1 - $v1_34;
                            int32_t $v1_35 = $t9_5 - $v1_34;
                            
                            if ($v1_34 >= $v0_1)
                                $a1_17 = $v1_34 - $v0_1;
                            
                            
                            if ($t9_5 < $v1_34)
                                $v1_35 = $v1_34 - $t9_5;
                            
                            $v1_36 = $t7_4 - $a1_17 * ($t7_4 - $t4_3) / $v1_35;
                        }
                        
                        data_aa690_1 = $v1_36;
                        int32_t $t3_18 = (&tisp_BCSH_au32Cxl)[i];
                        int32_t $t6_4 = (&tisp_BCSH_au32Cxl)[i_2];
                        int32_t $v1_39 = *($a3_1 - 4);
                        int32_t $t8_7 = *$a3_1;
                        int32_t $v1_41;
                        
                        if ($t3_18 >= $t6_4)
                        {
                            int32_t $a2_12 = $v0_1 - $v1_39;
                            int32_t $v1_42 = $v1_39 - $t8_7;
                            
                            if ($v1_39 >= $v0_1)
                                $a2_12 = $v1_39 - $v0_1;
                            
                            
                            if ($t8_7 >= $v1_39)
                                $v1_42 = $t8_7 - $v1_39;
                            
                            $v1_41 = $a2_12 * ($t3_18 - $t6_4) / $v1_42 + $t6_4;
                        }
                        else
                        {
                            int32_t $a0_7 = $v0_1 - $v1_39;
                            int32_t $v1_40 = $t8_7 - $v1_39;
                            
                            if ($v1_39 >= $v0_1)
                                $a0_7 = $v1_39 - $v0_1;
                            
                            
                            if ($t8_7 < $v1_39)
                                $v1_40 = $v1_39 - $t8_7;
                            
                            $v1_41 = $t6_4 - $a0_7 * ($t6_4 - $t3_18) / $v1_40;
                        }
                        
                        data_aa7bc_1 = $v1_41;
                        int32_t $t4_11 = (&tisp_BCSH_au32Cxh)[i];
                        int32_t $t7_6 = (&tisp_BCSH_au32Cxh)[i_2];
                        int32_t $v1_44 = *($a3_1 - 4);
                        int32_t $t9_6 = *$a3_1;
                        int32_t $v1_46;
                        
                        if ($t4_11 >= $t7_6)
                        {
                            int32_t $t3_23 = $v0_1 - $v1_44;
                            int32_t $v1_47 = $v1_44 - $t9_6;
                            
                            if ($v1_44 >= $v0_1)
                                $t3_23 = $v1_44 - $v0_1;
                            
                            
                            if ($t9_6 >= $v1_44)
                                $v1_47 = $t9_6 - $v1_44;
                            
                            $v1_46 = $t3_23 * ($t4_11 - $t7_6) / $v1_47 + $t7_6;
                        }
                        else
                        {
                            int32_t $a2_16 = $v0_1 - $v1_44;
                            int32_t $v1_45 = $t9_6 - $v1_44;
                            
                            if ($v1_44 >= $v0_1)
                                $a2_16 = $v1_44 - $v0_1;
                            
                            
                            if ($t9_6 < $v1_44)
                                $v1_45 = $v1_44 - $t9_6;
                            
                            $v1_46 = $t7_6 - $a2_16 * ($t7_6 - $t4_11) / $v1_45;
                        }
                        
                        data_aa7c0_1 = $v1_46;
                        int32_t $t4_16 = (&tisp_BCSH_au32Cyl)[i];
                        int32_t $t7_7 = (&tisp_BCSH_au32Cyl)[i_2];
                        int32_t $v1_49 = *($a3_1 - 4);
                        int32_t $t9_7 = *$a3_1;
                        int32_t $v1_51;
                        
                        if ($t4_16 >= $t7_7)
                        {
                            int32_t $t3_25 = $v0_1 - $v1_49;
                            int32_t $v1_52 = $v1_49 - $t9_7;
                            
                            if ($v1_49 >= $v0_1)
                                $t3_25 = $v1_49 - $v0_1;
                            
                            
                            if ($t9_7 >= $v1_49)
                                $v1_52 = $t9_7 - $v1_49;
                            
                            $v1_51 = $t3_25 * ($t4_16 - $t7_7) / $v1_52 + $t7_7;
                        }
                        else
                        {
                            int32_t $a2_22 = $v0_1 - $v1_49;
                            int32_t $v1_50 = $t9_7 - $v1_49;
                            
                            if ($v1_49 >= $v0_1)
                                $a2_22 = $v1_49 - $v0_1;
                            
                            
                            if ($t9_7 < $v1_49)
                                $v1_50 = $v1_49 - $t9_7;
                            
                            $v1_51 = $t7_7 - $a2_22 * ($t7_7 - $t4_16) / $v1_50;
                        }
                        
                        data_aa7c4_1 = $v1_51;
                        int32_t $a2_26 = (&tisp_BCSH_au32Cyh)[i];
                        int32_t $t0_3 = (&tisp_BCSH_au32Cyh)[i_2];
                        int32_t $v1_53 = *($a3_1 - 4);
                        int32_t $a1_23 = *$a3_1;
                        int32_t $v1_56;
                        
                        if ($a2_26 >= $t0_3)
                        {
                            int32_t $v0_4 = $v1_53 - $v0_1;
                            int32_t $v1_57 = $a1_23 - $v1_53;
                            
                            if ($v1_53 < $v0_1)
                                $v0_4 = $v0_1 - $v1_53;
                            
                            
                            if ($a1_23 < $v1_53)
                                $v1_57 = $v1_53 - $a1_23;
                            
                            $v1_56 = $v0_4 * ($a2_26 - $t0_3) / $v1_57 + $t0_3;
                        }
                        else
                        {
                            int32_t $v0_2 = $v1_53 - $v0_1;
                            int32_t $v1_54 = $a1_23 - $v1_53;
                            
                            if ($v1_53 < $v0_1)
                                $v0_2 = $v0_1 - $v1_53;
                            
                            
                            if ($a1_23 < $v1_53)
                                $v1_54 = $v1_53 - $a1_23;
                            
                            $v1_56 = $t0_3 - $v0_2 * ($t0_3 - $a2_26) / $v1_54;
                        }
                        
                        data_aa7c8_1 = $v1_56;
                        break;
                    }
                }
                
                i_2 = i;
                $a3_1 += 4;
            } while (i != 8);
        }
        else
        {
            tisp_BCSH_au32Svalue = *(tisp_BCSH_au32SminListS_now_1 + 0x20);
            data_aa688 = *(tisp_BCSH_au32SmaxListS_now_1 + 0x20);
            data_aa68c = *(tisp_BCSH_au32SminListM_now_1 + 0x20);
            data_aa690 = *(tisp_BCSH_au32SmaxListM_now_1 + 0x20);
            data_aa7bc = data_aa7b4;
            data_aa7c0 = data_aa790;
            data_aa7c4 = data_aa76c;
            data_aa7c8 = data_aa748;
        }
    }
    else
    {
        tisp_BCSH_au32Svalue = *tisp_BCSH_au32SminListS_now_1;
        data_aa688 = *tisp_BCSH_au32SmaxListS_now_1;
        data_aa68c = *tisp_BCSH_au32SminListM_now_1;
        data_aa690 = *tisp_BCSH_au32SmaxListM_now_1;
        data_aa7bc = tisp_BCSH_au32Cxl;
        data_aa7c0 = tisp_BCSH_au32Cxh;
        data_aa7c4 = tisp_BCSH_au32Cyl;
        data_aa7c8 = tisp_BCSH_au32Cyh;
    }
    
    tiziano_ct_bcsh_interpolation(tiziano_bcsh_Tccm_Comp2Orig());
    tiziano_bcsh_Tccm_RGB2YUV(&tisp_BCSH_au32HMatrix, &tisp_BCSH_as32CCMMatrix);
    tiziano_bcsh_Toffset_RGB2YUV(&tisp_BCSH_u32OffsetRGB2yuv, tisp_BCSH_au32OffsetRGB_now_1);
    tiziano_bcsh_TransitParam();
    
    for (int32_t i_1 = 0; (uintptr_t)i_1 < 0x3c; i_1 += 1)
    {
        char var_50[0x40];
        var_50[i_1] = *(&data_c5400 + i_1);
    }
    
    tiziano_bcsh_lut_parameter(tisp_BCSH, data_c53f4_1, data_c53f8_1, data_c53fc_1);
    BCSH_real = 0;
    return 0;
}

