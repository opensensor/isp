#include "include/main.h"


  int32_t tiziano_wdr_init(uint32_t arg1, uint32_t arg2)

{
    void var_b0_67;
    memcpy(&var_b0_68, 0x7da3c, 0x28);
    uint32_t $lo = (arg2 + 5) / 0xa;
    width_wdr_def = arg1;
    height_wdr_def = arg2;
    uint32_t $s7 = (arg1 + 8) >> 4;
    int32_t $s2 = 0;
    int32_t $s3 = $lo - ($lo & 1);
    int32_t $fp = $s3 << 1;
    int32_t $s4 = $s3 + $fp;
    int32_t $v0 = $s3 + $s4;
    int32_t $v0_1 = $s3 + $v0;
    int32_t $v0_2 = $s3 + $v0_1;
    int32_t $v0_3 = $s3 + $v0_2;
    int32_t $v0_4 = $s3 + $v0_3;
    int32_t $v0_5 = $s3 + $v0_4;
    int32_t $s7_1 = $s7 - ($s7 & 1);
    int32_t $v0_7 = $s7_1 << 1;
    int32_t $v0_8 = $s7_1 + $v0_7;
    int32_t $v0_9 = $s7_1 + $v0_8;
    int32_t $v0_10 = $s7_1 + $v0_9;
    int32_t $v0_11 = $s7_1 + $v0_10;
    int32_t $v0_12 = $s7_1 + $v0_11;
    int32_t $v0_13 = $s7_1 + $v0_12;
    int32_t $v0_14 = $s7_1 + $v0_13;
    int32_t $v0_15 = $s7_1 + $v0_14;
    int32_t $v0_16 = $s7_1 + $v0_15;
    int32_t $v0_17 = $s7_1 + $v0_16;
    int32_t $v0_18 = $s7_1 + $v0_17;
    int32_t $v0_19 = $s7_1 + $v0_18;
    int32_t $t0 = $s7_1 * $s3;
    int32_t $v0_20 = $s7_1 + $v0_19;
    int32_t $v1 = arg2 - $v0_5;
    int32_t $v0_23 = arg1 - $v0_20;
    void* $a1_1 = &var_b0_69;
    int32_t $a0_2;
    int32_t $s2_1;
    
    while (true)
    {
        int32_t $a0_1 = *$a1_1;
        
        if (($t0 >> 2) + 1 < $a0_1 << 8)
        {
            $s2_1 = $s2 + 0x12;
            $a0_2 = $a0_1 << 0x12;
            break;
        }
        
        $s2 += 1;
        $a1_1 += 4;
        
        if ($s2 == 0xa)
        {
            $a0_2 = 0;
            $s2_1 = 0;
            break;
        }
    }
    
    tiziano_wdr_params_refresh();
    uint32_t $a0_3 = ($s7_1 + 1) >> 1;
    uint32_t $v0_30 = ($s3 + 1) >> 1;
    uint32_t $v0_31;
    
    $v0_31 = $a0_3 >= $v0_30 ? $v0_30 * 3 : $a0_3 * 3;
    
    data_b1580_2 = ($v0_31 + 1) >> 1;
    tiziano_wdr_5x5_param();
    system_reg_write(0x23ac, 
        (data_b21fc_1 & 0x1f) << 8 | (data_b2200_1 & 0x1f) << 0x10
            | (param_wdr_weightLUT20_array_def & 0x1f) | (data_b2204_1 & 0x1f) << 0x18);
    system_reg_write(0x23b0, 
        (data_b220c_1 & 0x1f) << 8 | (data_b2210_1 & 0x1f) << 0x10 | (data_b2208_1 & 0x1f)
            | (data_b2214_1 & 0x1f) << 0x18);
    system_reg_write(0x23b4, 
        (data_b221c_1 & 0x1f) << 8 | (data_b2220_1 & 0x1f) << 0x10 | (data_b2218_1 & 0x1f)
            | (data_b2224_1 & 0x1f) << 0x18);
    system_reg_write(0x23b8, 
        (data_b222c_1 & 0x1f) << 8 | (data_b2230_1 & 0x1f) << 0x10 | (data_b2228_1 & 0x1f)
            | (data_b2234_1 & 0x1f) << 0x18);
    system_reg_write(0x23bc, 
        (data_b223c_1 & 0x1f) << 8 | (data_b2240_1 & 0x1f) << 0x10 | (data_b2238_1 & 0x1f)
            | (data_b2244_1 & 0x1f) << 0x18);
    system_reg_write(0x23c0, 
        (data_b224c_1 & 0x1f) << 8 | (data_b2250_1 & 0x1f) << 0x10 | (data_b2248_1 & 0x1f)
            | (data_b2254_1 & 0x1f) << 0x18);
    system_reg_write(0x23c4, 
        (data_b225c_1 & 0x1f) << 8 | (data_b2260_1 & 0x1f) << 0x10 | (data_b2258_1 & 0x1f)
            | (data_b2264_1 & 0x1f) << 0x18);
    system_reg_write(0x23c8, 
        (data_b226c_1 & 0x1f) << 8 | (data_b2270_1 & 0x1f) << 0x10 | (data_b2268_1 & 0x1f)
            | (data_b2274_1 & 0x1f) << 0x18);
    system_reg_write(0x23cc, 
        (data_b217c_1 & 0x1f) << 8 | (data_b2180_1 & 0x1f) << 0x10
            | (param_wdr_weightLUT02_array_def & 0x1f) | (data_b2184_1 & 0x1f) << 0x18);
    system_reg_write(0x23d0, 
        (data_b218c_1 & 0x1f) << 8 | (data_b2190_1 & 0x1f) << 0x10 | (data_b2188_1 & 0x1f)
            | (data_b2194_1 & 0x1f) << 0x18);
    system_reg_write(0x23d4, 
        (data_b219c_1 & 0x1f) << 8 | (data_b21a0_1 & 0x1f) << 0x10 | (data_b2198_1 & 0x1f)
            | (data_b21a4_1 & 0x1f) << 0x18);
    system_reg_write(0x23d8, 
        (data_b21ac_1 & 0x1f) << 8 | (data_b21b0_1 & 0x1f) << 0x10 | (data_b21a8_1 & 0x1f)
            | (data_b21b4_1 & 0x1f) << 0x18);
    system_reg_write(0x23dc, 
        (data_b21bc_1 & 0x1f) << 8 | (data_b21c0_1 & 0x1f) << 0x10 | (data_b21b8_1 & 0x1f)
            | (data_b21c4_1 & 0x1f) << 0x18);
    system_reg_write(0x23e0, 
        (data_b21cc_1 & 0x1f) << 8 | (data_b21d0_1 & 0x1f) << 0x10 | (data_b21c8_1 & 0x1f)
            | (data_b21d4_1 & 0x1f) << 0x18);
    system_reg_write(0x23e4, 
        (data_b21dc_1 & 0x1f) << 8 | (data_b21e0_1 & 0x1f) << 0x10 | (data_b21d8_1 & 0x1f)
            | (data_b21e4_1 & 0x1f) << 0x18);
    system_reg_write(0x23e8, 
        (data_b21ec_1 & 0x1f) << 8 | (data_b21f0_1 & 0x1f) << 0x10 | (data_b21e8_1 & 0x1f)
            | (data_b21f4_1 & 0x1f) << 0x18);
    system_reg_write(0x23ec, 
        (data_b20fc_1 & 0x1f) << 8 | (data_b2100_1 & 0x1f) << 0x10
            | (param_wdr_weightLUT12_array_def & 0x1f) | (data_b2104_1 & 0x1f) << 0x18);
    system_reg_write(0x23f0, 
        (data_b210c_1 & 0x1f) << 8 | (data_b2110_1 & 0x1f) << 0x10 | (data_b2108_1 & 0x1f)
            | (data_b2114_1 & 0x1f) << 0x18);
    system_reg_write(0x23f4, 
        (data_b211c_1 & 0x1f) << 8 | (data_b2120_1 & 0x1f) << 0x10 | (data_b2118_1 & 0x1f)
            | (data_b2124_1 & 0x1f) << 0x18);
    system_reg_write(0x23f8, 
        (data_b212c_1 & 0x1f) << 8 | (data_b2130_1 & 0x1f) << 0x10 | (data_b2128_1 & 0x1f)
            | (data_b2134_1 & 0x1f) << 0x18);
    system_reg_write(0x23fc, 
        (data_b213c_1 & 0x1f) << 8 | (data_b2140_1 & 0x1f) << 0x10 | (data_b2138_1 & 0x1f)
            | (data_b2144_1 & 0x1f) << 0x18);
    system_reg_write(0x2400, 
        (data_b214c_1 & 0x1f) << 8 | (data_b2150_1 & 0x1f) << 0x10 | (data_b2148_1 & 0x1f)
            | (data_b2154_1 & 0x1f) << 0x18);
    system_reg_write(0x2404, 
        (data_b215c_1 & 0x1f) << 8 | (data_b2160_1 & 0x1f) << 0x10 | (data_b2158_1 & 0x1f)
            | (data_b2164_1 & 0x1f) << 0x18);
    system_reg_write(0x2408, 
        (data_b216c_1 & 0x1f) << 8 | (data_b2170_1 & 0x1f) << 0x10 | (data_b2168_1 & 0x1f)
            | (data_b2174_1 & 0x1f) << 0x18);
    system_reg_write(0x240c, 
        (data_d8f0c_1 & 0x1f) << 8 | (data_d8f10_1 & 0x1f) << 0x10
            | (param_wdr_weightLUT22_array_def & 0x1f) | (data_d8f14_1 & 0x1f) << 0x18);
    system_reg_write(0x2410, 
        (data_d8f1c_1 & 0x1f) << 8 | (data_d8f20_1 & 0x1f) << 0x10 | (data_d8f18_1 & 0x1f)
            | (data_d8f24_1 & 0x1f) << 0x18);
    system_reg_write(0x2414, 
        (data_d8f2c_1 & 0x1f) << 8 | (data_d8f30_1 & 0x1f) << 0x10 | (data_d8f28_1 & 0x1f)
            | (data_d8f34_1 & 0x1f) << 0x18);
    system_reg_write(0x2418, 
        (data_d8f3c_1 & 0x1f) << 8 | (data_d8f40_1 & 0x1f) << 0x10 | (data_d8f38_1 & 0x1f)
            | (data_d8f44_1 & 0x1f) << 0x18);
    system_reg_write(0x241c, 
        (data_d8f4c_1 & 0x1f) << 8 | (data_d8f50_1 & 0x1f) << 0x10 | (data_d8f48_1 & 0x1f)
            | (data_d8f54_1 & 0x1f) << 0x18);
    system_reg_write(0x2420, 
        (data_d8f5c_1 & 0x1f) << 8 | (data_d8f60_1 & 0x1f) << 0x10 | (data_d8f58_1 & 0x1f)
            | (data_d8f64_1 & 0x1f) << 0x18);
    system_reg_write(0x2424, 
        (data_d8f6c_1 & 0x1f) << 8 | (data_d8f70_1 & 0x1f) << 0x10 | (data_d8f68_1 & 0x1f)
            | (data_d8f74_1 & 0x1f) << 0x18);
    system_reg_write(0x2428, 
        (data_d8f7c_1 & 0x1f) << 8 | (data_d8f80_1 & 0x1f) << 0x10 | (data_d8f78_1 & 0x1f)
            | (data_d8f84_1 & 0x1f) << 0x18);
    system_reg_write(0x242c, 
        (data_b207c_1 & 0x1f) << 8 | (data_b2080_1 & 0x1f) << 0x10
            | (param_wdr_weightLUT21_array_def & 0x1f) | (data_b2084_1 & 0x1f) << 0x18);
    system_reg_write(0x2430, 
        (data_b208c_1 & 0x1f) << 8 | (data_b2090_1 & 0x1f) << 0x10 | (data_b2088_1 & 0x1f)
            | (data_b2094_1 & 0x1f) << 0x18);
    system_reg_write(0x2434, 
        (data_b209c_1 & 0x1f) << 8 | (data_b20a0_1 & 0x1f) << 0x10 | (data_b2098_1 & 0x1f)
            | (data_b20a4_1 & 0x1f) << 0x18);
    system_reg_write(0x2438, 
        (data_b20ac_1 & 0x1f) << 8 | (data_b20b0_1 & 0x1f) << 0x10 | (data_b20a8_1 & 0x1f)
            | (data_b20b4_1 & 0x1f) << 0x18);
    system_reg_write(0x243c, 
        (data_b20bc_1 & 0x1f) << 8 | (data_b20c0_1 & 0x1f) << 0x10 | (data_b20b8_1 & 0x1f)
            | (data_b20c4_1 & 0x1f) << 0x18);
    system_reg_write(0x2440, 
        (data_b20cc_1 & 0x1f) << 8 | (data_b20d0_1 & 0x1f) << 0x10 | (data_b20c8_1 & 0x1f)
            | (data_b20d4_1 & 0x1f) << 0x18);
    system_reg_write(0x2444, 
        (data_b20dc_1 & 0x1f) << 8 | (data_b20e0_1 & 0x1f) << 0x10 | (data_b20d8_1 & 0x1f)
            | (data_b20e4_1 & 0x1f) << 0x18);
    system_reg_write(0x2448, 
        (data_b20ec_1 & 0x1f) << 8 | (data_b20f0_1 & 0x1f) << 0x10 | (data_b20e8_1 & 0x1f)
            | (data_b20f4_1 & 0x1f) << 0x18);
    system_reg_write(0x2500, param_centre5x5_w_distance_array_def | data_b2000_1 << 0x10);
    system_reg_write(0x2504, data_b2004_1 | data_b2008_1 << 0x10);
    system_reg_write(0x2508, data_b200c_1 | data_b2010_1 << 0x10);
    system_reg_write(0x250c, data_b2014_1 | data_b2018_1 << 0x10);
    system_reg_write(0x2510, data_b201c_1 | data_b2020_1 << 0x10);
    system_reg_write(0x2514, data_b2024_1 | data_b2028_1 << 0x10);
    system_reg_write(0x2518, data_b202c_1 | data_b2030_1 << 0x10);
    system_reg_write(0x251c, data_b2034_1 | data_b2038_1 << 0x10);
    system_reg_write(0x2520, data_b203c_1 | data_b2040_1 << 0x10);
    system_reg_write(0x2524, data_b2044_1 | data_b2048_1 << 0x10);
    system_reg_write(0x2528, data_b204c_1 | data_b2050_1 << 0x10);
    system_reg_write(0x252c, data_b2054_1 | data_b2058_1 << 0x10);
    system_reg_write(0x2530, data_b205c_1 | data_b2060_1 << 0x10);
    system_reg_write(0x2534, data_b2064_1[0] | data_b2064_2[1] << 0x10);
    system_reg_write(0x2538, data_b2064_3[2] | data_b2064_4[3] << 0x10);
    system_reg_write(0x253c, data_b2074_1);
    tiziano_wdr_params_init();
    tiziano_wdr_soft_para_out();
    system_reg_write(0x2370, ($s3 & 0xfff) << 0x10);
    system_reg_write(0x2374, ($s4 & 0xfff) << 0x10 | ($fp & 0xfff));
    system_reg_write(0x2378, ($v0_1 & 0xfff) << 0x10 | ($v0 & 0xfff));
    system_reg_write(0x237c, ($v0_3 & 0xfff) << 0x10 | ($v0_2 & 0xfff));
    system_reg_write(0x2380, ($v0_5 & 0xfff) << 0x10 | ($v0_4 & 0xfff));
    system_reg_write(0x2384, arg2 & 0xfff);
    system_reg_write(0x2388, ($s7_1 & 0xfff) << 0x10);
    system_reg_write(0x238c, ($v0_8 & 0xfff) << 0x10 | ($v0_7 & 0xfff));
    system_reg_write(0x2390, ($v0_10 & 0xfff) << 0x10 | ($v0_9 & 0xfff));
    system_reg_write(0x2394, ($v0_12 & 0xfff) << 0x10 | ($v0_11 & 0xfff));
    system_reg_write(0x2398, ($v0_14 & 0xfff) << 0x10 | ($v0_13 & 0xfff));
    system_reg_write(0x239c, ($v0_16 & 0xfff) << 0x10 | ($v0_15 & 0xfff));
    system_reg_write(0x23a0, ($v0_18 & 0xfff) << 0x10 | ($v0_17 & 0xfff));
    system_reg_write(0x23a4, ($v0_20 & 0xfff) << 0x10 | ($v0_19 & 0xfff));
    system_reg_write(0x23a8, arg1 & 0xfff);
    int32_t $a1_232 =
        ((((($s7_1 * $v1 + 2) >> 3) + $a0_2) / (($s7_1 * $v1 + 2) >> 2)) & 0xfff) << 0x10
        | ((((($t0 + 2) >> 3) + $a0_2) / (($t0 + 2) >> 2)) & 0xfff);
    system_reg_write(0x2640, $a1_232);
    int32_t $a1_234 =
        ((((($v1 * $v0_23 + 2) >> 3) + $a0_2) / (($v1 * $v0_23 + 2) >> 2)) & 0xfff) << 0x10
        | ((((($s3 * $v0_23 + 2) >> 3) + $a0_2) / (($s3 * $v0_23 + 2) >> 2)) & 0xfff);
    system_reg_write(0x2644, $a1_234);
    system_reg_write(0x2648, $s2_1);
    system_irq_func_set(0xb, tiziano_wdr_interrupt_static);
    tisp_event_set_cb(0xb, tisp_wdr_process);
    return 0;
}

