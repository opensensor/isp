#include "include/main.h"


  int32_t* tiziano_bcsh_TransitParam()

{
    int32_t tisp_BCSH_au32OffsetYUVy_1 = *tisp_BCSH_au32OffsetYUVy;
    uint32_t $s5 = data_9a91d;
        int32_t var_48_4;
        int32_t $a1_1;
        int32_t $a2_1;
        int32_t $a3_1;
            uint32_t $v0_10 =
            int32_t $a3_3 = data_aa688;
            uint32_t $v0_11 = tiziano_bcsh_StrenCal.part.0($s5, 0x80, 0x100, $a3_3, 0x1800);
            int32_t $a3_4 = data_aa68c;
    tisp_BCSH = &tisp_BCSH_au32clip;
    data_c5400 = &tisp_BCSH_au32Offset1;
    data_c53f8 = &tisp_BCSH_au32clip2;
    tisp_BCSH_au32Offset0 = tisp_BCSH_au32OffsetYUVy_1;
    data_c53fc = &tisp_BCSH_au32Offset0;
    data_aa670 = 0x400;
    data_aa674 = 0x400;
    data_c53f4 = &tisp_BCSH_au32clip1;
    
    if ($(uintptr_t)s5 != 0x80)
    {
        
        if ($s5 << 0x18 >> 0x18 < 0)
        {
                tiziano_bcsh_StrenCal.part.0($s5, 0x80, 0x100, tisp_BCSH_au32Svalue, 0x1800);
            tisp_BCSH_ai32Svalue = $v0_10;
            data_c5454 = $v0_11;
            data_c5458 = tiziano_bcsh_StrenCal.part.0($s5, 0x80, 0x100, $a3_4, 0x1800);
            $a3_1 = data_aa690;
            $a2_1 = 0x100;
            var_48_4 = 0x1800;
            $a1_1 = 0x80;
        }
        else
        {
            tisp_BCSH_ai32Svalue =
                tiziano_bcsh_StrenCal.part.0($s5, 0, 0x80, 0, tisp_BCSH_au32Svalue);
            data_c5454 = tiziano_bcsh_StrenCal.part.0($s5, 0, 0x80, 0, data_aa688);
            data_c5458 = tiziano_bcsh_StrenCal.part.0($s5, 0, 0x80, 0, data_aa68c);
            $a3_1 = 0;
            var_48_4 = data_aa690;
            $a2_1 = 0x80;
            $a1_1 = 0;
        }
        
        data_c545c_1 = tiziano_bcsh_StrenCal.part.0($s5, $a1_1, $a2_1, $a3_1, var_48_4_1);
    }
    else
        memcpy(&tisp_BCSH_ai32Svalue, &tisp_BCSH_au32Svalue, 0x10);
    
    uint32_t $s5_1 = data_9a91f_1;
    uint32_t $s7 = ($s5_1 * tisp_BCSH_u32B) >> 7;
    
    if ($(uintptr_t)s7 >= 0x76d)
        $s7 = 0x76c;
    
    if ($s5_1 << 0x18 >> 0x18 >= 0)
    {
        tisp_BCSH_ai32Svalue =
            tiziano_bcsh_StrenCal.part.0($s5_1, 0, 0x80, 0, tisp_BCSH_ai32Svalue);
        data_c5454 = tiziano_bcsh_StrenCal.part.0($s5_1, 0, 0x80, 0, data_c5454);
        data_c5458 = tiziano_bcsh_StrenCal.part.0($s5_1, 0, 0x80, 0, data_c5458);
        data_c545c = tiziano_bcsh_StrenCal.part.0($s5_1, 0, 0x80, 0, data_c545c);
    }
    
    **&data_c5400_1 = *(tisp_BCSH_au32OffsetYUVy + 4) + tisp_BCSH_u32OffsetRGB2yuv - 0x800 + $s7;
    *((int32_t*)((char*)data_c5400_2 + 4)) = data_aa67c_1; // Fixed void pointer dereference
    *((int32_t*)((char*)data_c5400_3 + 8)) = data_aa680_1; // Fixed void pointer dereference
    int32_t tisp_BCSH_au32C_1 = tisp_BCSH_au32C;
    uint32_t $v1_3;
    
    if (tisp_BCSH_au32C_1)
        $v1_3 = data_9a91e_1;
    
    if (tisp_BCSH_au32C_1 && $(uintptr_t)v1_3 != 0x80)
    {
            int32_t $s4_1 = data_aa7c0;
            int32_t $a3_7 = data_aa7bc;
            int32_t $t1_3 = $s4_1 - $a3_7;
            int32_t $s6_2 = ($t1_3 >> 1) + $a3_7;
        tisp_BCSH_ai32C = tisp_BCSH_au32C_1;
        
        if ($v1_3 < 0)
        {
            
            if ($a3_7 >= $s4_1)
                $t1_3 = $a3_7 - $s4_1;
            
            data_c5464 = tiziano_bcsh_StrenCal.part.0($v1_3, 0x80, 0xff, $a3_7, $s6_2);
            data_c5468 = tiziano_bcsh_StrenCal($v1_3, 0x80, 0xff, $s4_1, $s6_2, 1);
            data_c546c = tiziano_bcsh_StrenCal($v1_3, 0x80, 0xff, data_aa7c4, 0, 1);
            data_c5470 = tiziano_bcsh_StrenCal.part.0($v1_3, 0x80, 0xff, data_aa7c8, 0x3ff);
        }
        else
        {
            int32_t $s7_1 = data_aa7c8;
            int32_t $s6_1 = data_aa7c4;
            int32_t $t8_1 = $s7_1 - $s6_1;
            int32_t $v0_35 = ($t8_1 >> 1) + $s6_1;
            
            if ($s6_1 >= $s7_1)
                $t8_1 = $s6_1 - $s7_1;
            
            data_c5464 = tiziano_bcsh_StrenCal.part.0($v1_3, 0, 0x80, 0, data_aa7bc);
            data_c5468 = tiziano_bcsh_StrenCal($v1_3, 0, 0x80, 0x3ff, data_aa7c0, 1);
            data_c546c = tiziano_bcsh_StrenCal($v1_3, 0, 0x80, $v0_35, $s6_1, 1);
            data_c5470 = tiziano_bcsh_StrenCal.part.0($v1_3, 0, 0x80, $v0_35, $s7_1);
            tisp_BCSH_ai32Svalue =
                tiziano_bcsh_StrenCal.part.0($v1_3, 0, 0x80, 0, tisp_BCSH_ai32Svalue);
            data_c5454 = tiziano_bcsh_StrenCal.part.0($v1_3, 0, 0x80, 0, data_c5454);
            data_c5458 = tiziano_bcsh_StrenCal.part.0($v1_3, 0, 0x80, 0, data_c5458);
            data_c545c = tiziano_bcsh_StrenCal.part.0($v1_3, 0, 0x80, 0, data_c545c);
        }
    }
    else
        memcpy(&tisp_BCSH_ai32C, &tisp_BCSH_au32C, 0x14);
    
    uint32_t s_bcsh_mjpeg_mode_1 = s_bcsh_mjpeg_mode;
    
    if (s_bcsh_mjpeg_mode_1 == 1)
    {
        uint32_t s_bcsh_mjpeg_y_range_low_1 = s_bcsh_mjpeg_y_range_low;
        tisp_BCSH_ai32C = s_bcsh_mjpeg_mode_1;
        data_c546c = 0;
        data_c5464 = s_bcsh_mjpeg_y_range_low_1 << 2;
        data_c5468 = (data_9a91c << 2) + 3;
        data_c5470 = 0x3ff;
    }
    
    data_c5408_1 = &tisp_BCSH_u32Cslope0;
    data_c540c_1 = &tisp_BCSH_u32Cslope1;
    int32_t $a1_3 = data_c5464_1;
    data_c5404_1 = &tisp_BCSH_ai32C;
    data_c5410_1 = &tisp_BCSH_u32Cslope2;
    
    if ($a1_3)
        tisp_BCSH_u32Cslope0 = (data_c546c_1 << 0xa) / $a1_3;
    else
        tisp_BCSH_u32Cslope0 = 0;
    
    int32_t $a2_2 = data_c5468_1;
    
    if ($a1_3 < $a2_2)
    {
        int32_t $v1_5 = data_c5470;
        int32_t $a0_27 = data_c546c;
        int32_t $v1_6;
        
        $v1_6 = $a0_27 >= $v1_5 ? $a0_27 - $v1_5 : $v1_5 - $a0_27;
        
        tisp_BCSH_u32Cslope1 = ($v1_6 << 0xa) / ($a2_2 - $a1_3);
    }
    else
    {
        data_c5464 = $a2_2;
        tisp_BCSH_u32Cslope1 = 0;
    }
    
    int32_t $v1_9 = data_aa6cc_1;
    
    if ($a2_2 < $v1_9)
    {
        int32_t $v0_61 = data_c5470;
        int32_t $v0_63;
        
        $v0_63 = $v0_61 >= $v1_9 ? $v0_61 - $v1_9 : $v1_9 - $v0_61;
        
        **&data_c5410 = ($v0_63 << 0xa) / ($v1_9 - $a2_2);
    }
    else
    {
        int32_t* $v0_62 = data_c5410;
        data_c5468 = $v1_9;
        *$v0_62 = 0;
    }
    
    char* tisp_BCSH_au32Sthres_now_1 = (char*)(tisp_BCSH_au32Sthres_now); // Fixed void pointer assignment
    data_c5414_1 = tisp_BCSH_au32Sthres_now_1;
    data_c5418_1 = &tisp_BCSH_ai32Svalue;
    data_c541c_1 = &tisp_BCSH_u32Sstep;
    int32_t $t0 = *(tisp_BCSH_au32Sthres_now_1 + 4);
    int32_t $v0_66 = *(tisp_BCSH_au32Sthres_now_1 + 8);
    
    if ($t0 < $v0_66)
    {
        int32_t $v1_11 = data_c5454;
        int32_t tisp_BCSH_ai32Svalue_1 = tisp_BCSH_ai32Svalue;
        int32_t $v0_69 = $v0_66 - $t0;
        int32_t $v1_12 = $v1_11 - tisp_BCSH_ai32Svalue_1;
        int32_t $a1_7 = data_c545c;
        int32_t $a2_3 = data_c5458;
        int32_t $v1_14 = $a2_3 - $a1_7;
        
        if (tisp_BCSH_ai32Svalue_1 >= $v1_11)
            $v1_12 = tisp_BCSH_ai32Svalue_1 - $v1_11;
        
        *tisp_BCSH_u32Sstep = $v1_12 / $v0_69;
        
        if ($a2_3 < $a1_7)
            $v1_14 = $a1_7 - $a2_3;
        
        *((int32_t*)((char*)tisp_BCSH_u32Sstep + 4)) = $v1_14 / $v0_69; // Fixed void pointer dereference
    }
    else
    {
        *((int32_t*)((char*)tisp_BCSH_au32Sthres_now_1 + 4)) = $v0_66; // Fixed void pointer dereference
        **&data_c541c = 0;
        *((int32_t*)((char*)data_c541c + 4)) = 0; // Fixed void pointer dereference
    }
    
    data_c5420_1 = &tisp_BCSH_au32HMatrix;
    char* tisp_BCSH_au32HDP_now_1 = (char*)(tisp_BCSH_au32HDP_now); // Fixed void pointer assignment
    data_c542c_1 = tisp_BCSH_au32HBP_now;
    char* tisp_BCSH_au32HLSP_now_1 = (char*)(tisp_BCSH_au32HLSP_now); // Fixed void pointer assignment
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
        *((int32_t*)((char*)tisp_BCSH_au32HDP_now_1 + 4)) = $v0_72; // Fixed void pointer dereference
        **&data_c5428 = 0;
    }
    
    char* $v1_15 = (char*)(data_c542c_2); // Fixed void pointer assignment
    int32_t $a0_32 = *($v1_15 + 4);
    int32_t $v0_76 = *($v1_15 + 8);
    
    if ($a0_32 < $v0_76)
        **&data_c5430_2 = 0x400 / ($v0_76 - $a0_32);
    else
    {
        *((int32_t*)((char*)$v1_15 + 4)) = $v0_76; // Fixed void pointer dereference
        **&data_c5430 = 0;
    }
    
    char* $v1_16 = (char*)(data_c5434_2); // Fixed void pointer assignment
    int32_t $a0_33 = *($v1_16 + 4);
    int32_t $v0_80 = *($v1_16 + 8);
    int32_t* result;
    
    if ($a0_33 < $v0_80)
    {
        result = 0x400 / ($v0_80 - $a0_33);
        **&data_c5438 = result;
    }
    else
    {
        *((int32_t*)((char*)$v1_16 + 4)) = $v0_80; // Fixed void pointer dereference
        result = data_c5438;
        *result = 0;
    }
    
    return result;
}

