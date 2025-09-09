#include "include/main.h"


  int32_t tisp_dpc_d_m2_par_cfg()

{
    uint32_t dpc_d_m2_level_intp_1 = dpc_d_m2_level_intp;
    int32_t $v0;
    int32_t $a1;
    int32_t $a3_1;
    int32_t $t0;
    
    if (!dpc_d_m2_level_intp_1)
    {
        $a3_1 = 0;
        $t0 = 0;
        $a1 = 0;
        $v0 = 0;
    }
    else
    {
        if (dpc_d_m2_level_intp_1 == 1)
        {
            $a3_1 = 0;
            $t0 = 0;
            $a1 = 0;
        }
        else
        {
            if (dpc_d_m2_level_intp_1 == 2)
            {
                $a3_1 = 0;
                $t0 = 0;
            }
            else
            {
                $a3_1 = 0 < (dpc_d_m2_level_intp_1 ^ 3) ? 1 : 0;
                $t0 = 1;
            }
            
            $a1 = 1;
        }
        
        $v0 = 1;
    }
    
    int32_t $a2_8 = $a1 << 1 | $t0 << 2 | $v0 | $a3_1 << 3 | $a1 << 4 | $t0 << 5 | $a3_1 << 6
        | $t0 << 8 | $a3_1 << 9;
    int32_t $v1_10 = $a2_8 | $a3_1 << 0xc | $v0 << 0x10 | $a1 << 0x11 | $t0 << 0x12 | $a3_1 << 0x13
        | $a1 << 0x14 | $t0 << 0x15 | $a3_1 << 0x16;
    system_reg_write(0x280c, $v1_10 | $t0 << 0x18 | $a3_1 << 0x19 | $a3_1 << 0x1c);
    system_reg_write(0x283c, dpc_d_m2_hthres_intp << 0x10 | dpc_d_m2_lthres_intp);
    int32_t $a1_6 = data_cb85c_1;
    int32_t $v0_9 = dpc_d_m2_hthres_intp - $a1_6;
    int32_t $a0_10 = dpc_d_m2_lthres_intp - $a1_6;
    
    if ($v0_9 < 0)
        $v0_9 = 0;
    
    if ($a0_10 < 0)
        $a0_10 = 0;
    
    system_reg_write(0x2820, $v0_9 << 0x10 | $a0_10);
    system_reg_write(0x2840, dpc_d_m2_p1_d1_thres_intp << 0x10 | dpc_d_m2_p0_d1_thres_intp);
    int32_t $a1_11 = data_cb85c_2;
    int32_t $v0_14 = dpc_d_m2_p1_d1_thres_intp - $a1_11;
    int32_t $a0_12 = dpc_d_m2_p0_d1_thres_intp - $a1_11;
    
    if ($v0_14 < 0)
        $v0_14 = 0;
    
    if ($a0_12 < 0)
        $a0_12 = 0;
    
    system_reg_write(0x2824, $v0_14 << 0x10 | $a0_12);
    system_reg_write(0x2844, dpc_d_m2_p3_d1_thres_intp << 0x10 | dpc_d_m2_p2_d1_thres_intp);
    int32_t $a1_16 = data_cb85c_3;
    int32_t $v0_19 = dpc_d_m2_p3_d1_thres_intp - $a1_16;
    int32_t $a0_14 = dpc_d_m2_p2_d1_thres_intp - $a1_16;
    
    if ($v0_19 < 0)
        $v0_19 = 0;
    
    if ($a0_14 < 0)
        $a0_14 = 0;
    
    system_reg_write(0x2828, $v0_19 << 0x10 | $a0_14);
    system_reg_write(0x2848, dpc_d_m2_p1_d2_thres_intp << 0x10 | dpc_d_m2_p0_d2_thres_intp);
    int32_t $a1_21 = data_cb85c_4;
    int32_t $v0_24 = dpc_d_m2_p1_d2_thres_intp - $a1_21;
    int32_t $a0_16 = dpc_d_m2_p0_d2_thres_intp - $a1_21;
    
    if ($v0_24 < 0)
        $v0_24 = 0;
    
    if ($a0_16 < 0)
        $a0_16 = 0;
    
    system_reg_write(0x282c, $v0_24 << 0x10 | $a0_16);
    system_reg_write(0x284c, dpc_d_m2_p3_d2_thres_intp << 0x10 | dpc_d_m2_p2_d2_thres_intp);
    int32_t $a1_26 = data_cb85c_5;
    int32_t $v0_29 = dpc_d_m2_p3_d2_thres_intp - $a1_26;
    int32_t $a0_18 = dpc_d_m2_p2_d2_thres_intp - $a1_26;
    
    if ($v0_29 < 0)
        $v0_29 = 0;
    
    if ($a0_18 < 0)
        $a0_18 = 0;
    
    system_reg_write(0x2830, $v0_29 << 0x10 | $a0_18);
    system_reg_write(0x2808, 
        dpc_d_m2_con_par_array << 8 | dpc_d_m3_con_par_array << 0x10 | dpc_d_m1_con_par_array
            | data_cb850_1 << 0x18);
    system_reg_write(0x2818, data_cb858_1 << 0x10 | data_cb854_1);
    return 0;
}

