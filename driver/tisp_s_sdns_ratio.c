#include "include/main.h"


  int32_t tisp_s_sdns_ratio(int32_t arg1)

{
    uint32_t sdns_wdr_en_1 = sdns_wdr_en;
    char* $v0 = "TISP_PARAM_TOP_BYPASS";
    int32_t i = 0;
    int32_t $a2 = (uintptr_t)arg1 < 0x81 ? 1 : 0;
        int32_t $t0_114;
        void* $s6_34;
            uint32_t $t0_119;
                int32_t $s7_17 = *($v0 + 0x1c974);
                int32_t $t0_116 = 0;
    data_9a9c0 = arg1;
    
    do
    {
        
        if (sdns_wdr_en_1)
        {
            
            if ($a2)
                $t0_119 = (arg1 * *($v0 + 0x1c974)) >> 7;
            else
            {
                
                if ($(uintptr_t)s7_17 < 0x10)
                    $t0_116 = 0x10 - $s7_17;
                
                $t0_119 = (($t0_116 * (arg1 - 0x80)) >> 7) + $s7_17;
            }
            
            *((int32_t*)((char*)sdns_h_s_1_array_now + i)) = $t0_119; // Fixed void pointer dereference
            uint32_t $t0_126;
            
            if ($a2)
                $t0_126 = (arg1 * *($v0 + 0x1c998)) >> 7;
            else
            {
                int32_t $s7_18 = *($v0 + 0x1c998);
                int32_t $t0_123 = 0;
                
                if ($(uintptr_t)s7_18 < 0x10)
                    $t0_123 = 0x10 - $s7_18;
                
                $t0_126 = (($t0_123 * (arg1 - 0x80)) >> 7) + $s7_18;
            }
            
            *((int32_t*)((char*)sdns_h_s_2_array_now + i)) = $t0_126; // Fixed void pointer dereference
            uint32_t $t0_133;
            
            if ($a2)
                $t0_133 = (arg1 * *($v0 + 0x1c9bc)) >> 7;
            else
            {
                int32_t $s7_19 = *($v0 + 0x1c9bc);
                int32_t $t0_130 = 0;
                
                if ($(uintptr_t)s7_19 < 0x10)
                    $t0_130 = 0x10 - $s7_19;
                
                $t0_133 = (($t0_130 * (arg1 - 0x80)) >> 7) + $s7_19;
            }
            
            *((int32_t*)((char*)sdns_h_s_3_array_now + i)) = $t0_133; // Fixed void pointer dereference
            uint32_t $t0_140;
            
            if ($a2)
                $t0_140 = (arg1 * *($v0 + 0x1c9e0)) >> 7;
            else
            {
                int32_t $s7_20 = *($v0 + 0x1c9e0);
                int32_t $t0_137 = 0;
                
                if ($(uintptr_t)s7_20 < 0x10)
                    $t0_137 = 0x10 - $s7_20;
                
                $t0_140 = (($t0_137 * (arg1 - 0x80)) >> 7) + $s7_20;
            }
            
            *((int32_t*)((char*)sdns_h_s_4_array_now + i)) = $t0_140; // Fixed void pointer dereference
            uint32_t $t0_147;
            
            if ($a2)
                $t0_147 = (arg1 * *($v0 + 0x1ca04)) >> 7;
            else
            {
                int32_t $s7_21 = *($v0 + 0x1ca04);
                int32_t $t0_144 = 0;
                
                if ($(uintptr_t)s7_21 < 0x10)
                    $t0_144 = 0x10 - $s7_21;
                
                $t0_147 = (($t0_144 * (arg1 - 0x80)) >> 7) + $s7_21;
            }
            
            *((int32_t*)((char*)sdns_h_s_5_array_now + i)) = $t0_147; // Fixed void pointer dereference
            uint32_t $t0_154;
            
            if ($a2)
                $t0_154 = (arg1 * *($v0 + 0x1ca28)) >> 7;
            else
            {
                int32_t $s7_22 = *($v0 + 0x1ca28);
                int32_t $t0_151 = 0;
                
                if ($(uintptr_t)s7_22 < 0x10)
                    $t0_151 = 0x10 - $s7_22;
                
                $t0_154 = (($t0_151 * (arg1 - 0x80)) >> 7) + $s7_22;
            }
            
            *((int32_t*)((char*)sdns_h_s_6_array_now + i)) = $t0_154; // Fixed void pointer dereference
            uint32_t $t0_161;
            
            if ($a2)
                $t0_161 = (arg1 * *($v0 + 0x1ca4c)) >> 7;
            else
            {
                int32_t $s7_23 = *($v0 + 0x1ca4c);
                int32_t $t0_158 = 0;
                
                if ($(uintptr_t)s7_23 < 0x10)
                    $t0_158 = 0x10 - $s7_23;
                
                $t0_161 = (($t0_158 * (arg1 - 0x80)) >> 7) + $s7_23;
            }
            
            *((int32_t*)((char*)sdns_h_s_7_array_now + i)) = $t0_161; // Fixed void pointer dereference
            uint32_t $t0_168;
            
            if ($a2)
                $t0_168 = (arg1 * *($v0 + sensor_alloc_integration_time_short)) >> 7;
            else
            {
                int32_t $s7_24 = *($v0 + sensor_alloc_integration_time_short);
                int32_t $t0_165 = 0;
                
                if ($(uintptr_t)s7_24 < 0x10)
                    $t0_165 = 0x10 - $s7_24;
                
                $t0_168 = (($t0_165 * (arg1 - 0x80)) >> 7) + $s7_24;
            }
            
            *((int32_t*)((char*)sdns_h_s_8_array_now + i)) = $t0_168; // Fixed void pointer dereference
            uint32_t $t0_175;
            
            if ($a2)
                $t0_175 = (arg1 * *($v0 + 0x1ca94)) >> 7;
            else
            {
                int32_t $s7_25 = *($v0 + 0x1ca94);
                int32_t $t0_172 = 0;
                
                if ($(uintptr_t)s7_25 < 0x10)
                    $t0_172 = 0x10 - $s7_25;
                
                $t0_175 = (($t0_172 * (arg1 - 0x80)) >> 7) + $s7_25;
            }
            
            *((int32_t*)((char*)sdns_h_s_9_array_now + i)) = $t0_175; // Fixed void pointer dereference
            uint32_t $t0_182;
            
            if ($a2)
                $t0_182 = (arg1 * *($v0 + 0x1cab8)) >> 7;
            else
            {
                int32_t $s7_26 = *($v0 + 0x1cab8);
                int32_t $t0_179 = 0;
                
                if ($(uintptr_t)s7_26 < 0x10)
                    $t0_179 = 0x10 - $s7_26;
                
                $t0_182 = (($t0_179 * (arg1 - 0x80)) >> 7) + $s7_26;
            }
            
            *((int32_t*)((char*)sdns_h_s_10_array_now + i)) = $t0_182; // Fixed void pointer dereference
            uint32_t $t0_189;
            
            if ($a2)
                $t0_189 = (arg1 * *($v0 + 0x1cadc)) >> 7;
            else
            {
                int32_t $s7_27 = *($v0 + 0x1cadc);
                int32_t $t0_186 = 0;
                
                if ($(uintptr_t)s7_27 < 0x10)
                    $t0_186 = 0x10 - $s7_27;
                
                $t0_189 = (($t0_186 * (arg1 - 0x80)) >> 7) + $s7_27;
            }
            
            *((int32_t*)((char*)sdns_h_s_11_array_now + i)) = $t0_189; // Fixed void pointer dereference
            uint32_t $t0_196;
            
            if ($a2)
                $t0_196 = (arg1 * *($v0 + 0x1cb00)) >> 7;
            else
            {
                int32_t $s7_28 = *($v0 + 0x1cb00);
                int32_t $t0_193 = 0;
                
                if ($(uintptr_t)s7_28 < 0x10)
                    $t0_193 = 0x10 - $s7_28;
                
                $t0_196 = (($t0_193 * (arg1 - 0x80)) >> 7) + $s7_28;
            }
            
            *((int32_t*)((char*)sdns_h_s_12_array_now + i)) = $t0_196; // Fixed void pointer dereference
            uint32_t $t0_203;
            
            if ($a2)
                $t0_203 = (arg1 * *($v0 + 0x1cb24)) >> 7;
            else
            {
                int32_t $s7_29 = *($v0 + 0x1cb24);
                int32_t $t0_200 = 0;
                
                if ($(uintptr_t)s7_29 < 0x10)
                    $t0_200 = 0x10 - $s7_29;
                
                $t0_203 = (($t0_200 * (arg1 - 0x80)) >> 7) + $s7_29;
            }
            
            *((int32_t*)((char*)sdns_h_s_13_array_now + i)) = $t0_203; // Fixed void pointer dereference
            uint32_t $t0_210;
            
            if ($a2)
                $t0_210 = (arg1 * *($v0 + 0x1cb48)) >> 7;
            else
            {
                int32_t $s7_30 = *($v0 + 0x1cb48);
                int32_t $t0_207 = 0;
                
                if ($(uintptr_t)s7_30 < 0x10)
                    $t0_207 = 0x10 - $s7_30;
                
                $t0_210 = (($t0_207 * (arg1 - 0x80)) >> 7) + $s7_30;
            }
            
            *((int32_t*)((char*)sdns_h_s_14_array_now + i)) = $t0_210; // Fixed void pointer dereference
            uint32_t $t0_217;
            
            if ($a2)
                $t0_217 = (arg1 * *($v0 + 0x1cb6c)) >> 7;
            else
            {
                int32_t $s7_31 = *($v0 + 0x1cb6c);
                int32_t $t0_214 = 0;
                
                if ($(uintptr_t)s7_31 < 0x10)
                    $t0_214 = 0x10 - $s7_31;
                
                $t0_217 = (($t0_214 * (arg1 - 0x80)) >> 7) + $s7_31;
            }
            
            *((int32_t*)((char*)sdns_h_s_15_array_now + i)) = $t0_217; // Fixed void pointer dereference
            uint32_t $t0_224;
            
            if ($a2)
                $t0_224 = (arg1 * *($v0 + 0x1cb90)) >> 7;
            else
            {
                int32_t $s7_32 = *($v0 + 0x1cb90);
                int32_t $t0_221 = 0;
                
                if ($(uintptr_t)s7_32 < 0x10)
                    $t0_221 = 0x10 - $s7_32;
                
                $t0_224 = (($t0_221 * (arg1 - 0x80)) >> 7) + $s7_32;
            }
            
            *((int32_t*)((char*)sdns_h_s_16_array_now + i)) = $t0_224; // Fixed void pointer dereference
            $t0_114 = 0x1cbe4;
            $s6_34 = sdns_ave_thres_array_now + i;
        }
        else
        {
            uint32_t $t0_6;
                int32_t $s7_1 = *($v0 + 0x1bbfc);
                int32_t $t0_3 = 0;
            
            if ($a2)
                $t0_6 = (arg1 * *($v0 + 0x1bbfc)) >> 7;
            else
            {
                
                if ($(uintptr_t)s7_1 < 0x10)
                    $t0_3 = 0x10 - $s7_1;
                
                $t0_6 = (($t0_3 * (arg1 - 0x80)) >> 7) + $s7_1;
            }
            
            *((int32_t*)((char*)sdns_h_s_1_array_now + i)) = $t0_6; // Fixed void pointer dereference
            uint32_t $t0_13;
            
            if ($a2)
                $t0_13 = (arg1 * *($v0 + 0x1bc20)) >> 7;
            else
            {
                int32_t $s7_2 = *($v0 + 0x1bc20);
                int32_t $t0_10 = 0;
                
                if ($(uintptr_t)s7_2 < 0x10)
                    $t0_10 = 0x10 - $s7_2;
                
                $t0_13 = (($t0_10 * (arg1 - 0x80)) >> 7) + $s7_2;
            }
            
            *((int32_t*)((char*)sdns_h_s_2_array_now + i)) = $t0_13; // Fixed void pointer dereference
            uint32_t $t0_20;
            
            if ($a2)
                $t0_20 = (arg1 * *($v0 + 0x1bc44)) >> 7;
            else
            {
                int32_t $s7_3 = *($v0 + 0x1bc44);
                int32_t $t0_17 = 0;
                
                if ($(uintptr_t)s7_3 < 0x10)
                    $t0_17 = 0x10 - $s7_3;
                
                $t0_20 = (($t0_17 * (arg1 - 0x80)) >> 7) + $s7_3;
            }
            
            *((int32_t*)((char*)sdns_h_s_3_array_now + i)) = $t0_20; // Fixed void pointer dereference
            uint32_t $t0_27;
            
            if ($a2)
                $t0_27 = (arg1 * *($v0 + 0x1bc68)) >> 7;
            else
            {
                int32_t $s7_4 = *($v0 + 0x1bc68);
                int32_t $t0_24 = 0;
                
                if ($(uintptr_t)s7_4 < 0x10)
                    $t0_24 = 0x10 - $s7_4;
                
                $t0_27 = (($t0_24 * (arg1 - 0x80)) >> 7) + $s7_4;
            }
            
            *((int32_t*)((char*)sdns_h_s_4_array_now + i)) = $t0_27; // Fixed void pointer dereference
            uint32_t $t0_34;
            
            if ($a2)
                $t0_34 = (arg1 * *($v0 + 0x1bc8c)) >> 7;
            else
            {
                int32_t $s7_5 = *($v0 + 0x1bc8c);
                int32_t $t0_31 = 0;
                
                if ($(uintptr_t)s7_5 < 0x10)
                    $t0_31 = 0x10 - $s7_5;
                
                $t0_34 = (($t0_31 * (arg1 - 0x80)) >> 7) + $s7_5;
            }
            
            *((int32_t*)((char*)sdns_h_s_5_array_now + i)) = $t0_34; // Fixed void pointer dereference
            uint32_t $t0_41;
            
            if ($a2)
                $t0_41 = (arg1 * *($v0 + 0x1bcb0)) >> 7;
            else
            {
                int32_t $s7_6 = *($v0 + 0x1bcb0);
                int32_t $t0_38 = 0;
                
                if ($(uintptr_t)s7_6 < 0x10)
                    $t0_38 = 0x10 - $s7_6;
                
                $t0_41 = (($t0_38 * (arg1 - 0x80)) >> 7) + $s7_6;
            }
            
            *((int32_t*)((char*)sdns_h_s_6_array_now + i)) = $t0_41; // Fixed void pointer dereference
            uint32_t $t0_48;
            
            if ($a2)
                $t0_48 = (arg1 * *($v0 + 0x1bcd4)) >> 7;
            else
            {
                int32_t $s7_7 = *($v0 + 0x1bcd4);
                int32_t $t0_45 = 0;
                
                if ($(uintptr_t)s7_7 < 0x10)
                    $t0_45 = 0x10 - $s7_7;
                
                $t0_48 = (($t0_45 * (arg1 - 0x80)) >> 7) + $s7_7;
            }
            
            *((int32_t*)((char*)sdns_h_s_7_array_now + i)) = $t0_48; // Fixed void pointer dereference
            uint32_t $t0_55;
            
            if ($a2)
                $t0_55 = (arg1 * *($v0 + &data_1bcf8)) >> 7;
            else
            {
                int32_t $s7_8 = *($v0 + &data_1bcf8);
                int32_t $t0_52 = 0;
                
                if ($(uintptr_t)s7_8 < 0x10)
                    $t0_52 = 0x10 - $s7_8;
                
                $t0_55 = (($t0_52 * (arg1 - 0x80)) >> 7) + $s7_8;
            }
            
            *((int32_t*)((char*)sdns_h_s_8_array_now + i)) = $t0_55; // Fixed void pointer dereference
            uint32_t $t0_62;
            
            if ($a2)
                $t0_62 = (arg1 * *($v0 + 0x1bd1c)) >> 7;
            else
            {
                int32_t $s7_9 = *($v0 + 0x1bd1c);
                int32_t $t0_59 = 0;
                
                if ($(uintptr_t)s7_9 < 0x10)
                    $t0_59 = 0x10 - $s7_9;
                
                $t0_62 = (($t0_59 * (arg1 - 0x80)) >> 7) + $s7_9;
            }
            
            *((int32_t*)((char*)sdns_h_s_9_array_now + i)) = $t0_62; // Fixed void pointer dereference
            uint32_t $t0_69;
            
            if ($a2)
                $t0_69 = (arg1 * *($v0 + 0x1bd40)) >> 7;
            else
            {
                int32_t $s7_10 = *($v0 + 0x1bd40);
                int32_t $t0_66 = 0;
                
                if ($(uintptr_t)s7_10 < 0x10)
                    $t0_66 = 0x10 - $s7_10;
                
                $t0_69 = (($t0_66 * (arg1 - 0x80)) >> 7) + $s7_10;
            }
            
            *((int32_t*)((char*)sdns_h_s_10_array_now + i)) = $t0_69; // Fixed void pointer dereference
            uint32_t $t0_76;
            
            if ($a2)
                $t0_76 = (arg1 * *($v0 + &data_1bd64)) >> 7;
            else
            {
                int32_t $s7_11 = *($v0 + &data_1bd64);
                int32_t $t0_73 = 0;
                
                if ($(uintptr_t)s7_11 < 0x10)
                    $t0_73 = 0x10 - $s7_11;
                
                $t0_76 = (($t0_73 * (arg1 - 0x80)) >> 7) + $s7_11;
            }
            
            *((int32_t*)((char*)sdns_h_s_11_array_now + i)) = $t0_76; // Fixed void pointer dereference
            uint32_t $t0_83;
            
            if ($a2)
                $t0_83 = (arg1 * *($v0 + 0x1bd88)) >> 7;
            else
            {
                int32_t $s7_12 = *($v0 + 0x1bd88);
                int32_t $t0_80 = 0;
                
                if ($(uintptr_t)s7_12 < 0x10)
                    $t0_80 = 0x10 - $s7_12;
                
                $t0_83 = (($t0_80 * (arg1 - 0x80)) >> 7) + $s7_12;
            }
            
            *((int32_t*)((char*)sdns_h_s_12_array_now + i)) = $t0_83; // Fixed void pointer dereference
            uint32_t $t0_90;
            
            if ($a2)
                $t0_90 = (arg1 * *($v0 + 0x1bdac)) >> 7;
            else
            {
                int32_t $s7_13 = *($v0 + 0x1bdac);
                int32_t $t0_87 = 0;
                
                if ($(uintptr_t)s7_13 < 0x10)
                    $t0_87 = 0x10 - $s7_13;
                
                $t0_90 = (($t0_87 * (arg1 - 0x80)) >> 7) + $s7_13;
            }
            
            *((int32_t*)((char*)sdns_h_s_13_array_now + i)) = $t0_90; // Fixed void pointer dereference
            uint32_t $t0_97;
            
            if ($a2)
                $t0_97 = (arg1 * *($v0 + &data_1bdd0)) >> 7;
            else
            {
                int32_t $s7_14 = *($v0 + &data_1bdd0);
                int32_t $t0_94 = 0;
                
                if ($(uintptr_t)s7_14 < 0x10)
                    $t0_94 = 0x10 - $s7_14;
                
                $t0_97 = (($t0_94 * (arg1 - 0x80)) >> 7) + $s7_14;
            }
            
            *((int32_t*)((char*)sdns_h_s_14_array_now + i)) = $t0_97; // Fixed void pointer dereference
            uint32_t $t0_104;
            
            if ($a2)
                $t0_104 = (arg1 * *($v0 + 0x1bdf4)) >> 7;
            else
            {
                int32_t $s7_15 = *($v0 + 0x1bdf4);
                int32_t $t0_101 = 0;
                
                if ($(uintptr_t)s7_15 < 0x10)
                    $t0_101 = 0x10 - $s7_15;
                
                $t0_104 = (($t0_101 * (arg1 - 0x80)) >> 7) + $s7_15;
            }
            
            *((int32_t*)((char*)sdns_h_s_15_array_now + i)) = $t0_104; // Fixed void pointer dereference
            uint32_t $t0_111;
            
            if ($a2)
                $t0_111 = (arg1 * *($v0 + &data_1be18)) >> 7;
            else
            {
                int32_t $s7_16 = *($v0 + &data_1be18);
                int32_t $t0_108 = 0;
                
                if ($(uintptr_t)s7_16 < 0x10)
                    $t0_108 = 0x10 - $s7_16;
                
                $t0_111 = (($t0_108 * (arg1 - 0x80)) >> 7) + $s7_16;
            }
            
            *((int32_t*)((char*)sdns_h_s_16_array_now + i)) = $t0_111; // Fixed void pointer dereference
            $t0_114 = 0x1c8b0;
            $s6_34 = sdns_ave_thres_array_now + i;
        }
        
        char* $t0_227 = (char*)(&$v0[$t0_114]); // Fixed void pointer assignment
        uint32_t $t0_231;
        
        if ($a2)
            $t0_231 = (arg1 * *$t0_227) >> 7;
        else
        {
            int32_t $s7_33 = *$t0_227;
            int32_t $t0_228 = 0;
            
            if ($(uintptr_t)s7_33 < 0xc8)
                $t0_228 = 0xc8 - $s7_33;
            
            $t0_231 = (($t0_228 * (arg1 - 0x80)) >> 7) + $s7_33;
        }
        
        *$s6_34 = $t0_231;
        i += 4;
        $v0 = &$v0[4];
    } while ((uintptr_t)i != 0x24);
    
    /* tailcall */
    return tisp_sdns_all_reg_refresh(data_9a9c4_2 + 0x200);
}

