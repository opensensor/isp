#include "include/main.h"


  int32_t* tiziano_bcsh_TransitParam()

{
    tisp_BCSH = &tisp_BCSH_au32clip;
    data_c5400_1 = &tisp_BCSH_au32Offset1;
    int32_t tisp_BCSH_au32OffsetYUVy_1 = *tisp_BCSH_au32OffsetYUVy;
    data_c53f8_1 = &tisp_BCSH_au32clip2;
    tisp_BCSH_au32Offset0 = tisp_BCSH_au32OffsetYUVy_1;
    data_c53fc_1 = &tisp_BCSH_au32Offset0;
    data_aa670_1 = 0x400;
    data_aa674_1 = 0x400;
    uint32_t $s5 = data_9a91d_1;
    data_c53f4_1 = &tisp_BCSH_au32clip1;
    
    if ($s5 != 0x80)
    {
        int32_t var_48_4_1;
        int32_t $a1_1;
        int32_t $a2_1;
        int32_t $a3_1;
        
        if ($s5 << 0x18 >> 0x18 < 0)
        {
            uint32_t $v0_10 =
                tiziano_bcsh_StrenCal.part.0($s5, 0x80, 0x100, tisp_BCSH_au32Svalue, 0x1800);
            int32_t $a3_3 = data_aa688_1;
            tisp_BCSH_ai32Svalue = $v0_10;
            uint32_t $v0_11 = tiziano_bcsh_StrenCal.part.0($s5, 0x80, 0x100, $a3_3, 0x1800);
            int32_t $a3_4 = data_aa68c_1;
            data_c5454_1 = $v0_11;
            data_c5458_1 = tiziano_bcsh_StrenCal.part.0($s5, 0x80, 0x100, $a3_4, 0x1800);
            $a3_1 = data_aa690_1;
            $a2_1 = 0x100;
            var_48_4_2 = 0x1800;
            $a1_1 = 0x80;
        }
        else
        {
            tisp_BCSH_ai32Svalue =
                tiziano_bcsh_StrenCal.part.0($s5, 0, 0x80, 0, tisp_BCSH_au32Svalue);
            data_c5454_2 = tiziano_bcsh_StrenCal.part.0($s5, 0, 0x80, 0, data_aa688_2);
            data_c5458_2 = tiziano_bcsh_StrenCal.part.0($s5, 0, 0x80, 0, data_aa68c_2);
            $a3_1 = 0;
            var_48_4_3 = data_aa690_2;
            $a2_1 = 0x80;
            $a1_1 = 0;
        }
        
        data_c545c_1 = tiziano_bcsh_StrenCal.part.0($s5, $a1_1, $a2_1, $a3_1, var_48_4_4);
    }
    else
        memcpy(&tisp_BCSH_ai32Svalue, &tisp_BCSH_au32Svalue, 0x10);
    
    uint32_t $s5_1 = data_9a91f_1;
    uint32_t $s7 = ($s5_1 * tisp_BCSH_u32B) >> 7;
    
    if ($s7 >= 0x76d)
        $s7 = 0x76c;
    
    if ($s5_1 << 0x18 >> 0x18 >= 0)
    {
        tisp_BCSH_ai32Svalue =
            tiziano_bcsh_StrenCal.part.0($s5_1, 0, 0x80, 0, tisp_BCSH_ai32Svalue);
        data_c5454_3 = tiziano_bcsh_StrenCal.part.0($s5_1, 0, 0x80, 0, data_c5454_4);
        data_c5458_3 = tiziano_bcsh_StrenCal.part.0($s5_1, 0, 0x80, 0, data_c5458_4);
        data_c545c_2 = tiziano_bcsh_StrenCal.part.0($s5_1, 0, 0x80, 0, data_c545c_3);
    }
    
    **&data_c5400_2 = *(tisp_BCSH_au32OffsetYUVy + 4) + tisp_BCSH_u32OffsetRGB2yuv - 0x800 + $s7;
    *(data_c5400_3 + 4) = data_aa67c_1;
    *(data_c5400_4 + 8) = data_aa680_1;
    int32_t tisp_BCSH_au32C_1 = tisp_BCSH_au32C;
    uint32_t $v1_3;
    
    if (tisp_BCSH_au32C_1)
        $v1_3 = data_9a91e_1;
    
    if (tisp_BCSH_au32C_1 && $v1_3 != 0x80)
    {
        tisp_BCSH_ai32C = tisp_BCSH_au32C_1;
        
        if ($v1_3 < 0)
        {
            int32_t $s4_1 = data_aa7c0_1;
            int32_t $a3_7 = data_aa7bc_1;
            int32_t $t1_3 = $s4_1 - $a3_7;
            
            if ($a3_7 >= $s4_1)
                $t1_3 = $a3_7 - $s4_1;
            
            int32_t $s6_2 = ($t1_3 >> 1) + $a3_7;
            data_c5464_1 = tiziano_bcsh_StrenCal.part.0($v1_3, 0x80, 0xff, $a3_7, $s6_2);
            data_c5468_1 = tiziano_bcsh_StrenCal($v1_3, 0x80, 0xff, $s4_1, $s6_2, 1);
            data_c546c_1 = tiziano_bcsh_StrenCal($v1_3, 0x80, 0xff, data_aa7c4_1, 0, 1);
            data_c5470_1 = tiziano_bcsh_StrenCal.part.0($v1_3, 0x80, 0xff, data_aa7c8_1, 0x3ff);
        }
        else
        {
            int32_t $s7_1 = data_aa7c8_2;
            int32_t $s6_1 = data_aa7c4_2;
            int32_t $t8_1 = $s7_1 - $s6_1;
            
            if ($s6_1 >= $s7_1)
                $t8_1 = $s6_1 - $s7_1;
            
            data_c5464_2 = tiziano_bcsh_StrenCal.part.0($v1_3, 0, 0x80, 0, data_aa7bc_2);
            data_c5468_2 = tiziano_bcsh_StrenCal($v1_3, 0, 0x80, 0x3ff, data_aa7c0_2, 1);
            int32_t $v0_35 = ($t8_1 >> 1) + $s6_1;
            data_c546c_2 = tiziano_bcsh_StrenCal($v1_3, 0, 0x80, $v0_35, $s6_1, 1);
            data_c5470_2 = tiziano_bcsh_StrenCal.part.0($v1_3, 0, 0x80, $v0_35, $s7_1);
            tisp_BCSH_ai32Svalue =
                tiziano_bcsh_StrenCal.part.0($v1_3, 0, 0x80, 0, tisp_BCSH_ai32Svalue);
            data_c5454_5 = tiziano_bcsh_StrenCal.part.0($v1_3, 0, 0x80, 0, data_c5454_6);
            data_c5458_5 = tiziano_bcsh_StrenCal.part.0($v1_3, 0, 0x80, 0, data_c5458_6);
            data_c545c_4 = tiziano_bcsh_StrenCal.part.0($v1_3, 0, 0x80, 0, data_c545c_5);
        }
    }
    else
        memcpy(&tisp_BCSH_ai32C, &tisp_BCSH_au32C, 0x14);
    
    uint32_t s_bcsh_mjpeg_mode_1 = s_bcsh_mjpeg_mode;
    
    if (s_bcsh_mjpeg_mode_1 == 1)
    {
        tisp_BCSH_ai32C = s_bcsh_mjpeg_mode_1;
        uint32_t s_bcsh_mjpeg_y_range_low_1 = s_bcsh_mjpeg_y_range_low;
        data_c546c_3 = 0;
        data_c5464_3 = s_bcsh_mjpeg_y_range_low_1 << 2;
        data_c5468_3 = (data_9a91c_1 << 2) + 3;
        data_c5470_3 = 0x3ff;
    }
    
    data_c5408_1 = &tisp_BCSH_u32Cslope0;
    data_c540c_1 = &tisp_BCSH_u32Cslope1;
    int32_t $a1_3 = data_c5464_4;
    data_c5404_1 = &tisp_BCSH_ai32C;
    data_c5410_1 = &tisp_BCSH_u32Cslope2;
    
    if ($a1_3)
        tisp_BCSH_u32Cslope0 = (data_c546c_4 << 0xa) / $a1_3;
    else
        tisp_BCSH_u32Cslope0 = 0;
    
    int32_t $a2_2 = data_c5468_4;
    
    if ($a1_3 < $a2_2)
    {
        int32_t $v1_5 = data_c5470_4;
        int32_t $a0_27 = data_c546c_5;
        int32_t $v1_6;
        
        $v1_6 = $a0_27 >= $v1_5 ? $a0_27 - $v1_5 : $v1_5 - $a0_27;
        
        tisp_BCSH_u32Cslope1 = ($v1_6 << 0xa) / ($a2_2 - $a1_3);
    }
    else
    {
        data_c5464_5 = $a2_2;
        tisp_BCSH_u32Cslope1 = 0;
    }
    
    int32_t $v1_9 = data_aa6cc_1;
    
    if ($a2_2 < $v1_9)
    {
        int32_t $v0_61 = data_c5470_5;
        int32_t $v0_63;
        
        $v0_63 = $v0_61 >= $v1_9 ? $v0_61 - $v1_9 : $v1_9 - $v0_61;
        
        **&data_c5410_2 = ($v0_63 << 0xa) / ($v1_9 - $a2_2);
    }
    else
    {
        int32_t* $v0_62 = data_c5410_3;
        data_c5468_5 = $v1_9;
        *$v0_62 = 0;
    }
    
    void* tisp_BCSH_au32Sthres_now_1 = tisp_BCSH_au32Sthres_now;
    data_c5414_1 = tisp_BCSH_au32Sthres_now_1;
    data_c5418_1 = &tisp_BCSH_ai32Svalue;
    data_c541c_1 = &tisp_BCSH_u32Sstep;
    int32_t $t0 = *(tisp_BCSH_au32Sthres_now_1 + 4);
    int32_t $v0_66 = *(tisp_BCSH_au32Sthres_now_1 + 8);
    
    if ($t0 < $v0_66)
    {
        int32_t $v1_11 = data_c5454_7;
        int32_t tisp_BCSH_ai32Svalue_1 = tisp_BCSH_ai32Svalue;
        int32_t $v0_69 = $v0_66 - $t0;
        int32_t $v1_12 = $v1_11 - tisp_BCSH_ai32Svalue_1;
        
        if (tisp_BCSH_ai32Svalue_1 >= $v1_11)
            $v1_12 = tisp_BCSH_ai32Svalue_1 - $v1_11;
        
        int32_t $a1_7 = data_c545c_6;
        *tisp_BCSH_u32Sstep = $v1_12 / $v0_69;
        int32_t $a2_3 = data_c5458_7;
        int32_t $v1_14 = $a2_3 - $a1_7;
        
        if ($a2_3 < $a1_7)
            $v1_14 = $a1_7 - $a2_3;
        
        *(tisp_BCSH_u32Sstep + 4) = $v1_14 / $v0_69;
    }
    else
    {
        *(tisp_BCSH_au32Sthres_now_1 + 4) = $v0_66;
        **&data_c541c_2 = 0;
        *(data_c541c_3 + 4) = 0;
    }
    
    data_c5420_1 = &tisp_BCSH_au32HMatrix;
    void* tisp_BCSH_au32HDP_now_1 = tisp_BCSH_au32HDP_now;
    data_c542c_1 = tisp_BCSH_au32HBP_now;
    void* tisp_BCSH_au32HLSP_now_1 = tisp_BCSH_au32HLSP_now;
    data_c5424_1 = tisp_BCSH_au32HDP_now_1;
    data_c5434_1 = tisp_BCSH_au32HLSP_now_1;
    data_c5428_1 = &tisp_BCSH_u32HDPslope;
    data_c5430_1 = &tisp_BCSH_u32HBPslope;
    data_c5438_1 = &awb_array_r;
    int32_t $a1_9 = *(tisp_BCSH_au32HDP_now_1 + 4);
    int32_t $v0_72 = *(tisp_BCSH_au32HDP_now_1 + 8);
    
    if ($a1_9 < $v0_72)
        tisp_BCSH_u32HDPslope = 0x400 / ($v0_72 - $a1_9);
    else
    {
        *(tisp_BCSH_au32HDP_now_1 + 4) = $v0_72;
        **&data_c5428_2 = 0;
    }
    
    void* $v1_15 = data_c542c_2;
    int32_t $a0_32 = *($v1_15 + 4);
    int32_t $v0_76 = *($v1_15 + 8);
    
    if ($a0_32 < $v0_76)
        **&data_c5430_2 = 0x400 / ($v0_76 - $a0_32);
    else
    {
        *($v1_15 + 4) = $v0_76;
        **&data_c5430_3 = 0;
    }
    
    void* $v1_16 = data_c5434_2;
    int32_t $a0_33 = *($v1_16 + 4);
    int32_t $v0_80 = *($v1_16 + 8);
    int32_t* result;
    
    if ($a0_33 < $v0_80)
    {
        result = 0x400 / ($v0_80 - $a0_33);
        **&data_c5438_2 = result;
    }
    else
    {
        *($v1_16 + 4) = $v0_80;
        result = data_c5438_3;
        *result = 0;
    }
    
    return result;
}

