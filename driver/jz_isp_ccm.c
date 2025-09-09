#include "include/main.h"


  int32_t jz_isp_ccm()

{
    int32_t $a1 = data_9a454;
    int32_t ccm_real_1 = ccm_real;
    uint32_t $a1_1 = $a1 >> 0xa;
    int32_t $v0_1;
        int32_t $a0_1 = data_c52ec;
    data_9a450;
    data_c52f8 = 0x64;
    
    if (ccm_real_1 != 1)
    {
        $v0_1 = $a1_1 - $a0_1;
        
        if ($a0_1 >= $a1_1)
            $v0_1 = $a0_1 - $a1_1;
    }
    
    if (ccm_real_1 == 1 || data_c52f0_1 < $v0_1)
    {
        char* cm_ev_list_now_1 = (char*)(cm_ev_list_now); // Fixed void pointer assignment
        char* cm_sat_list_now_1 = (char*)(cm_sat_list_now); // Fixed void pointer assignment
        char* cm_ev_list_now_2 = (char*)(cm_ev_list_now_1); // Fixed void pointer assignment
        int32_t $a3_1 = 0;
        int32_t $v0_3;
            int32_t $t0_1 = *cm_ev_list_now_2;
                    int32_t $a2_1 = *(cm_ev_list_now_1 + ($a3_1 << 2) - 4);
                    char* $v1_3 = (char*)(cm_sat_list_now_1 + ($a3_1 << 2)); // Fixed void pointer assignment
                        int32_t $t1_1 = *(cm_sat_list_now_1 + ($a3_1 << 2) - 4);
                        int32_t $v0_4 = *$v1_3;
                        int32_t $a3_5 = $t0_1 < $a2_1 ? 1 : 0;
                        int32_t $v1_4 = $a2_1 < $a1_1 ? 1 : 0;
                            int32_t $t3_2 = $a1_1 - $a2_1;
                            int32_t $a2_3 = $t0_1 - $a2_1;
        
        while (true)
        {
            
            if ($t0_1 >= $a1_1)
            {
                if ($a3_1)
                {
                    
                    if ($t0_1 != $a2_1)
                    {
                        
                        if ($v0_4 >= $t1_1)
                        {
                            
                            if (!$v1_4)
                                $t3_2 = $a2_1 - $a1_1;
                            
                            
                            if ($a3_5)
                                $a2_3 = $a2_1 - $t0_1;
                            
                            $v0_3 = $t3_2 * ($v0_4 - $t1_1) / $a2_3 + $t1_1;
                        }
                        else
                        {
                            int32_t $a0_5 = $a1_1 - $a2_1;
                            int32_t $a2_2 = $t0_1 - $a2_1;
                            
                            if (!$v1_4)
                                $a0_5 = $a2_1 - $a1_1;
                            
                            
                            if ($a3_5)
                                $a2_2 = $a2_1 - $t0_1;
                            
                            $v0_3 = $t1_1 - $a0_5 * ($t1_1 - $v0_4) / $a2_2;
                        }
                    }
                    else
                        $v0_3 = *$v1_3;
                }
                else
                    $v0_3 = *cm_sat_list_now_1;
                
                break;
            }
            
            $a3_1 += 1;
            cm_ev_list_now_2 += 4;
            
            if ($a3_1 == 9)
            {
                $v0_3 = *(cm_sat_list_now_1 + 0x20);
                break;
            }
        }
        
        data_c52fc_1 = $v0_3;
    }
    
    int32_t $t2_1 = jz_isp_ccm_parameter_convert();
    int32_t $v0_9;
    
    if (ccm_real != 1)
    {
        int32_t $a0_8 = data_c52f4;
        $v0_9 = $t2_1 - $a0_8;
        
        if ($a0_8 >= $t2_1)
            $v0_9 = $a0_8 - $t2_1;
    }
    
    if (ccm_real == 1 || data_c52f8_1 < $v0_9)
        tiziano_ct_ccm_interpolation($t2_1, data_c52f8_2);
    
    void var_34_12;
    cm_control(&ccm_parameter, data_c52fc_2, &var_34_13);
    void var_58_1;
    jz_isp_ccm_para2reg(&var_58_2, &var_34_14);
    tiziano_ccm_lut_parameter(&var_58_4);
    ccm_real = 0;
    return 0;
}

