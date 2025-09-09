#include "include/main.h"


  int32_t tiziano_adr_algorithm()

{
        void* adr_ev_list_now_1 = adr_ev_list_now;
        uint32_t ev_now_1 = ev_now;
        void* adr_ev_list_now_85 = adr_ev_list_now_1;
        int32_t $a3_1 = 0;
            int32_t $s4_1 = *adr_ev_list_now_85;
            int32_t $v1_1 = $a3_1 << 2;
                    void* adr_map_mode_now_2 = adr_map_mode_now;
    if (ev_changed == 1)
    {
        ev_changed = 0;
        
        while (true)
        {
            void* adr_block_light_now_1;
            int32_t $a1_13;
            
            if ($s4_1 < ev_now_1)
            {
                $a3_1 += 1;
                adr_ev_list_now_85 += 4;
                
                if ($a3_1 != 9)
                    continue;
                else
                {
                    **&adr_map_mode_now = data_af7cc;
                    *(((void**)((char*)adr_map_mode_now_2 + 8))) = data_af800; // Fixed void pointer dereference
                    *(((void**)((char*)adr_map_mode_now_2 + 0x14))) = *(adr_blp2_list_now + 0x20); // Fixed void pointer dereference
                    *(((void**)((char*)adr_light_end_now + 0x70))) = *(adr_ligb_list_now + 0x20); // Fixed void pointer dereference
                    adr_block_light_now_1 = adr_block_light_now;
                    *(((void**)((char*)adr_block_light_now_1 + 0x28))) = *(adr_mapb1_list_now + 0x20); // Fixed void pointer dereference
                    *(((void**)((char*)adr_block_light_now_1 + 0x2c))) = *(adr_mapb2_list_now + 0x20); // Fixed void pointer dereference
                    *(((void**)((char*)adr_block_light_now_1 + 0x30))) = *(adr_mapb3_list_now + 0x20); // Fixed void pointer dereference
                    $a1_13 = *(adr_mapb4_list_now + 0x20);
                }
            }
            else
            {
                    int32_t $a2_2 = *(adr_ev_list_now_1 + ($a3_1 << 2) - 4);
                        int32_t $s2_1 = *(&param_adr_min_kneepoint_array + $v1_1);
                        int32_t $t1_7 = *(&param_adr_min_kneepoint_array + (($a3_1 + 1) << 2));
                        int32_t $t2_2 = $a2_2 < ev_now_1 ? 1 : 0;
                        int32_t $s5_1 = $s4_1 < $a2_2 ? 1 : 0;
                            int32_t $t1_12 = ev_now_1 - $a2_2;
                            int32_t $a2_16 = $s4_1 - $a2_2;
                if ($a3_1)
                {
                    
                    if ($s4_1 != $a2_2)
                    {
                        int32_t $t1_11;
                        
                        if ($t1_7 >= $s2_1)
                        {
                            
                            if (!$t2_2)
                                $t1_12 = $a2_2 - ev_now_1;
                            
                            
                            if ($s5_1)
                                $a2_16 = $a2_2 - $s4_1;
                            
                            $t1_11 = $t1_12 * ($t1_7 - $s2_1) / $a2_16 + $s2_1;
                        }
                        else
                        {
                            int32_t $s7_1 = ev_now_1 - $a2_2;
                            int32_t $t1_9 = $a2_2 - $s4_1;
                            
                            if (!$t2_2)
                                $s7_1 = $a2_2 - ev_now_1;
                            
                            
                            if (!$s5_1)
                                $t1_9 = $s4_1 - $a2_2;
                            
                            $t1_11 = $s2_1 - $s7_1 * ($s2_1 - $t1_7) / $t1_9;
                        }
                        
                        **&adr_map_mode_now = $t1_11;
                        int32_t $a2_17 = *(&param_adr_min_kneepoint_array + (($a3_1 + 0xd) << 2));
                        int32_t $a3_5 = *(&param_adr_min_kneepoint_array + (($a3_1 + 0xe) << 2));
                        void* adr_map_mode_now_4 = adr_map_mode_now;
                        int32_t $a2_18;
                        
                        if ($a3_5 >= $a2_17)
                        {
                            void* adr_ev_list_now_80 = adr_ev_list_now;
                            int32_t $s1_3 = *(adr_ev_list_now_80 + ($a3_1 << 2) - 4);
                            int32_t $t0_6 = $s1_3 - ev_now_1;
                            int32_t $t1_18 = *(adr_ev_list_now_80 + $v1_1);
                            int32_t $s1_4 = $t1_18 - $s1_3;
                            
                            if ($s1_3 < ev_now_1)
                                $t0_6 = ev_now_1 - $s1_3;
                            
                            
                            if ($t1_18 < $s1_3)
                                $s1_4 = $s1_3 - $t1_18;
                            
                            $a2_18 = $t0_6 * ($a3_5 - $a2_17) / $s1_4 + $a2_17;
                        }
                        else
                        {
                            void* adr_ev_list_now_79 = adr_ev_list_now;
                            int32_t $s1_1 = *(adr_ev_list_now_79 + ($a3_1 << 2) - 4);
                            int32_t $t0_4 = $s1_1 - ev_now_1;
                            int32_t $t1_16 = *(adr_ev_list_now_79 + $v1_1);
                            int32_t $s1_2 = $t1_16 - $s1_1;
                            
                            if ($s1_1 < ev_now_1)
                                $t0_4 = ev_now_1 - $s1_1;
                            
                            
                            if ($t1_16 < $s1_1)
                                $s1_2 = $s1_1 - $t1_16;
                            
                            $a2_18 = $a2_17 - $t0_4 * ($a2_17 - $a3_5) / $s1_2;
                        }
                        
                        void* adr_blp2_list_now_1 = adr_blp2_list_now;
                        *(((void**)((char*)adr_map_mode_now_4 + 8))) = $a2_18; // Fixed void pointer dereference
                        int32_t $a2_20 = *(adr_blp2_list_now_1 + ($a3_1 << 2) - 4);
                        int32_t $a3_15 = *(adr_blp2_list_now_1 + $v1_1);
                        void* adr_ev_list_now_78 = adr_ev_list_now;
                        int32_t $a2_21;
                        
                        if ($a3_15 >= $a2_20)
                        {
                            int32_t $s0_3 = *(adr_ev_list_now_78 + ($a3_1 << 2) - 4);
                            int32_t $t0_11 = $s0_3 - ev_now_1;
                            int32_t $t1_22 = *(adr_ev_list_now_78 + $v1_1);
                            int32_t $s0_4 = $t1_22 - $s0_3;
                            
                            if ($s0_3 < ev_now_1)
                                $t0_11 = ev_now_1 - $s0_3;
                            
                            
                            if ($t1_22 < $s0_3)
                                $s0_4 = $s0_3 - $t1_22;
                            
                            $a2_21 = $t0_11 * ($a3_15 - $a2_20) / $s0_4 + $a2_20;
                        }
                        else
                        {
                            int32_t $s0_1 = *(adr_ev_list_now_78 + ($a3_1 << 2) - 4);
                            int32_t $t0_9 = $s0_1 - ev_now_1;
                            int32_t $t1_20 = *(adr_ev_list_now_78 + $v1_1);
                            int32_t $s0_2 = $t1_20 - $s0_1;
                            
                            if ($s0_1 < ev_now_1)
                                $t0_9 = ev_now_1 - $s0_1;
                            
                            
                            if ($t1_20 < $s0_1)
                                $s0_2 = $s0_1 - $t1_20;
                            
                            $a2_21 = $a2_20 - $t0_9 * ($a2_20 - $a3_15) / $s0_2;
                        }
                        
                        void* adr_ligb_list_now_1 = adr_ligb_list_now;
                        *(((void**)((char*)adr_map_mode_now_4 + 0x14))) = $a2_21; // Fixed void pointer dereference
                        int32_t $a2_23 = *(adr_ligb_list_now_1 + ($a3_1 << 2) - 4);
                        int32_t $a3_25 = *(adr_ligb_list_now_1 + $v1_1);
                        int32_t $a2_24;
                        
                        if ($a3_25 >= $a2_23)
                        {
                            void* adr_ev_list_now_84 = adr_ev_list_now;
                            int32_t $t3_14 = *(adr_ev_list_now_84 + ($a3_1 << 2) - 4);
                            int32_t $t0_16 = $t3_14 - ev_now_1;
                            int32_t $t1_26 = *(adr_ev_list_now_84 + $v1_1);
                            int32_t $t3_15 = $t1_26 - $t3_14;
                            
                            if ($t3_14 < ev_now_1)
                                $t0_16 = ev_now_1 - $t3_14;
                            
                            
                            if ($t1_26 < $t3_14)
                                $t3_15 = $t3_14 - $t1_26;
                            
                            $a2_24 = $t0_16 * ($a3_25 - $a2_23) / $t3_15 + $a2_23;
                        }
                        else
                        {
                            void* adr_ev_list_now_83 = adr_ev_list_now;
                            int32_t $t3_12 = *(adr_ev_list_now_83 + ($a3_1 << 2) - 4);
                            int32_t $t0_14 = $t3_12 - ev_now_1;
                            int32_t $t1_24 = *(adr_ev_list_now_83 + $v1_1);
                            int32_t $t3_13 = $t1_24 - $t3_12;
                            
                            if ($t3_12 < ev_now_1)
                                $t0_14 = ev_now_1 - $t3_12;
                            
                            
                            if ($t1_24 < $t3_12)
                                $t3_13 = $t3_12 - $t1_24;
                            
                            $a2_24 = $a2_23 - $t0_14 * ($a2_23 - $a3_25) / $t3_13;
                        }
                        
                        void* adr_mapb1_list_now_1 = adr_mapb1_list_now;
                        *(((void**)((char*)adr_light_end_now + 0x70))) = $a2_24; // Fixed void pointer dereference
                        int32_t $a2_26 = *(adr_mapb1_list_now_1 + ($a3_1 << 2) - 4);
                        int32_t $a3_35 = *(adr_mapb1_list_now_1 + $v1_1);
                        void* adr_block_light_now_4 = adr_block_light_now;
                        int32_t $a2_27;
                        
                        if ($a3_35 >= $a2_26)
                        {
                            void* adr_ev_list_now_82 = adr_ev_list_now;
                            int32_t $t7_3 = *(adr_ev_list_now_82 + ($a3_1 << 2) - 4);
                            int32_t $t0_21 = $t7_3 - ev_now_1;
                            int32_t $t1_30 = *(adr_ev_list_now_82 + $v1_1);
                            int32_t $t7_4 = $t1_30 - $t7_3;
                            
                            if ($t7_3 < ev_now_1)
                                $t0_21 = ev_now_1 - $t7_3;
                            
                            
                            if ($t1_30 < $t7_3)
                                $t7_4 = $t7_3 - $t1_30;
                            
                            $a2_27 = $t0_21 * ($a3_35 - $a2_26) / $t7_4 + $a2_26;
                        }
                        else
                        {
                            void* adr_ev_list_now_81 = adr_ev_list_now;
                            int32_t $t7_1 = *(adr_ev_list_now_81 + ($a3_1 << 2) - 4);
                            int32_t $t0_19 = $t7_1 - ev_now_1;
                            int32_t $t1_28 = *(adr_ev_list_now_81 + $v1_1);
                            int32_t $t7_2 = $t1_28 - $t7_1;
                            
                            if ($t7_1 < ev_now_1)
                                $t0_19 = ev_now_1 - $t7_1;
                            
                            
                            if ($t1_28 < $t7_1)
                                $t7_2 = $t7_1 - $t1_28;
                            
                            $a2_27 = $a2_26 - $t0_19 * ($a2_26 - $a3_35) / $t7_2;
                        }
                        
                        void* adr_mapb2_list_now_1 = adr_mapb2_list_now;
                        *(((void**)((char*)adr_block_light_now_4 + 0x28))) = $a2_27; // Fixed void pointer dereference
                        int32_t $a2_29 = *(adr_mapb2_list_now_1 + ($a3_1 << 2) - 4);
                        int32_t $a3_45 = *(adr_mapb2_list_now_1 + $v1_1);
                        void* adr_ev_list_now_77 = adr_ev_list_now;
                        int32_t $a2_30;
                        
                        if ($a3_45 >= $a2_29)
                        {
                            int32_t $t6_3 = *(adr_ev_list_now_77 + ($a3_1 << 2) - 4);
                            int32_t $t0_26 = $t6_3 - ev_now_1;
                            int32_t $t1_34 = *(adr_ev_list_now_77 + $v1_1);
                            int32_t $t6_4 = $t1_34 - $t6_3;
                            
                            if ($t6_3 < ev_now_1)
                                $t0_26 = ev_now_1 - $t6_3;
                            
                            
                            if ($t1_34 < $t6_3)
                                $t6_4 = $t6_3 - $t1_34;
                            
                            $a2_30 = $t0_26 * ($a3_45 - $a2_29) / $t6_4 + $a2_29;
                        }
                        else
                        {
                            int32_t $t6_1 = *(adr_ev_list_now_77 + ($a3_1 << 2) - 4);
                            int32_t $t0_24 = $t6_1 - ev_now_1;
                            int32_t $t1_32 = *(adr_ev_list_now_77 + $v1_1);
                            int32_t $t6_2 = $t1_32 - $t6_1;
                            
                            if ($t6_1 < ev_now_1)
                                $t0_24 = ev_now_1 - $t6_1;
                            
                            
                            if ($t1_32 < $t6_1)
                                $t6_2 = $t6_1 - $t1_32;
                            
                            $a2_30 = $a2_29 - $t0_24 * ($a2_29 - $a3_45) / $t6_2;
                        }
                        
                        void* adr_mapb3_list_now_1 = adr_mapb3_list_now;
                        *(((void**)((char*)adr_block_light_now_4 + 0x2c))) = $a2_30; // Fixed void pointer dereference
                        int32_t $a2_32 = *(adr_mapb3_list_now_1 + ($a3_1 << 2) - 4);
                        int32_t $a3_55 = *(adr_mapb3_list_now_1 + $v1_1);
                        void* adr_ev_list_now_76 = adr_ev_list_now;
                        int32_t $a2_33;
                        
                        if ($a3_55 >= $a2_32)
                        {
                            int32_t $t5_3 = *(adr_ev_list_now_76 + ($a3_1 << 2) - 4);
                            int32_t $t0_31 = $t5_3 - ev_now_1;
                            int32_t $t1_38 = *(adr_ev_list_now_76 + $v1_1);
                            int32_t $t5_4 = $t1_38 - $t5_3;
                            
                            if ($t5_3 < ev_now_1)
                                $t0_31 = ev_now_1 - $t5_3;
                            
                            
                            if ($t1_38 < $t5_3)
                                $t5_4 = $t5_3 - $t1_38;
                            
                            $a2_33 = $t0_31 * ($a3_55 - $a2_32) / $t5_4 + $a2_32;
                        }
                        else
                        {
                            int32_t $t5_1 = *(adr_ev_list_now_76 + ($a3_1 << 2) - 4);
                            int32_t $t0_29 = $t5_1 - ev_now_1;
                            int32_t $t1_36 = *(adr_ev_list_now_76 + $v1_1);
                            int32_t $t5_2 = $t1_36 - $t5_1;
                            
                            if ($t5_1 < ev_now_1)
                                $t0_29 = ev_now_1 - $t5_1;
                            
                            
                            if ($t1_36 < $t5_1)
                                $t5_2 = $t5_1 - $t1_36;
                            
                            $a2_33 = $a2_32 - $t0_29 * ($a2_32 - $a3_55) / $t5_2;
                        }
                        
                        void* adr_mapb4_list_now_1 = adr_mapb4_list_now;
                        *(((void**)((char*)adr_block_light_now_4 + 0x30))) = $a2_33; // Fixed void pointer dereference
                        int32_t $a2_35 = *(adr_mapb4_list_now_1 + ($a3_1 << 2) - 4);
                        int32_t $a3_65 = *(adr_mapb4_list_now_1 + $v1_1);
                        void* adr_ev_list_now_11 = adr_ev_list_now;
                        int32_t $a2_36;
                        
                        if ($a3_65 >= $a2_35)
                        {
                            int32_t $t3_30 = *(adr_ev_list_now_11 + ($a3_1 << 2) - 4);
                            int32_t $a3_68 = $t3_30 - ev_now_1;
                            int32_t $a3_69 = *(adr_ev_list_now_11 + $v1_1);
                            int32_t $t3_31 = $a3_69 - $t3_30;
                            
                            if ($t3_30 < ev_now_1)
                                $a3_68 = ev_now_1 - $t3_30;
                            
                            
                            if ($a3_69 < $t3_30)
                                $t3_31 = $t3_30 - $a3_69;
                            
                            $a2_36 = $a3_68 * ($a3_65 - $a2_35) / $t3_31 + $a2_35;
                        }
                        else
                        {
                            int32_t $t3_28 = *(adr_ev_list_now_11 + ($a3_1 << 2) - 4);
                            int32_t $a3_66 = $t3_28 - ev_now_1;
                            int32_t $a3_67 = *(adr_ev_list_now_11 + $v1_1);
                            int32_t $t3_29 = $a3_67 - $t3_28;
                            
                            if ($t3_28 < ev_now_1)
                                $a3_66 = ev_now_1 - $t3_28;
                            
                            
                            if ($a3_67 < $t3_28)
                                $t3_29 = $t3_28 - $a3_67;
                            
                            $a2_36 = $a2_35 - $a3_66 * ($a2_35 - $a3_65) / $t3_29;
                        }
                        
                        *(((void**)((char*)adr_block_light_now_4 + 0x34))) = $a2_36; // Fixed void pointer dereference
                    }
                    else
                    {
                        void* adr_map_mode_now_3 = adr_map_mode_now;
                        void* adr_block_light_now_2 = adr_block_light_now;
                        **&adr_map_mode_now =
                            *(&param_adr_min_kneepoint_array + (($a3_1 + 1) << 2));
                        *(adr_map_mode_now_3 + 8) =
                            *(&param_adr_min_kneepoint_array + (($a3_1 + 0xe) << 2));
                        *(((void**)((char*)adr_map_mode_now_3 + 0x14))) = *(adr_blp2_list_now + $v1_1); // Fixed void pointer dereference
                        *(((void**)((char*)adr_light_end_now + 0x70))) = *(adr_ligb_list_now + $v1_1); // Fixed void pointer dereference
                        *(((void**)((char*)adr_block_light_now_2 + 0x28))) = *(adr_mapb1_list_now + $v1_1); // Fixed void pointer dereference
                        *(((void**)((char*)adr_block_light_now_2 + 0x2c))) = *(adr_mapb2_list_now + $v1_1); // Fixed void pointer dereference
                        *(((void**)((char*)adr_block_light_now_2 + 0x30))) = *(adr_mapb3_list_now + $v1_1); // Fixed void pointer dereference
                        *(((void**)((char*)adr_block_light_now_2 + 0x34))) = *(adr_mapb4_list_now + $v1_1); // Fixed void pointer dereference
                    }
                    
                    break;
                }
                
                **&adr_map_mode_now = data_af7ac_1;
                void* adr_map_mode_now_1 = adr_map_mode_now;
                *(((void**)((char*)adr_map_mode_now_1 + 8))) = data_af7e0_1; // Fixed void pointer dereference
                *(((void**)((char*)adr_map_mode_now_1 + 0x14))) = **&adr_blp2_list_now; // Fixed void pointer dereference
                *(((void**)((char*)adr_light_end_now + 0x70))) = **&adr_ligb_list_now; // Fixed void pointer dereference
                adr_block_light_now_1 = adr_block_light_now;
                *(((void**)((char*)adr_block_light_now_1 + 0x28))) = **&adr_mapb1_list_now; // Fixed void pointer dereference
                *(((void**)((char*)adr_block_light_now_1 + 0x2c))) = **&adr_mapb2_list_now; // Fixed void pointer dereference
                *(((void**)((char*)adr_block_light_now_1 + 0x30))) = **&adr_mapb3_list_now; // Fixed void pointer dereference
                $a1_13 = **&adr_mapb4_list_now;
            }
            
            *(((void**)((char*)adr_block_light_now_1 + 0x34))) = $a1_13; // Fixed void pointer dereference
            break;
        }
        
        if (data_ace5c_1 == 1)
        {
            void* adr_ev_list_now_2 = adr_ev_list_now;
            int32_t $t1_41 = *adr_ev_list_now_2;
                int32_t $t2_9 = *(adr_ev_list_now_2 + 4);
                    uint32_t param_adr_gam_y_array_1 = param_adr_gam_y_array;
                    uint32_t $a3_71 = data_af220;
                    int32_t $t4_1 = ev_now_1 - $t1_41;
                    int32_t $a2_38 = $t2_9 < $t1_41 ? 1 : 0;
                        int32_t $t1_43 = $t2_9 - $t1_41;
            void* adr_ctc_map2cut_y_now_1;
            
            if ($t1_41 < ev_now_1)
            {
                int32_t $v0_17;
                int32_t $v0_20;
                int32_t $a0_9;
                int32_t $a0_10;
                void* adr_ctc_map2cut_y_now_2;
                uint32_t $a2_53;
                int32_t $a3_128;
                int32_t $a3_130;
                
                if ($t2_9 >= ev_now_1)
                {
                    uint32_t $a3_74;
                    
                    if ($a3_71 >= param_adr_gam_y_array_1)
                    {
                        
                        if ($a2_38)
                            $t1_43 = $t1_41 - $t2_9;
                        
                        $a3_74 = ($a3_71 - param_adr_gam_y_array_1) * $t4_1 / $t1_43
                            + param_adr_gam_y_array_1;
                    }
                    else
                    {
                        int32_t $t0_42 = $t1_41 - $t2_9;
                        
                        if (!$a2_38)
                            $t0_42 = $t2_9 - $t1_41;
                        
                        $a3_74 = param_adr_gam_y_array_1
                            - (param_adr_gam_y_array_1 - $a3_71) * $t4_1 / $t0_42;
                    }
                    
                    **&adr_ctc_map2cut_y_now = $a3_74;
                    uint32_t $a2_39 = data_af20e_1;
                    uint32_t $a3_77 = data_af222_1;
                    adr_ctc_map2cut_y_now_2 = adr_ctc_map2cut_y_now;
                    uint32_t $a2_40;
                    
                    if ($a3_77 >= $a2_39)
                    {
                        void* adr_ev_list_now_13 = adr_ev_list_now;
                        int32_t $t3_34 = *adr_ev_list_now_13;
                        int32_t $t0_48 = $t3_34 - ev_now_1;
                        int32_t $t1_47 = *(adr_ev_list_now_13 + 4);
                        int32_t $t3_35 = $t1_47 - $t3_34;
                        
                        if ($t3_34 < ev_now_1)
                            $t0_48 = ev_now_1 - $t3_34;
                        
                        
                        if ($t1_47 < $t3_34)
                            $t3_35 = $t3_34 - $t1_47;
                        
                        $a2_40 = $t0_48 * ($a3_77 - $a2_39) / $t3_35 + $a2_39;
                    }
                    else
                    {
                        void* adr_ev_list_now_12 = adr_ev_list_now;
                        int32_t $t3_32 = *adr_ev_list_now_12;
                        int32_t $t0_46 = $t3_32 - ev_now_1;
                        int32_t $t1_45 = *(adr_ev_list_now_12 + 4);
                        int32_t $t3_33 = $t1_45 - $t3_32;
                        
                        if ($t3_32 < ev_now_1)
                            $t0_46 = ev_now_1 - $t3_32;
                        
                        
                        if ($t1_45 < $t3_32)
                            $t3_33 = $t3_32 - $t1_45;
                        
                        $a2_40 = $a2_39 - $t0_46 * ($a2_39 - $a3_77) / $t3_33;
                    }
                    
                    *(((void**)((char*)adr_ctc_map2cut_y_now_2 + 4))) = $a2_40; // Fixed void pointer dereference
                    uint32_t $a2_41 = data_af210_1;
                    uint32_t $a3_84 = data_af224_1;
                    void* adr_ev_list_now_14 = adr_ev_list_now;
                    uint32_t $a2_42;
                    
                    if ($a3_84 >= $a2_41)
                    {
                        int32_t $t3_38 = *adr_ev_list_now_14;
                        int32_t $t0_53 = $t3_38 - ev_now_1;
                        int32_t $t1_51 = *(adr_ev_list_now_14 + 4);
                        int32_t $t3_39 = $t1_51 - $t3_38;
                        
                        if ($t3_38 < ev_now_1)
                            $t0_53 = ev_now_1 - $t3_38;
                        
                        
                        if ($t1_51 < $t3_38)
                            $t3_39 = $t3_38 - $t1_51;
                        
                        $a2_42 = $t0_53 * ($a3_84 - $a2_41) / $t3_39 + $a2_41;
                    }
                    else
                    {
                        int32_t $t3_36 = *adr_ev_list_now_14;
                        int32_t $t0_51 = $t3_36 - ev_now_1;
                        int32_t $t1_49 = *(adr_ev_list_now_14 + 4);
                        int32_t $t3_37 = $t1_49 - $t3_36;
                        
                        if ($t3_36 < ev_now_1)
                            $t0_51 = ev_now_1 - $t3_36;
                        
                        
                        if ($t1_49 < $t3_36)
                            $t3_37 = $t3_36 - $t1_49;
                        
                        $a2_42 = $a2_41 - $t0_51 * ($a2_41 - $a3_84) / $t3_37;
                    }
                    
                    *(((void**)((char*)adr_ctc_map2cut_y_now_2 + 8))) = $a2_42; // Fixed void pointer dereference
                    uint32_t $a2_43 = data_af212_1;
                    uint32_t $a3_91 = data_af226_1;
                    void* adr_ev_list_now_15 = adr_ev_list_now;
                    uint32_t $a2_44;
                    
                    if ($a3_91 >= $a2_43)
                    {
                        int32_t $t3_42 = *adr_ev_list_now_15;
                        int32_t $t0_58 = $t3_42 - ev_now_1;
                        int32_t $t1_55 = *(adr_ev_list_now_15 + 4);
                        int32_t $t3_43 = $t1_55 - $t3_42;
                        
                        if ($t3_42 < ev_now_1)
                            $t0_58 = ev_now_1 - $t3_42;
                        
                        
                        if ($t1_55 < $t3_42)
                            $t3_43 = $t3_42 - $t1_55;
                        
                        $a2_44 = $t0_58 * ($a3_91 - $a2_43) / $t3_43 + $a2_43;
                    }
                    else
                    {
                        int32_t $t3_40 = *adr_ev_list_now_15;
                        int32_t $t0_56 = $t3_40 - ev_now_1;
                        int32_t $t1_53 = *(adr_ev_list_now_15 + 4);
                        int32_t $t3_41 = $t1_53 - $t3_40;
                        
                        if ($t3_40 < ev_now_1)
                            $t0_56 = ev_now_1 - $t3_40;
                        
                        
                        if ($t1_53 < $t3_40)
                            $t3_41 = $t3_40 - $t1_53;
                        
                        $a2_44 = $a2_43 - $t0_56 * ($a2_43 - $a3_91) / $t3_41;
                    }
                    
                    *(((void**)((char*)adr_ctc_map2cut_y_now_2 + 0xc))) = $a2_44; // Fixed void pointer dereference
                    uint32_t $a2_45 = data_af214_1;
                    uint32_t $a3_98 = data_af228_1;
                    void* adr_ev_list_now_16 = adr_ev_list_now;
                    uint32_t $a2_46;
                    
                    if ($a3_98 >= $a2_45)
                    {
                        int32_t $t3_46 = *adr_ev_list_now_16;
                        int32_t $t0_63 = $t3_46 - ev_now_1;
                        int32_t $t1_59 = *(adr_ev_list_now_16 + 4);
                        int32_t $t3_47 = $t1_59 - $t3_46;
                        
                        if ($t3_46 < ev_now_1)
                            $t0_63 = ev_now_1 - $t3_46;
                        
                        
                        if ($t1_59 < $t3_46)
                            $t3_47 = $t3_46 - $t1_59;
                        
                        $a2_46 = $t0_63 * ($a3_98 - $a2_45) / $t3_47 + $a2_45;
                    }
                    else
                    {
                        int32_t $t3_44 = *adr_ev_list_now_16;
                        int32_t $t0_61 = $t3_44 - ev_now_1;
                        int32_t $t1_57 = *(adr_ev_list_now_16 + 4);
                        int32_t $t3_45 = $t1_57 - $t3_44;
                        
                        if ($t3_44 < ev_now_1)
                            $t0_61 = ev_now_1 - $t3_44;
                        
                        
                        if ($t1_57 < $t3_44)
                            $t3_45 = $t3_44 - $t1_57;
                        
                        $a2_46 = $a2_45 - $t0_61 * ($a2_45 - $a3_98) / $t3_45;
                    }
                    
                    *(((void**)((char*)adr_ctc_map2cut_y_now_2 + 0x10))) = $a2_46; // Fixed void pointer dereference
                    uint32_t $a2_47 = data_af216_1;
                    uint32_t $a3_105 = data_af22a_1;
                    void* adr_ev_list_now_17 = adr_ev_list_now;
                    uint32_t $a2_48;
                    
                    if ($a3_105 >= $a2_47)
                    {
                        int32_t $t3_50 = *adr_ev_list_now_17;
                        int32_t $t0_68 = $t3_50 - ev_now_1;
                        int32_t $t1_63 = *(adr_ev_list_now_17 + 4);
                        int32_t $t3_51 = $t1_63 - $t3_50;
                        
                        if ($t3_50 < ev_now_1)
                            $t0_68 = ev_now_1 - $t3_50;
                        
                        
                        if ($t1_63 < $t3_50)
                            $t3_51 = $t3_50 - $t1_63;
                        
                        $a2_48 = $t0_68 * ($a3_105 - $a2_47) / $t3_51 + $a2_47;
                    }
                    else
                    {
                        int32_t $t3_48 = *adr_ev_list_now_17;
                        int32_t $t0_66 = $t3_48 - ev_now_1;
                        int32_t $t1_61 = *(adr_ev_list_now_17 + 4);
                        int32_t $t3_49 = $t1_61 - $t3_48;
                        
                        if ($t3_48 < ev_now_1)
                            $t0_66 = ev_now_1 - $t3_48;
                        
                        
                        if ($t1_61 < $t3_48)
                            $t3_49 = $t3_48 - $t1_61;
                        
                        $a2_48 = $a2_47 - $t0_66 * ($a2_47 - $a3_105) / $t3_49;
                    }
                    
                    *(((void**)((char*)adr_ctc_map2cut_y_now_2 + 0x14))) = $a2_48; // Fixed void pointer dereference
                    uint32_t $a2_49 = data_af218_1;
                    uint32_t $a3_112 = data_af22c_1;
                    void* adr_ev_list_now_18 = adr_ev_list_now;
                    uint32_t $a2_50;
                    
                    if ($a3_112 >= $a2_49)
                    {
                        int32_t $t3_54 = *adr_ev_list_now_18;
                        int32_t $t0_73 = $t3_54 - ev_now_1;
                        int32_t $t1_67 = *(adr_ev_list_now_18 + 4);
                        int32_t $t3_55 = $t1_67 - $t3_54;
                        
                        if ($t3_54 < ev_now_1)
                            $t0_73 = ev_now_1 - $t3_54;
                        
                        
                        if ($t1_67 < $t3_54)
                            $t3_55 = $t3_54 - $t1_67;
                        
                        $a2_50 = $t0_73 * ($a3_112 - $a2_49) / $t3_55 + $a2_49;
                    }
                    else
                    {
                        int32_t $t3_52 = *adr_ev_list_now_18;
                        int32_t $t0_71 = $t3_52 - ev_now_1;
                        int32_t $t1_65 = *(adr_ev_list_now_18 + 4);
                        int32_t $t3_53 = $t1_65 - $t3_52;
                        
                        if ($t3_52 < ev_now_1)
                            $t0_71 = ev_now_1 - $t3_52;
                        
                        
                        if ($t1_65 < $t3_52)
                            $t3_53 = $t3_52 - $t1_65;
                        
                        $a2_50 = $a2_49 - $t0_71 * ($a2_49 - $a3_112) / $t3_53;
                    }
                    
                    *(((void**)((char*)adr_ctc_map2cut_y_now_2 + 0x18))) = $a2_50; // Fixed void pointer dereference
                    uint32_t $a2_51 = data_af21a_1;
                    uint32_t $a3_119 = data_af22e_1;
                    void* adr_ev_list_now_19 = adr_ev_list_now;
                    uint32_t $a2_52;
                    
                    if ($a3_119 >= $a2_51)
                    {
                        int32_t $t3_58 = *adr_ev_list_now_19;
                        int32_t $t0_78 = $t3_58 - ev_now_1;
                        int32_t $t1_71 = *(adr_ev_list_now_19 + 4);
                        int32_t $t3_59 = $t1_71 - $t3_58;
                        
                        if ($t3_58 < ev_now_1)
                            $t0_78 = ev_now_1 - $t3_58;
                        
                        
                        if ($t1_71 < $t3_58)
                            $t3_59 = $t3_58 - $t1_71;
                        
                        $a2_52 = $t0_78 * ($a3_119 - $a2_51) / $t3_59 + $a2_51;
                    }
                    else
                    {
                        int32_t $t3_56 = *adr_ev_list_now_19;
                        int32_t $t0_76 = $t3_56 - ev_now_1;
                        int32_t $t1_69 = *(adr_ev_list_now_19 + 4);
                        int32_t $t3_57 = $t1_69 - $t3_56;
                        
                        if ($t3_56 < ev_now_1)
                            $t0_76 = ev_now_1 - $t3_56;
                        
                        
                        if ($t1_69 < $t3_56)
                            $t3_57 = $t3_56 - $t1_69;
                        
                        $a2_52 = $a2_51 - $t0_76 * ($a2_51 - $a3_119) / $t3_57;
                    }
                    
                    *(((void**)((char*)adr_ctc_map2cut_y_now_2 + 0x1c))) = $a2_52; // Fixed void pointer dereference
                    $a2_53 = data_af21c_1;
                    uint32_t $v1_17 = data_af230_1;
                    void* adr_ev_list_now_3 = adr_ev_list_now;
                    
                    if ($v1_17 >= $a2_53)
                    {
                        int32_t $v0_3 = $a0_10 - ev_now_1;
                        $a0_10 = *adr_ev_list_now_3;
                        
                        if ($a0_10 < ev_now_1)
                            $v0_3 = ev_now_1 - $a0_10;
                        
                        $v0_20 = $v0_3 * ($v1_17 - $a2_53);
                        $a3_130 = *(adr_ev_list_now_3 + 4);
                        goto label_5a5fc;
                    }
                    
                    $a0_9 = *adr_ev_list_now_3;
                    int32_t $v0_2 = $a0_9 - ev_now_1;
                    
                    if ($a0_9 < ev_now_1)
                        $v0_2 = ev_now_1 - $a0_9;
                    
                    $v0_17 = $v0_2 * ($a2_53 - $v1_17);
                    $a3_128 = *(adr_ev_list_now_3 + 4);
                    goto label_5a5bc;
                }
                
                int32_t $t1_42 = *(adr_ev_list_now_2 + 8);
                
                if ($t1_42 >= ev_now_1)
                {
                    uint32_t $t4_2 = data_af220;
                    uint32_t $a2_54 = data_af234;
                    int32_t $a3_132 = ev_now_1 - $t2_9;
                    int32_t $t3_60 = $t1_42 < $t2_9 ? 1 : 0;
                        int32_t $t1_75 = $t1_42 - $t2_9;
                    uint32_t $a2_57;
                    
                    if ($a2_54 >= $t4_2)
                    {
                        
                        if ($t3_60)
                            $t1_75 = $t2_9 - $t1_42;
                        
                        $a2_57 = ($a2_54 - $t4_2) * $a3_132 / $t1_75 + $t4_2;
                    }
                    else
                    {
                        int32_t $t1_74 = $t1_42 - $t2_9;
                        
                        if ($t3_60)
                            $t1_74 = $t2_9 - $t1_42;
                        
                        $a2_57 = $t4_2 - ($t4_2 - $a2_54) * $a3_132 / $t1_74;
                    }
                    
                    **&adr_ctc_map2cut_y_now = $a2_57;
                    uint32_t $a2_60 = data_af222_2;
                    uint32_t $a3_134 = data_af236_1;
                    adr_ctc_map2cut_y_now_2 = adr_ctc_map2cut_y_now;
                    uint32_t $a2_61;
                    
                    if ($a3_134 >= $a2_60)
                    {
                        void* adr_ev_list_now_21 = adr_ev_list_now;
                        int32_t $t3_63 = *(adr_ev_list_now_21 + 4);
                        int32_t $t0_87 = $t3_63 - ev_now_1;
                        int32_t $t1_79 = *(adr_ev_list_now_21 + 8);
                        int32_t $t3_64 = $t1_79 - $t3_63;
                        
                        if ($t3_63 < ev_now_1)
                            $t0_87 = ev_now_1 - $t3_63;
                        
                        
                        if ($t1_79 < $t3_63)
                            $t3_64 = $t3_63 - $t1_79;
                        
                        $a2_61 = $t0_87 * ($a3_134 - $a2_60) / $t3_64 + $a2_60;
                    }
                    else
                    {
                        void* adr_ev_list_now_20 = adr_ev_list_now;
                        int32_t $t3_61 = *(adr_ev_list_now_20 + 4);
                        int32_t $t0_85 = $t3_61 - ev_now_1;
                        int32_t $t1_77 = *(adr_ev_list_now_20 + 8);
                        int32_t $t3_62 = $t1_77 - $t3_61;
                        
                        if ($t3_61 < ev_now_1)
                            $t0_85 = ev_now_1 - $t3_61;
                        
                        
                        if ($t1_77 < $t3_61)
                            $t3_62 = $t3_61 - $t1_77;
                        
                        $a2_61 = $a2_60 - $t0_85 * ($a2_60 - $a3_134) / $t3_62;
                    }
                    
                    *(((void**)((char*)adr_ctc_map2cut_y_now_2 + 4))) = $a2_61; // Fixed void pointer dereference
                    uint32_t $a2_62 = data_af224_2;
                    uint32_t $a3_141 = data_af238_1;
                    void* adr_ev_list_now_22 = adr_ev_list_now;
                    uint32_t $a2_63;
                    
                    if ($a3_141 >= $a2_62)
                    {
                        int32_t $t3_67 = *(adr_ev_list_now_22 + 4);
                        int32_t $t0_92 = $t3_67 - ev_now_1;
                        int32_t $t1_83 = *(adr_ev_list_now_22 + 8);
                        int32_t $t3_68 = $t1_83 - $t3_67;
                        
                        if ($t3_67 < ev_now_1)
                            $t0_92 = ev_now_1 - $t3_67;
                        
                        
                        if ($t1_83 < $t3_67)
                            $t3_68 = $t3_67 - $t1_83;
                        
                        $a2_63 = $t0_92 * ($a3_141 - $a2_62) / $t3_68 + $a2_62;
                    }
                    else
                    {
                        int32_t $t3_65 = *(adr_ev_list_now_22 + 4);
                        int32_t $t0_90 = $t3_65 - ev_now_1;
                        int32_t $t1_81 = *(adr_ev_list_now_22 + 8);
                        int32_t $t3_66 = $t1_81 - $t3_65;
                        
                        if ($t3_65 < ev_now_1)
                            $t0_90 = ev_now_1 - $t3_65;
                        
                        
                        if ($t1_81 < $t3_65)
                            $t3_66 = $t3_65 - $t1_81;
                        
                        $a2_63 = $a2_62 - $t0_90 * ($a2_62 - $a3_141) / $t3_66;
                    }
                    
                    *(((void**)((char*)adr_ctc_map2cut_y_now_2 + 8))) = $a2_63; // Fixed void pointer dereference
                    uint32_t $a2_64 = data_af226_2;
                    uint32_t $a3_148 = data_af23a_1;
                    void* adr_ev_list_now_23 = adr_ev_list_now;
                    uint32_t $a2_65;
                    
                    if ($a3_148 >= $a2_64)
                    {
                        int32_t $t3_71 = *(adr_ev_list_now_23 + 4);
                        int32_t $t0_97 = $t3_71 - ev_now_1;
                        int32_t $t1_87 = *(adr_ev_list_now_23 + 8);
                        int32_t $t3_72 = $t1_87 - $t3_71;
                        
                        if ($t3_71 < ev_now_1)
                            $t0_97 = ev_now_1 - $t3_71;
                        
                        
                        if ($t1_87 < $t3_71)
                            $t3_72 = $t3_71 - $t1_87;
                        
                        $a2_65 = $t0_97 * ($a3_148 - $a2_64) / $t3_72 + $a2_64;
                    }
                    else
                    {
                        int32_t $t3_69 = *(adr_ev_list_now_23 + 4);
                        int32_t $t0_95 = $t3_69 - ev_now_1;
                        int32_t $t1_85 = *(adr_ev_list_now_23 + 8);
                        int32_t $t3_70 = $t1_85 - $t3_69;
                        
                        if ($t3_69 < ev_now_1)
                            $t0_95 = ev_now_1 - $t3_69;
                        
                        
                        if ($t1_85 < $t3_69)
                            $t3_70 = $t3_69 - $t1_85;
                        
                        $a2_65 = $a2_64 - $t0_95 * ($a2_64 - $a3_148) / $t3_70;
                    }
                    
                    *(((void**)((char*)adr_ctc_map2cut_y_now_2 + 0xc))) = $a2_65; // Fixed void pointer dereference
                    uint32_t $a2_66 = data_af228_2;
                    uint32_t $a3_155 = data_af23c_1;
                    void* adr_ev_list_now_24 = adr_ev_list_now;
                    uint32_t $a2_67;
                    
                    if ($a3_155 >= $a2_66)
                    {
                        int32_t $t3_75 = *(adr_ev_list_now_24 + 4);
                        int32_t $t0_102 = $t3_75 - ev_now_1;
                        int32_t $t1_91 = *(adr_ev_list_now_24 + 8);
                        int32_t $t3_76 = $t1_91 - $t3_75;
                        
                        if ($t3_75 < ev_now_1)
                            $t0_102 = ev_now_1 - $t3_75;
                        
                        
                        if ($t1_91 < $t3_75)
                            $t3_76 = $t3_75 - $t1_91;
                        
                        $a2_67 = $t0_102 * ($a3_155 - $a2_66) / $t3_76 + $a2_66;
                    }
                    else
                    {
                        int32_t $t3_73 = *(adr_ev_list_now_24 + 4);
                        int32_t $t0_100 = $t3_73 - ev_now_1;
                        int32_t $t1_89 = *(adr_ev_list_now_24 + 8);
                        int32_t $t3_74 = $t1_89 - $t3_73;
                        
                        if ($t3_73 < ev_now_1)
                            $t0_100 = ev_now_1 - $t3_73;
                        
                        
                        if ($t1_89 < $t3_73)
                            $t3_74 = $t3_73 - $t1_89;
                        
                        $a2_67 = $a2_66 - $t0_100 * ($a2_66 - $a3_155) / $t3_74;
                    }
                    
                    *(((void**)((char*)adr_ctc_map2cut_y_now_2 + 0x10))) = $a2_67; // Fixed void pointer dereference
                    uint32_t $a2_68 = data_af22a_2;
                    uint32_t $a3_162 = data_af23e_1;
                    void* adr_ev_list_now_25 = adr_ev_list_now;
                    uint32_t $a2_69;
                    
                    if ($a3_162 >= $a2_68)
                    {
                        int32_t $t3_79 = *(adr_ev_list_now_25 + 4);
                        int32_t $t0_107 = $t3_79 - ev_now_1;
                        int32_t $t1_95 = *(adr_ev_list_now_25 + 8);
                        int32_t $t3_80 = $t1_95 - $t3_79;
                        
                        if ($t3_79 < ev_now_1)
                            $t0_107 = ev_now_1 - $t3_79;
                        
                        
                        if ($t1_95 < $t3_79)
                            $t3_80 = $t3_79 - $t1_95;
                        
                        $a2_69 = $t0_107 * ($a3_162 - $a2_68) / $t3_80 + $a2_68;
                    }
                    else
                    {
                        int32_t $t3_77 = *(adr_ev_list_now_25 + 4);
                        int32_t $t0_105 = $t3_77 - ev_now_1;
                        int32_t $t1_93 = *(adr_ev_list_now_25 + 8);
                        int32_t $t3_78 = $t1_93 - $t3_77;
                        
                        if ($t3_77 < ev_now_1)
                            $t0_105 = ev_now_1 - $t3_77;
                        
                        
                        if ($t1_93 < $t3_77)
                            $t3_78 = $t3_77 - $t1_93;
                        
                        $a2_69 = $a2_68 - $t0_105 * ($a2_68 - $a3_162) / $t3_78;
                    }
                    
                    *(((void**)((char*)adr_ctc_map2cut_y_now_2 + 0x14))) = $a2_69; // Fixed void pointer dereference
                    uint32_t $a2_70 = data_af22c_2;
                    uint32_t $a3_169 = data_af240_1;
                    void* adr_ev_list_now_26 = adr_ev_list_now;
                    uint32_t $a2_71;
                    
                    if ($a3_169 >= $a2_70)
                    {
                        int32_t $t3_83 = *(adr_ev_list_now_26 + 4);
                        int32_t $t0_112 = $t3_83 - ev_now_1;
                        int32_t $t1_99 = *(adr_ev_list_now_26 + 8);
                        int32_t $t3_84 = $t1_99 - $t3_83;
                        
                        if ($t3_83 < ev_now_1)
                            $t0_112 = ev_now_1 - $t3_83;
                        
                        
                        if ($t1_99 < $t3_83)
                            $t3_84 = $t3_83 - $t1_99;
                        
                        $a2_71 = $t0_112 * ($a3_169 - $a2_70) / $t3_84 + $a2_70;
                    }
                    else
                    {
                        int32_t $t3_81 = *(adr_ev_list_now_26 + 4);
                        int32_t $t0_110 = $t3_81 - ev_now_1;
                        int32_t $t1_97 = *(adr_ev_list_now_26 + 8);
                        int32_t $t3_82 = $t1_97 - $t3_81;
                        
                        if ($t3_81 < ev_now_1)
                            $t0_110 = ev_now_1 - $t3_81;
                        
                        
                        if ($t1_97 < $t3_81)
                            $t3_82 = $t3_81 - $t1_97;
                        
                        $a2_71 = $a2_70 - $t0_110 * ($a2_70 - $a3_169) / $t3_82;
                    }
                    
                    *(((void**)((char*)adr_ctc_map2cut_y_now_2 + 0x18))) = $a2_71; // Fixed void pointer dereference
                    uint32_t $a2_72 = data_af22e_2;
                    uint32_t $a3_176 = data_af242_1;
                    void* adr_ev_list_now_27 = adr_ev_list_now;
                    uint32_t $a2_73;
                    
                    if ($a3_176 >= $a2_72)
                    {
                        int32_t $t3_87 = *(adr_ev_list_now_27 + 4);
                        int32_t $t0_117 = $t3_87 - ev_now_1;
                        int32_t $t1_103 = *(adr_ev_list_now_27 + 8);
                        int32_t $t3_88 = $t1_103 - $t3_87;
                        
                        if ($t3_87 < ev_now_1)
                            $t0_117 = ev_now_1 - $t3_87;
                        
                        
                        if ($t1_103 < $t3_87)
                            $t3_88 = $t3_87 - $t1_103;
                        
                        $a2_73 = $t0_117 * ($a3_176 - $a2_72) / $t3_88 + $a2_72;
                    }
                    else
                    {
                        int32_t $t3_85 = *(adr_ev_list_now_27 + 4);
                        int32_t $t0_115 = $t3_85 - ev_now_1;
                        int32_t $t1_101 = *(adr_ev_list_now_27 + 8);
                        int32_t $t3_86 = $t1_101 - $t3_85;
                        
                        if ($t3_85 < ev_now_1)
                            $t0_115 = ev_now_1 - $t3_85;
                        
                        
                        if ($t1_101 < $t3_85)
                            $t3_86 = $t3_85 - $t1_101;
                        
                        $a2_73 = $a2_72 - $t0_115 * ($a2_72 - $a3_176) / $t3_86;
                    }
                    
                    *(((void**)((char*)adr_ctc_map2cut_y_now_2 + 0x1c))) = $a2_73; // Fixed void pointer dereference
                    $a2_53 = data_af230_2;
                    uint32_t $v1_20 = data_af244_1;
                    void* adr_ev_list_now_4 = adr_ev_list_now;
                    
                    if ($v1_20 >= $a2_53)
                    {
                        int32_t $v0_5 = $a0_10 - ev_now_1;
                        $a0_10 = *(adr_ev_list_now_4 + 4);
                        
                        if ($a0_10 < ev_now_1)
                            $v0_5 = ev_now_1 - $a0_10;
                        
                        $v0_20 = $v0_5 * ($v1_20 - $a2_53);
                        $a3_130 = *(adr_ev_list_now_4 + 8);
                        goto label_5a5fc;
                    }
                    
                    $a0_9 = *(adr_ev_list_now_4 + 4);
                    int32_t $v0_4 = $a0_9 - ev_now_1;
                    
                    if ($a0_9 < ev_now_1)
                        $v0_4 = ev_now_1 - $a0_9;
                    
                    $v0_17 = $v0_4 * ($a2_53 - $v1_20);
                    $a3_128 = *(adr_ev_list_now_4 + 8);
                    goto label_5a5bc;
                }
                
                int32_t $t2_39 = *(adr_ev_list_now_2 + 0xc);
                
                if ($t2_39 >= ev_now_1)
                {
                    uint32_t $t4_3 = data_af234;
                    uint32_t $a3_187 = data_af24c;
                    int32_t $a2_74 = ev_now_1 - $t1_42;
                    int32_t $t3_89 = $t2_39 < $t1_42 ? 1 : 0;
                        int32_t $t1_108 = $t2_39 - $t1_42;
                    uint32_t $a3_190;
                    
                    if ($a3_187 >= $t4_3)
                    {
                        
                        if ($t3_89)
                            $t1_108 = $t1_42 - $t2_39;
                        
                        $a3_190 = ($a3_187 - $t4_3) * $a2_74 / $t1_108 + $t4_3;
                    }
                    else
                    {
                        int32_t $t1_107 = $t2_39 - $t1_42;
                        
                        if ($t3_89)
                            $t1_107 = $t1_42 - $t2_39;
                        
                        $a3_190 = $t4_3 - ($t4_3 - $a3_187) * $a2_74 / $t1_107;
                    }
                    
                    **&adr_ctc_map2cut_y_now = $a3_190;
                    uint32_t $a2_76 = data_af236_2;
                    uint32_t $a3_193 = data_af24e_1;
                    adr_ctc_map2cut_y_now_2 = adr_ctc_map2cut_y_now;
                    uint32_t $a2_77;
                    
                    if ($a3_193 >= $a2_76)
                    {
                        void* adr_ev_list_now_29 = adr_ev_list_now;
                        int32_t $t3_92 = *(adr_ev_list_now_29 + 8);
                        int32_t $t0_126 = $t3_92 - ev_now_1;
                        int32_t $t1_112 = *(adr_ev_list_now_29 + 0xc);
                        int32_t $t3_93 = $t1_112 - $t3_92;
                        
                        if ($t3_92 < ev_now_1)
                            $t0_126 = ev_now_1 - $t3_92;
                        
                        
                        if ($t1_112 < $t3_92)
                            $t3_93 = $t3_92 - $t1_112;
                        
                        $a2_77 = $t0_126 * ($a3_193 - $a2_76) / $t3_93 + $a2_76;
                    }
                    else
                    {
                        void* adr_ev_list_now_28 = adr_ev_list_now;
                        int32_t $t3_90 = *(adr_ev_list_now_28 + 8);
                        int32_t $t0_124 = $t3_90 - ev_now_1;
                        int32_t $t1_110 = *(adr_ev_list_now_28 + 0xc);
                        int32_t $t3_91 = $t1_110 - $t3_90;
                        
                        if ($t3_90 < ev_now_1)
                            $t0_124 = ev_now_1 - $t3_90;
                        
                        
                        if ($t1_110 < $t3_90)
                            $t3_91 = $t3_90 - $t1_110;
                        
                        $a2_77 = $a2_76 - $t0_124 * ($a2_76 - $a3_193) / $t3_91;
                    }
                    
                    *(((void**)((char*)adr_ctc_map2cut_y_now_2 + 4))) = $a2_77; // Fixed void pointer dereference
                    uint32_t $a2_78 = data_af238_2;
                    uint32_t $a3_200 = data_af250_1;
                    void* adr_ev_list_now_30 = adr_ev_list_now;
                    uint32_t $a2_79;
                    
                    if ($a3_200 >= $a2_78)
                    {
                        int32_t $t3_96 = *(adr_ev_list_now_30 + 8);
                        int32_t $t0_131 = $t3_96 - ev_now_1;
                        int32_t $t1_116 = *(adr_ev_list_now_30 + 0xc);
                        int32_t $t3_97 = $t1_116 - $t3_96;
                        
                        if ($t3_96 < ev_now_1)
                            $t0_131 = ev_now_1 - $t3_96;
                        
                        
                        if ($t1_116 < $t3_96)
                            $t3_97 = $t3_96 - $t1_116;
                        
                        $a2_79 = $t0_131 * ($a3_200 - $a2_78) / $t3_97 + $a2_78;
                    }
                    else
                    {
                        int32_t $t3_94 = *(adr_ev_list_now_30 + 8);
                        int32_t $t0_129 = $t3_94 - ev_now_1;
                        int32_t $t1_114 = *(adr_ev_list_now_30 + 0xc);
                        int32_t $t3_95 = $t1_114 - $t3_94;
                        
                        if ($t3_94 < ev_now_1)
                            $t0_129 = ev_now_1 - $t3_94;
                        
                        
                        if ($t1_114 < $t3_94)
                            $t3_95 = $t3_94 - $t1_114;
                        
                        $a2_79 = $a2_78 - $t0_129 * ($a2_78 - $a3_200) / $t3_95;
                    }
                    
                    *(((void**)((char*)adr_ctc_map2cut_y_now_2 + 8))) = $a2_79; // Fixed void pointer dereference
                    uint32_t $a2_80 = data_af23a_2;
                    uint32_t $a3_207 = data_af252_1;
                    void* adr_ev_list_now_31 = adr_ev_list_now;
                    uint32_t $a2_81;
                    
                    if ($a3_207 >= $a2_80)
                    {
                        int32_t $t3_100 = *(adr_ev_list_now_31 + 8);
                        int32_t $t0_136 = $t3_100 - ev_now_1;
                        int32_t $t1_120 = *(adr_ev_list_now_31 + 0xc);
                        int32_t $t3_101 = $t1_120 - $t3_100;
                        
                        if ($t3_100 < ev_now_1)
                            $t0_136 = ev_now_1 - $t3_100;
                        
                        
                        if ($t1_120 < $t3_100)
                            $t3_101 = $t3_100 - $t1_120;
                        
                        $a2_81 = $t0_136 * ($a3_207 - $a2_80) / $t3_101 + $a2_80;
                    }
                    else
                    {
                        int32_t $t3_98 = *(adr_ev_list_now_31 + 8);
                        int32_t $t0_134 = $t3_98 - ev_now_1;
                        int32_t $t1_118 = *(adr_ev_list_now_31 + 0xc);
                        int32_t $t3_99 = $t1_118 - $t3_98;
                        
                        if ($t3_98 < ev_now_1)
                            $t0_134 = ev_now_1 - $t3_98;
                        
                        
                        if ($t1_118 < $t3_98)
                            $t3_99 = $t3_98 - $t1_118;
                        
                        $a2_81 = $a2_80 - $t0_134 * ($a2_80 - $a3_207) / $t3_99;
                    }
                    
                    *(((void**)((char*)adr_ctc_map2cut_y_now_2 + 0xc))) = $a2_81; // Fixed void pointer dereference
                    uint32_t $a2_82 = data_af23c_2;
                    uint32_t $a3_214 = data_af254_1;
                    void* adr_ev_list_now_32 = adr_ev_list_now;
                    uint32_t $a2_83;
                    
                    if ($a3_214 >= $a2_82)
                    {
                        int32_t $t3_104 = *(adr_ev_list_now_32 + 8);
                        int32_t $t0_141 = $t3_104 - ev_now_1;
                        int32_t $t1_124 = *(adr_ev_list_now_32 + 0xc);
                        int32_t $t3_105 = $t1_124 - $t3_104;
                        
                        if ($t3_104 < ev_now_1)
                            $t0_141 = ev_now_1 - $t3_104;
                        
                        
                        if ($t1_124 < $t3_104)
                            $t3_105 = $t3_104 - $t1_124;
                        
                        $a2_83 = $t0_141 * ($a3_214 - $a2_82) / $t3_105 + $a2_82;
                    }
                    else
                    {
                        int32_t $t3_102 = *(adr_ev_list_now_32 + 8);
                        int32_t $t0_139 = $t3_102 - ev_now_1;
                        int32_t $t1_122 = *(adr_ev_list_now_32 + 0xc);
                        int32_t $t3_103 = $t1_122 - $t3_102;
                        
                        if ($t3_102 < ev_now_1)
                            $t0_139 = ev_now_1 - $t3_102;
                        
                        
                        if ($t1_122 < $t3_102)
                            $t3_103 = $t3_102 - $t1_122;
                        
                        $a2_83 = $a2_82 - $t0_139 * ($a2_82 - $a3_214) / $t3_103;
                    }
                    
                    *(((void**)((char*)adr_ctc_map2cut_y_now_2 + 0x10))) = $a2_83; // Fixed void pointer dereference
                    uint32_t $a2_84 = data_af23e_2;
                    uint32_t $a3_221 = data_af256_1;
                    void* adr_ev_list_now_33 = adr_ev_list_now;
                    uint32_t $a2_85;
                    
                    if ($a3_221 >= $a2_84)
                    {
                        int32_t $t3_108 = *(adr_ev_list_now_33 + 8);
                        int32_t $t0_146 = $t3_108 - ev_now_1;
                        int32_t $t1_128 = *(adr_ev_list_now_33 + 0xc);
                        int32_t $t3_109 = $t1_128 - $t3_108;
                        
                        if ($t3_108 < ev_now_1)
                            $t0_146 = ev_now_1 - $t3_108;
                        
                        
                        if ($t1_128 < $t3_108)
                            $t3_109 = $t3_108 - $t1_128;
                        
                        $a2_85 = $t0_146 * ($a3_221 - $a2_84) / $t3_109 + $a2_84;
                    }
                    else
                    {
                        int32_t $t3_106 = *(adr_ev_list_now_33 + 8);
                        int32_t $t0_144 = $t3_106 - ev_now_1;
                        int32_t $t1_126 = *(adr_ev_list_now_33 + 0xc);
                        int32_t $t3_107 = $t1_126 - $t3_106;
                        
                        if ($t3_106 < ev_now_1)
                            $t0_144 = ev_now_1 - $t3_106;
                        
                        
                        if ($t1_126 < $t3_106)
                            $t3_107 = $t3_106 - $t1_126;
                        
                        $a2_85 = $a2_84 - $t0_144 * ($a2_84 - $a3_221) / $t3_107;
                    }
                    
                    *(((void**)((char*)adr_ctc_map2cut_y_now_2 + 0x14))) = $a2_85; // Fixed void pointer dereference
                    uint32_t $a2_86 = data_af240_2;
                    uint32_t $a3_228 = data_af258_1;
                    void* adr_ev_list_now_34 = adr_ev_list_now;
                    uint32_t $a2_87;
                    
                    if ($a3_228 >= $a2_86)
                    {
                        int32_t $t3_112 = *(adr_ev_list_now_34 + 8);
                        int32_t $t0_151 = $t3_112 - ev_now_1;
                        int32_t $t1_132 = *(adr_ev_list_now_34 + 0xc);
                        int32_t $t3_113 = $t1_132 - $t3_112;
                        
                        if ($t3_112 < ev_now_1)
                            $t0_151 = ev_now_1 - $t3_112;
                        
                        
                        if ($t1_132 < $t3_112)
                            $t3_113 = $t3_112 - $t1_132;
                        
                        $a2_87 = $t0_151 * ($a3_228 - $a2_86) / $t3_113 + $a2_86;
                    }
                    else
                    {
                        int32_t $t3_110 = *(adr_ev_list_now_34 + 8);
                        int32_t $t0_149 = $t3_110 - ev_now_1;
                        int32_t $t1_130 = *(adr_ev_list_now_34 + 0xc);
                        int32_t $t3_111 = $t1_130 - $t3_110;
                        
                        if ($t3_110 < ev_now_1)
                            $t0_149 = ev_now_1 - $t3_110;
                        
                        
                        if ($t1_130 < $t3_110)
                            $t3_111 = $t3_110 - $t1_130;
                        
                        $a2_87 = $a2_86 - $t0_149 * ($a2_86 - $a3_228) / $t3_111;
                    }
                    
                    *(((void**)((char*)adr_ctc_map2cut_y_now_2 + 0x18))) = $a2_87; // Fixed void pointer dereference
                    uint32_t $a2_88 = data_af242_2;
                    uint32_t $a3_235 = data_af25a_1;
                    void* adr_ev_list_now_35 = adr_ev_list_now;
                    uint32_t $a2_89;
                    
                    if ($a3_235 >= $a2_88)
                    {
                        int32_t $t3_116 = *(adr_ev_list_now_35 + 8);
                        int32_t $t0_156 = $t3_116 - ev_now_1;
                        int32_t $t1_136 = *(adr_ev_list_now_35 + 0xc);
                        int32_t $t3_117 = $t1_136 - $t3_116;
                        
                        if ($t3_116 < ev_now_1)
                            $t0_156 = ev_now_1 - $t3_116;
                        
                        
                        if ($t1_136 < $t3_116)
                            $t3_117 = $t3_116 - $t1_136;
                        
                        $a2_89 = $t0_156 * ($a3_235 - $a2_88) / $t3_117 + $a2_88;
                    }
                    else
                    {
                        int32_t $t3_114 = *(adr_ev_list_now_35 + 8);
                        int32_t $t0_154 = $t3_114 - ev_now_1;
                        int32_t $t1_134 = *(adr_ev_list_now_35 + 0xc);
                        int32_t $t3_115 = $t1_134 - $t3_114;
                        
                        if ($t3_114 < ev_now_1)
                            $t0_154 = ev_now_1 - $t3_114;
                        
                        
                        if ($t1_134 < $t3_114)
                            $t3_115 = $t3_114 - $t1_134;
                        
                        $a2_89 = $a2_88 - $t0_154 * ($a2_88 - $a3_235) / $t3_115;
                    }
                    
                    *(((void**)((char*)adr_ctc_map2cut_y_now_2 + 0x1c))) = $a2_89; // Fixed void pointer dereference
                    $a2_53 = data_af244_2;
                    uint32_t $v1_23 = data_af25c_1[0];
                    void* adr_ev_list_now_5 = adr_ev_list_now;
                    
                    if ($v1_23 >= $a2_53)
                    {
                        int32_t $v0_7 = $a0_10 - ev_now_1;
                        $a0_10 = *(adr_ev_list_now_5 + 8);
                        
                        if ($a0_10 < ev_now_1)
                            $v0_7 = ev_now_1 - $a0_10;
                        
                        $v0_20 = $v0_7 * ($v1_23 - $a2_53);
                        $a3_130 = *(adr_ev_list_now_5 + 0xc);
                        goto label_5a5fc;
                    }
                    
                    $a0_9 = *(adr_ev_list_now_5 + 8);
                    int32_t $v0_6 = $a0_9 - ev_now_1;
                    
                    if ($a0_9 < ev_now_1)
                        $v0_6 = ev_now_1 - $a0_9;
                    
                    $v0_17 = $v0_6 * ($a2_53 - $v1_23);
                    $a3_128 = *(adr_ev_list_now_5 + 0xc);
                    goto label_5a5bc;
                }
                
                int32_t $t1_106 = *(adr_ev_list_now_2 + 0x10);
                
                if ($t1_106 >= ev_now_1)
                {
                    uint32_t $t4_4 = data_af24c;
                    uint32_t $a2_90 = data_af25c[4][0];
                    int32_t $a3_246 = ev_now_1 - $t2_39;
                    int32_t $t3_118 = $t1_106 < $t2_39 ? 1 : 0;
                        int32_t $t1_140 = $t1_106 - $t2_39;
                    uint32_t $a2_93;
                    
                    if ($a2_90 >= $t4_4)
                    {
                        
                        if ($t3_118)
                            $t1_140 = $t2_39 - $t1_106;
                        
                        $a2_93 = ($a2_90 - $t4_4) * $a3_246 / $t1_140 + $t4_4;
                    }
                    else
                    {
                        int32_t $t1_139 = $t1_106 - $t2_39;
                        
                        if ($t3_118)
                            $t1_139 = $t2_39 - $t1_106;
                        
                        $a2_93 = $t4_4 - ($t4_4 - $a2_90) * $a3_246 / $t1_139;
                    }
                    
                    **&adr_ctc_map2cut_y_now = $a2_93;
                    uint32_t $a2_96 = data_af24e_2;
                    uint32_t $a3_248 = data_af25c_2[6][0];
                    adr_ctc_map2cut_y_now_2 = adr_ctc_map2cut_y_now;
                    uint32_t $a2_97;
                    
                    if ($a3_248 >= $a2_96)
                    {
                        void* adr_ev_list_now_37 = adr_ev_list_now;
                        int32_t $t3_121 = *(adr_ev_list_now_37 + 0xc);
                        int32_t $t0_165 = $t3_121 - ev_now_1;
                        int32_t $t1_144 = *(adr_ev_list_now_37 + 0x10);
                        int32_t $t3_122 = $t1_144 - $t3_121;
                        
                        if ($t3_121 < ev_now_1)
                            $t0_165 = ev_now_1 - $t3_121;
                        
                        
                        if ($t1_144 < $t3_121)
                            $t3_122 = $t3_121 - $t1_144;
                        
                        $a2_97 = $t0_165 * ($a3_248 - $a2_96) / $t3_122 + $a2_96;
                    }
                    else
                    {
                        void* adr_ev_list_now_36 = adr_ev_list_now;
                        int32_t $t3_119 = *(adr_ev_list_now_36 + 0xc);
                        int32_t $t0_163 = $t3_119 - ev_now_1;
                        int32_t $t1_142 = *(adr_ev_list_now_36 + 0x10);
                        int32_t $t3_120 = $t1_142 - $t3_119;
                        
                        if ($t3_119 < ev_now_1)
                            $t0_163 = ev_now_1 - $t3_119;
                        
                        
                        if ($t1_142 < $t3_119)
                            $t3_120 = $t3_119 - $t1_142;
                        
                        $a2_97 = $a2_96 - $t0_163 * ($a2_96 - $a3_248) / $t3_120;
                    }
                    
                    *(((void**)((char*)adr_ctc_map2cut_y_now_2 + 4))) = $a2_97; // Fixed void pointer dereference
                    uint32_t $a2_98 = data_af250_2;
                    uint32_t $a3_255 = data_af264_1;
                    void* adr_ev_list_now_38 = adr_ev_list_now;
                    uint32_t $a2_99;
                    
                    if ($a3_255 >= $a2_98)
                    {
                        int32_t $t3_125 = *(adr_ev_list_now_38 + 0xc);
                        int32_t $t0_170 = $t3_125 - ev_now_1;
                        int32_t $t1_148 = *(adr_ev_list_now_38 + 0x10);
                        int32_t $t3_126 = $t1_148 - $t3_125;
                        
                        if ($t3_125 < ev_now_1)
                            $t0_170 = ev_now_1 - $t3_125;
                        
                        
                        if ($t1_148 < $t3_125)
                            $t3_126 = $t3_125 - $t1_148;
                        
                        $a2_99 = $t0_170 * ($a3_255 - $a2_98) / $t3_126 + $a2_98;
                    }
                    else
                    {
                        int32_t $t3_123 = *(adr_ev_list_now_38 + 0xc);
                        int32_t $t0_168 = $t3_123 - ev_now_1;
                        int32_t $t1_146 = *(adr_ev_list_now_38 + 0x10);
                        int32_t $t3_124 = $t1_146 - $t3_123;
                        
                        if ($t3_123 < ev_now_1)
                            $t0_168 = ev_now_1 - $t3_123;
                        
                        
                        if ($t1_146 < $t3_123)
                            $t3_124 = $t3_123 - $t1_146;
                        
                        $a2_99 = $a2_98 - $t0_168 * ($a2_98 - $a3_255) / $t3_124;
                    }
                    
                    *(((void**)((char*)adr_ctc_map2cut_y_now_2 + 8))) = $a2_99; // Fixed void pointer dereference
                    uint32_t $a2_100 = data_af252_2;
                    uint32_t $a3_262 = data_af266_1;
                    void* adr_ev_list_now_39 = adr_ev_list_now;
                    uint32_t $a2_101;
                    
                    if ($a3_262 >= $a2_100)
                    {
                        int32_t $t3_129 = *(adr_ev_list_now_39 + 0xc);
                        int32_t $t0_175 = $t3_129 - ev_now_1;
                        int32_t $t1_152 = *(adr_ev_list_now_39 + 0x10);
                        int32_t $t3_130 = $t1_152 - $t3_129;
                        
                        if ($t3_129 < ev_now_1)
                            $t0_175 = ev_now_1 - $t3_129;
                        
                        
                        if ($t1_152 < $t3_129)
                            $t3_130 = $t3_129 - $t1_152;
                        
                        $a2_101 = $t0_175 * ($a3_262 - $a2_100) / $t3_130 + $a2_100;
                    }
                    else
                    {
                        int32_t $t3_127 = *(adr_ev_list_now_39 + 0xc);
                        int32_t $t0_173 = $t3_127 - ev_now_1;
                        int32_t $t1_150 = *(adr_ev_list_now_39 + 0x10);
                        int32_t $t3_128 = $t1_150 - $t3_127;
                        
                        if ($t3_127 < ev_now_1)
                            $t0_173 = ev_now_1 - $t3_127;
                        
                        
                        if ($t1_150 < $t3_127)
                            $t3_128 = $t3_127 - $t1_150;
                        
                        $a2_101 = $a2_100 - $t0_173 * ($a2_100 - $a3_262) / $t3_128;
                    }
                    
                    *(((void**)((char*)adr_ctc_map2cut_y_now_2 + 0xc))) = $a2_101; // Fixed void pointer dereference
                    uint32_t $a2_102 = data_af254_2;
                    uint32_t $a3_269 = data_af268_1;
                    void* adr_ev_list_now_40 = adr_ev_list_now;
                    uint32_t $a2_103;
                    
                    if ($a3_269 >= $a2_102)
                    {
                        int32_t $t3_133 = *(adr_ev_list_now_40 + 0xc);
                        int32_t $t0_180 = $t3_133 - ev_now_1;
                        int32_t $t1_156 = *(adr_ev_list_now_40 + 0x10);
                        int32_t $t3_134 = $t1_156 - $t3_133;
                        
                        if ($t3_133 < ev_now_1)
                            $t0_180 = ev_now_1 - $t3_133;
                        
                        
                        if ($t1_156 < $t3_133)
                            $t3_134 = $t3_133 - $t1_156;
                        
                        $a2_103 = $t0_180 * ($a3_269 - $a2_102) / $t3_134 + $a2_102;
                    }
                    else
                    {
                        int32_t $t3_131 = *(adr_ev_list_now_40 + 0xc);
                        int32_t $t0_178 = $t3_131 - ev_now_1;
                        int32_t $t1_154 = *(adr_ev_list_now_40 + 0x10);
                        int32_t $t3_132 = $t1_154 - $t3_131;
                        
                        if ($t3_131 < ev_now_1)
                            $t0_178 = ev_now_1 - $t3_131;
                        
                        
                        if ($t1_154 < $t3_131)
                            $t3_132 = $t3_131 - $t1_154;
                        
                        $a2_103 = $a2_102 - $t0_178 * ($a2_102 - $a3_269) / $t3_132;
                    }
                    
                    *(((void**)((char*)adr_ctc_map2cut_y_now_2 + 0x10))) = $a2_103; // Fixed void pointer dereference
                    uint32_t $a2_104 = data_af256_2;
                    uint32_t $a3_276 = data_af26a_1;
                    void* adr_ev_list_now_41 = adr_ev_list_now;
                    uint32_t $a2_105;
                    
                    if ($a3_276 >= $a2_104)
                    {
                        int32_t $t3_137 = *(adr_ev_list_now_41 + 0xc);
                        int32_t $t0_185 = $t3_137 - ev_now_1;
                        int32_t $t1_160 = *(adr_ev_list_now_41 + 0x10);
                        int32_t $t3_138 = $t1_160 - $t3_137;
                        
                        if ($t3_137 < ev_now_1)
                            $t0_185 = ev_now_1 - $t3_137;
                        
                        
                        if ($t1_160 < $t3_137)
                            $t3_138 = $t3_137 - $t1_160;
                        
                        $a2_105 = $t0_185 * ($a3_276 - $a2_104) / $t3_138 + $a2_104;
                    }
                    else
                    {
                        int32_t $t3_135 = *(adr_ev_list_now_41 + 0xc);
                        int32_t $t0_183 = $t3_135 - ev_now_1;
                        int32_t $t1_158 = *(adr_ev_list_now_41 + 0x10);
                        int32_t $t3_136 = $t1_158 - $t3_135;
                        
                        if ($t3_135 < ev_now_1)
                            $t0_183 = ev_now_1 - $t3_135;
                        
                        
                        if ($t1_158 < $t3_135)
                            $t3_136 = $t3_135 - $t1_158;
                        
                        $a2_105 = $a2_104 - $t0_183 * ($a2_104 - $a3_276) / $t3_136;
                    }
                    
                    *(((void**)((char*)adr_ctc_map2cut_y_now_2 + 0x14))) = $a2_105; // Fixed void pointer dereference
                    uint32_t $a2_106 = data_af258_2;
                    uint32_t $a3_283 = data_af26c_1;
                    void* adr_ev_list_now_42 = adr_ev_list_now;
                    uint32_t $a2_107;
                    
                    if ($a3_283 >= $a2_106)
                    {
                        int32_t $t3_141 = *(adr_ev_list_now_42 + 0xc);
                        int32_t $t0_190 = $t3_141 - ev_now_1;
                        int32_t $t1_164 = *(adr_ev_list_now_42 + 0x10);
                        int32_t $t3_142 = $t1_164 - $t3_141;
                        
                        if ($t3_141 < ev_now_1)
                            $t0_190 = ev_now_1 - $t3_141;
                        
                        
                        if ($t1_164 < $t3_141)
                            $t3_142 = $t3_141 - $t1_164;
                        
                        $a2_107 = $t0_190 * ($a3_283 - $a2_106) / $t3_142 + $a2_106;
                    }
                    else
                    {
                        int32_t $t3_139 = *(adr_ev_list_now_42 + 0xc);
                        int32_t $t0_188 = $t3_139 - ev_now_1;
                        int32_t $t1_162 = *(adr_ev_list_now_42 + 0x10);
                        int32_t $t3_140 = $t1_162 - $t3_139;
                        
                        if ($t3_139 < ev_now_1)
                            $t0_188 = ev_now_1 - $t3_139;
                        
                        
                        if ($t1_162 < $t3_139)
                            $t3_140 = $t3_139 - $t1_162;
                        
                        $a2_107 = $a2_106 - $t0_188 * ($a2_106 - $a3_283) / $t3_140;
                    }
                    
                    *(((void**)((char*)adr_ctc_map2cut_y_now_2 + 0x18))) = $a2_107; // Fixed void pointer dereference
                    uint32_t $a2_108 = data_af25a_2;
                    uint32_t $a3_290 = data_af26e_1[0];
                    void* adr_ev_list_now_43 = adr_ev_list_now;
                    uint32_t $a2_109;
                    
                    if ($a3_290 >= $a2_108)
                    {
                        int32_t $t3_145 = *(adr_ev_list_now_43 + 0xc);
                        int32_t $t0_195 = $t3_145 - ev_now_1;
                        int32_t $t1_168 = *(adr_ev_list_now_43 + 0x10);
                        int32_t $t3_146 = $t1_168 - $t3_145;
                        
                        if ($t3_145 < ev_now_1)
                            $t0_195 = ev_now_1 - $t3_145;
                        
                        
                        if ($t1_168 < $t3_145)
                            $t3_146 = $t3_145 - $t1_168;
                        
                        $a2_109 = $t0_195 * ($a3_290 - $a2_108) / $t3_146 + $a2_108;
                    }
                    else
                    {
                        int32_t $t3_143 = *(adr_ev_list_now_43 + 0xc);
                        int32_t $t0_193 = $t3_143 - ev_now_1;
                        int32_t $t1_166 = *(adr_ev_list_now_43 + 0x10);
                        int32_t $t3_144 = $t1_166 - $t3_143;
                        
                        if ($t3_143 < ev_now_1)
                            $t0_193 = ev_now_1 - $t3_143;
                        
                        
                        if ($t1_166 < $t3_143)
                            $t3_144 = $t3_143 - $t1_166;
                        
                        $a2_109 = $a2_108 - $t0_193 * ($a2_108 - $a3_290) / $t3_144;
                    }
                    
                    *(((void**)((char*)adr_ctc_map2cut_y_now_2 + 0x1c))) = $a2_109; // Fixed void pointer dereference
                    $a2_53 = data_af25c_3[0];
                    uint32_t $v1_26 = data_af26e_2[2][0];
                    void* adr_ev_list_now_6 = adr_ev_list_now;
                    
                    if ($v1_26 >= $a2_53)
                    {
                        int32_t $v0_9 = $a0_10 - ev_now_1;
                        $a0_10 = *(adr_ev_list_now_6 + 0xc);
                        
                        if ($a0_10 < ev_now_1)
                            $v0_9 = ev_now_1 - $a0_10;
                        
                        $v0_20 = $v0_9 * ($v1_26 - $a2_53);
                        $a3_130 = *(adr_ev_list_now_6 + 0x10);
                        goto label_5a5fc;
                    }
                    
                    $a0_9 = *(adr_ev_list_now_6 + 0xc);
                    int32_t $v0_8 = $a0_9 - ev_now_1;
                    
                    if ($a0_9 < ev_now_1)
                        $v0_8 = ev_now_1 - $a0_9;
                    
                    $v0_17 = $v0_8 * ($a2_53 - $v1_26);
                    $a3_128 = *(adr_ev_list_now_6 + 0x10);
                    goto label_5a5bc;
                }
                
                int32_t $t2_96 = *(adr_ev_list_now_2 + 0x14);
                
                if ($t2_96 >= ev_now_1)
                {
                    uint32_t $t4_5 = data_af25c[4][0];
                    uint32_t $a3_301 = data_af26e[6][0];
                    int32_t $a2_110 = ev_now_1 - $t1_106;
                    int32_t $t3_147 = $t2_96 < $t1_106 ? 1 : 0;
                        int32_t $t1_173 = $t2_96 - $t1_106;
                    uint32_t $a3_304;
                    
                    if ($a3_301 >= $t4_5)
                    {
                        
                        if ($t3_147)
                            $t1_173 = $t1_106 - $t2_96;
                        
                        $a3_304 = ($a3_301 - $t4_5) * $a2_110 / $t1_173 + $t4_5;
                    }
                    else
                    {
                        int32_t $t1_172 = $t2_96 - $t1_106;
                        
                        if ($t3_147)
                            $t1_172 = $t1_106 - $t2_96;
                        
                        $a3_304 = $t4_5 - ($t4_5 - $a3_301) * $a2_110 / $t1_172;
                    }
                    
                    **&adr_ctc_map2cut_y_now = $a3_304;
                    uint32_t $a2_112 = data_af25c_4[6][0];
                    uint32_t $a3_307 = data_af26e_3[8][0];
                    adr_ctc_map2cut_y_now_2 = adr_ctc_map2cut_y_now;
                    uint32_t $a2_113;
                    
                    if ($a3_307 >= $a2_112)
                    {
                        void* adr_ev_list_now_45 = adr_ev_list_now;
                        int32_t $t3_150 = *(adr_ev_list_now_45 + 0x10);
                        int32_t $t0_204 = $t3_150 - ev_now_1;
                        int32_t $t1_177 = *(adr_ev_list_now_45 + 0x14);
                        int32_t $t3_151 = $t1_177 - $t3_150;
                        
                        if ($t3_150 < ev_now_1)
                            $t0_204 = ev_now_1 - $t3_150;
                        
                        
                        if ($t1_177 < $t3_150)
                            $t3_151 = $t3_150 - $t1_177;
                        
                        $a2_113 = $t0_204 * ($a3_307 - $a2_112) / $t3_151 + $a2_112;
                    }
                    else
                    {
                        void* adr_ev_list_now_44 = adr_ev_list_now;
                        int32_t $t3_148 = *(adr_ev_list_now_44 + 0x10);
                        int32_t $t0_202 = $t3_148 - ev_now_1;
                        int32_t $t1_175 = *(adr_ev_list_now_44 + 0x14);
                        int32_t $t3_149 = $t1_175 - $t3_148;
                        
                        if ($t3_148 < ev_now_1)
                            $t0_202 = ev_now_1 - $t3_148;
                        
                        
                        if ($t1_175 < $t3_148)
                            $t3_149 = $t3_148 - $t1_175;
                        
                        $a2_113 = $a2_112 - $t0_202 * ($a2_112 - $a3_307) / $t3_149;
                    }
                    
                    *(((void**)((char*)adr_ctc_map2cut_y_now_2 + 4))) = $a2_113; // Fixed void pointer dereference
                    uint32_t $a2_114 = data_af264_2;
                    uint32_t $a3_314 = data_af278_1;
                    void* adr_ev_list_now_46 = adr_ev_list_now;
                    uint32_t $a2_115;
                    
                    if ($a3_314 >= $a2_114)
                    {
                        int32_t $t3_154 = *(adr_ev_list_now_46 + 0x10);
                        int32_t $t0_209 = $t3_154 - ev_now_1;
                        int32_t $t1_181 = *(adr_ev_list_now_46 + 0x14);
                        int32_t $t3_155 = $t1_181 - $t3_154;
                        
                        if ($t3_154 < ev_now_1)
                            $t0_209 = ev_now_1 - $t3_154;
                        
                        
                        if ($t1_181 < $t3_154)
                            $t3_155 = $t3_154 - $t1_181;
                        
                        $a2_115 = $t0_209 * ($a3_314 - $a2_114) / $t3_155 + $a2_114;
                    }
                    else
                    {
                        int32_t $t3_152 = *(adr_ev_list_now_46 + 0x10);
                        int32_t $t0_207 = $t3_152 - ev_now_1;
                        int32_t $t1_179 = *(adr_ev_list_now_46 + 0x14);
                        int32_t $t3_153 = $t1_179 - $t3_152;
                        
                        if ($t3_152 < ev_now_1)
                            $t0_207 = ev_now_1 - $t3_152;
                        
                        
                        if ($t1_179 < $t3_152)
                            $t3_153 = $t3_152 - $t1_179;
                        
                        $a2_115 = $a2_114 - $t0_207 * ($a2_114 - $a3_314) / $t3_153;
                    }
                    
                    *(((void**)((char*)adr_ctc_map2cut_y_now_2 + 8))) = $a2_115; // Fixed void pointer dereference
                    uint32_t $a2_116 = data_af266_2;
                    uint32_t $a3_321 = data_af27a_1;
                    void* adr_ev_list_now_47 = adr_ev_list_now;
                    uint32_t $a2_117;
                    
                    if ($a3_321 >= $a2_116)
                    {
                        int32_t $t3_158 = *(adr_ev_list_now_47 + 0x10);
                        int32_t $t0_214 = $t3_158 - ev_now_1;
                        int32_t $t1_185 = *(adr_ev_list_now_47 + 0x14);
                        int32_t $t3_159 = $t1_185 - $t3_158;
                        
                        if ($t3_158 < ev_now_1)
                            $t0_214 = ev_now_1 - $t3_158;
                        
                        
                        if ($t1_185 < $t3_158)
                            $t3_159 = $t3_158 - $t1_185;
                        
                        $a2_117 = $t0_214 * ($a3_321 - $a2_116) / $t3_159 + $a2_116;
                    }
                    else
                    {
                        int32_t $t3_156 = *(adr_ev_list_now_47 + 0x10);
                        int32_t $t0_212 = $t3_156 - ev_now_1;
                        int32_t $t1_183 = *(adr_ev_list_now_47 + 0x14);
                        int32_t $t3_157 = $t1_183 - $t3_156;
                        
                        if ($t3_156 < ev_now_1)
                            $t0_212 = ev_now_1 - $t3_156;
                        
                        
                        if ($t1_183 < $t3_156)
                            $t3_157 = $t3_156 - $t1_183;
                        
                        $a2_117 = $a2_116 - $t0_212 * ($a2_116 - $a3_321) / $t3_157;
                    }
                    
                    *(((void**)((char*)adr_ctc_map2cut_y_now_2 + 0xc))) = $a2_117; // Fixed void pointer dereference
                    uint32_t $a2_118 = data_af268_2;
                    uint32_t $a3_328 = data_af27c_1;
                    void* adr_ev_list_now_48 = adr_ev_list_now;
                    uint32_t $a2_119;
                    
                    if ($a3_328 >= $a2_118)
                    {
                        int32_t $t3_162 = *(adr_ev_list_now_48 + 0x10);
                        int32_t $t0_219 = $t3_162 - ev_now_1;
                        int32_t $t1_189 = *(adr_ev_list_now_48 + 0x14);
                        int32_t $t3_163 = $t1_189 - $t3_162;
                        
                        if ($t3_162 < ev_now_1)
                            $t0_219 = ev_now_1 - $t3_162;
                        
                        
                        if ($t1_189 < $t3_162)
                            $t3_163 = $t3_162 - $t1_189;
                        
                        $a2_119 = $t0_219 * ($a3_328 - $a2_118) / $t3_163 + $a2_118;
                    }
                    else
                    {
                        int32_t $t3_160 = *(adr_ev_list_now_48 + 0x10);
                        int32_t $t0_217 = $t3_160 - ev_now_1;
                        int32_t $t1_187 = *(adr_ev_list_now_48 + 0x14);
                        int32_t $t3_161 = $t1_187 - $t3_160;
                        
                        if ($t3_160 < ev_now_1)
                            $t0_217 = ev_now_1 - $t3_160;
                        
                        
                        if ($t1_187 < $t3_160)
                            $t3_161 = $t3_160 - $t1_187;
                        
                        $a2_119 = $a2_118 - $t0_217 * ($a2_118 - $a3_328) / $t3_161;
                    }
                    
                    *(((void**)((char*)adr_ctc_map2cut_y_now_2 + 0x10))) = $a2_119; // Fixed void pointer dereference
                    uint32_t $a2_120 = data_af26a_2;
                    uint32_t $a3_335 = data_af27e_1;
                    void* adr_ev_list_now_49 = adr_ev_list_now;
                    uint32_t $a2_121;
                    
                    if ($a3_335 >= $a2_120)
                    {
                        int32_t $t3_166 = *(adr_ev_list_now_49 + 0x10);
                        int32_t $t0_224 = $t3_166 - ev_now_1;
                        int32_t $t1_193 = *(adr_ev_list_now_49 + 0x14);
                        int32_t $t3_167 = $t1_193 - $t3_166;
                        
                        if ($t3_166 < ev_now_1)
                            $t0_224 = ev_now_1 - $t3_166;
                        
                        
                        if ($t1_193 < $t3_166)
                            $t3_167 = $t3_166 - $t1_193;
                        
                        $a2_121 = $t0_224 * ($a3_335 - $a2_120) / $t3_167 + $a2_120;
                    }
                    else
                    {
                        int32_t $t3_164 = *(adr_ev_list_now_49 + 0x10);
                        int32_t $t0_222 = $t3_164 - ev_now_1;
                        int32_t $t1_191 = *(adr_ev_list_now_49 + 0x14);
                        int32_t $t3_165 = $t1_191 - $t3_164;
                        
                        if ($t3_164 < ev_now_1)
                            $t0_222 = ev_now_1 - $t3_164;
                        
                        
                        if ($t1_191 < $t3_164)
                            $t3_165 = $t3_164 - $t1_191;
                        
                        $a2_121 = $a2_120 - $t0_222 * ($a2_120 - $a3_335) / $t3_165;
                    }
                    
                    *(((void**)((char*)adr_ctc_map2cut_y_now_2 + 0x14))) = $a2_121; // Fixed void pointer dereference
                    uint32_t $a2_122 = data_af26c_2;
                    uint32_t $a3_342 = data_af280_1;
                    void* adr_ev_list_now_50 = adr_ev_list_now;
                    uint32_t $a2_123;
                    
                    if ($a3_342 >= $a2_122)
                    {
                        int32_t $t3_170 = *(adr_ev_list_now_50 + 0x10);
                        int32_t $t0_229 = $t3_170 - ev_now_1;
                        int32_t $t1_197 = *(adr_ev_list_now_50 + 0x14);
                        int32_t $t3_171 = $t1_197 - $t3_170;
                        
                        if ($t3_170 < ev_now_1)
                            $t0_229 = ev_now_1 - $t3_170;
                        
                        
                        if ($t1_197 < $t3_170)
                            $t3_171 = $t3_170 - $t1_197;
                        
                        $a2_123 = $t0_229 * ($a3_342 - $a2_122) / $t3_171 + $a2_122;
                    }
                    else
                    {
                        int32_t $t3_168 = *(adr_ev_list_now_50 + 0x10);
                        int32_t $t0_227 = $t3_168 - ev_now_1;
                        int32_t $t1_195 = *(adr_ev_list_now_50 + 0x14);
                        int32_t $t3_169 = $t1_195 - $t3_168;
                        
                        if ($t3_168 < ev_now_1)
                            $t0_227 = ev_now_1 - $t3_168;
                        
                        
                        if ($t1_195 < $t3_168)
                            $t3_169 = $t3_168 - $t1_195;
                        
                        $a2_123 = $a2_122 - $t0_227 * ($a2_122 - $a3_342) / $t3_169;
                    }
                    
                    *(((void**)((char*)adr_ctc_map2cut_y_now_2 + 0x18))) = $a2_123; // Fixed void pointer dereference
                    uint32_t $a2_124 = data_af26e_4[0];
                    uint32_t $a3_349 = data_af282_1;
                    void* adr_ev_list_now_51 = adr_ev_list_now;
                    uint32_t $a2_125;
                    
                    if ($a3_349 >= $a2_124)
                    {
                        int32_t $t3_174 = *(adr_ev_list_now_51 + 0x10);
                        int32_t $t0_234 = $t3_174 - ev_now_1;
                        int32_t $t1_201 = *(adr_ev_list_now_51 + 0x14);
                        int32_t $t3_175 = $t1_201 - $t3_174;
                        
                        if ($t3_174 < ev_now_1)
                            $t0_234 = ev_now_1 - $t3_174;
                        
                        
                        if ($t1_201 < $t3_174)
                            $t3_175 = $t3_174 - $t1_201;
                        
                        $a2_125 = $t0_234 * ($a3_349 - $a2_124) / $t3_175 + $a2_124;
                    }
                    else
                    {
                        int32_t $t3_172 = *(adr_ev_list_now_51 + 0x10);
                        int32_t $t0_232 = $t3_172 - ev_now_1;
                        int32_t $t1_199 = *(adr_ev_list_now_51 + 0x14);
                        int32_t $t3_173 = $t1_199 - $t3_172;
                        
                        if ($t3_172 < ev_now_1)
                            $t0_232 = ev_now_1 - $t3_172;
                        
                        
                        if ($t1_199 < $t3_172)
                            $t3_173 = $t3_172 - $t1_199;
                        
                        $a2_125 = $a2_124 - $t0_232 * ($a2_124 - $a3_349) / $t3_173;
                    }
                    
                    *(((void**)((char*)adr_ctc_map2cut_y_now_2 + 0x1c))) = $a2_125; // Fixed void pointer dereference
                    $a2_53 = data_af26e_5[2][0];
                    uint32_t $v1_29 = data_af284_1;
                    void* adr_ev_list_now_7 = adr_ev_list_now;
                    
                    if ($v1_29 >= $a2_53)
                    {
                        int32_t $v0_11 = $a0_10 - ev_now_1;
                        $a0_10 = *(adr_ev_list_now_7 + 0x10);
                        
                        if ($a0_10 < ev_now_1)
                            $v0_11 = ev_now_1 - $a0_10;
                        
                        $v0_20 = $v0_11 * ($v1_29 - $a2_53);
                        $a3_130 = *(adr_ev_list_now_7 + 0x14);
                        goto label_5a5fc;
                    }
                    
                    $a0_9 = *(adr_ev_list_now_7 + 0x10);
                    int32_t $v0_10 = $a0_9 - ev_now_1;
                    
                    if ($a0_9 < ev_now_1)
                        $v0_10 = ev_now_1 - $a0_9;
                    
                    $v0_17 = $v0_10 * ($a2_53 - $v1_29);
                    $a3_128 = *(adr_ev_list_now_7 + 0x14);
                    goto label_5a5bc;
                }
                
                int32_t $t1_171 = *(adr_ev_list_now_2 + 0x18);
                
                if ($t1_171 >= ev_now_1)
                {
                    uint32_t $t4_6 = data_af26e[6][0];
                    uint32_t $a2_126 = data_af28c;
                    int32_t $a3_360 = ev_now_1 - $t2_96;
                    int32_t $t3_176 = $t1_171 < $t2_96 ? 1 : 0;
                        int32_t $t1_205 = $t1_171 - $t2_96;
                    uint32_t $a2_129;
                    
                    if ($a2_126 >= $t4_6)
                    {
                        
                        if ($t3_176)
                            $t1_205 = $t2_96 - $t1_171;
                        
                        $a2_129 = ($a2_126 - $t4_6) * $a3_360 / $t1_205 + $t4_6;
                    }
                    else
                    {
                        int32_t $t1_204 = $t1_171 - $t2_96;
                        
                        if ($t3_176)
                            $t1_204 = $t2_96 - $t1_171;
                        
                        $a2_129 = $t4_6 - ($t4_6 - $a2_126) * $a3_360 / $t1_204;
                    }
                    
                    **&adr_ctc_map2cut_y_now = $a2_129;
                    uint32_t $a2_132 = data_af26e_6[8][0];
                    uint32_t $a3_362 = data_af28e_1;
                    adr_ctc_map2cut_y_now_2 = adr_ctc_map2cut_y_now;
                    uint32_t $a2_133;
                    
                    if ($a3_362 >= $a2_132)
                    {
                        void* adr_ev_list_now_53 = adr_ev_list_now;
                        int32_t $t3_179 = *(adr_ev_list_now_53 + 0x14);
                        int32_t $t0_243 = $t3_179 - ev_now_1;
                        int32_t $t1_209 = *(adr_ev_list_now_53 + 0x18);
                        int32_t $t3_180 = $t1_209 - $t3_179;
                        
                        if ($t3_179 < ev_now_1)
                            $t0_243 = ev_now_1 - $t3_179;
                        
                        
                        if ($t1_209 < $t3_179)
                            $t3_180 = $t3_179 - $t1_209;
                        
                        $a2_133 = $t0_243 * ($a3_362 - $a2_132) / $t3_180 + $a2_132;
                    }
                    else
                    {
                        void* adr_ev_list_now_52 = adr_ev_list_now;
                        int32_t $t3_177 = *(adr_ev_list_now_52 + 0x14);
                        int32_t $t0_241 = $t3_177 - ev_now_1;
                        int32_t $t1_207 = *(adr_ev_list_now_52 + 0x18);
                        int32_t $t3_178 = $t1_207 - $t3_177;
                        
                        if ($t3_177 < ev_now_1)
                            $t0_241 = ev_now_1 - $t3_177;
                        
                        
                        if ($t1_207 < $t3_177)
                            $t3_178 = $t3_177 - $t1_207;
                        
                        $a2_133 = $a2_132 - $t0_241 * ($a2_132 - $a3_362) / $t3_178;
                    }
                    
                    *(((void**)((char*)adr_ctc_map2cut_y_now_2 + 4))) = $a2_133; // Fixed void pointer dereference
                    uint32_t $a2_134 = data_af278_2;
                    uint32_t $a3_369 = data_af290_1;
                    void* adr_ev_list_now_54 = adr_ev_list_now;
                    uint32_t $a2_135;
                    
                    if ($a3_369 >= $a2_134)
                    {
                        int32_t $t3_183 = *(adr_ev_list_now_54 + 0x14);
                        int32_t $t0_248 = $t3_183 - ev_now_1;
                        int32_t $t1_213 = *(adr_ev_list_now_54 + 0x18);
                        int32_t $t3_184 = $t1_213 - $t3_183;
                        
                        if ($t3_183 < ev_now_1)
                            $t0_248 = ev_now_1 - $t3_183;
                        
                        
                        if ($t1_213 < $t3_183)
                            $t3_184 = $t3_183 - $t1_213;
                        
                        $a2_135 = $t0_248 * ($a3_369 - $a2_134) / $t3_184 + $a2_134;
                    }
                    else
                    {
                        int32_t $t3_181 = *(adr_ev_list_now_54 + 0x14);
                        int32_t $t0_246 = $t3_181 - ev_now_1;
                        int32_t $t1_211 = *(adr_ev_list_now_54 + 0x18);
                        int32_t $t3_182 = $t1_211 - $t3_181;
                        
                        if ($t3_181 < ev_now_1)
                            $t0_246 = ev_now_1 - $t3_181;
                        
                        
                        if ($t1_211 < $t3_181)
                            $t3_182 = $t3_181 - $t1_211;
                        
                        $a2_135 = $a2_134 - $t0_246 * ($a2_134 - $a3_369) / $t3_182;
                    }
                    
                    *(((void**)((char*)adr_ctc_map2cut_y_now_2 + 8))) = $a2_135; // Fixed void pointer dereference
                    uint32_t $a2_136 = data_af27a_2;
                    uint32_t $a3_376 = data_af292_1;
                    void* adr_ev_list_now_55 = adr_ev_list_now;
                    uint32_t $a2_137;
                    
                    if ($a3_376 >= $a2_136)
                    {
                        int32_t $t3_187 = *(adr_ev_list_now_55 + 0x14);
                        int32_t $t0_253 = $t3_187 - ev_now_1;
                        int32_t $t1_217 = *(adr_ev_list_now_55 + 0x18);
                        int32_t $t3_188 = $t1_217 - $t3_187;
                        
                        if ($t3_187 < ev_now_1)
                            $t0_253 = ev_now_1 - $t3_187;
                        
                        
                        if ($t1_217 < $t3_187)
                            $t3_188 = $t3_187 - $t1_217;
                        
                        $a2_137 = $t0_253 * ($a3_376 - $a2_136) / $t3_188 + $a2_136;
                    }
                    else
                    {
                        int32_t $t3_185 = *(adr_ev_list_now_55 + 0x14);
                        int32_t $t0_251 = $t3_185 - ev_now_1;
                        int32_t $t1_215 = *(adr_ev_list_now_55 + 0x18);
                        int32_t $t3_186 = $t1_215 - $t3_185;
                        
                        if ($t3_185 < ev_now_1)
                            $t0_251 = ev_now_1 - $t3_185;
                        
                        
                        if ($t1_215 < $t3_185)
                            $t3_186 = $t3_185 - $t1_215;
                        
                        $a2_137 = $a2_136 - $t0_251 * ($a2_136 - $a3_376) / $t3_186;
                    }
                    
                    *(((void**)((char*)adr_ctc_map2cut_y_now_2 + 0xc))) = $a2_137; // Fixed void pointer dereference
                    uint32_t $a2_138 = data_af27c_2;
                    uint32_t $a3_383 = data_af294_1;
                    void* adr_ev_list_now_56 = adr_ev_list_now;
                    uint32_t $a2_139;
                    
                    if ($a3_383 >= $a2_138)
                    {
                        int32_t $t3_191 = *(adr_ev_list_now_56 + 0x14);
                        int32_t $t0_258 = $t3_191 - ev_now_1;
                        int32_t $t1_221 = *(adr_ev_list_now_56 + 0x18);
                        int32_t $t3_192 = $t1_221 - $t3_191;
                        
                        if ($t3_191 < ev_now_1)
                            $t0_258 = ev_now_1 - $t3_191;
                        
                        
                        if ($t1_221 < $t3_191)
                            $t3_192 = $t3_191 - $t1_221;
                        
                        $a2_139 = $t0_258 * ($a3_383 - $a2_138) / $t3_192 + $a2_138;
                    }
                    else
                    {
                        int32_t $t3_189 = *(adr_ev_list_now_56 + 0x14);
                        int32_t $t0_256 = $t3_189 - ev_now_1;
                        int32_t $t1_219 = *(adr_ev_list_now_56 + 0x18);
                        int32_t $t3_190 = $t1_219 - $t3_189;
                        
                        if ($t3_189 < ev_now_1)
                            $t0_256 = ev_now_1 - $t3_189;
                        
                        
                        if ($t1_219 < $t3_189)
                            $t3_190 = $t3_189 - $t1_219;
                        
                        $a2_139 = $a2_138 - $t0_256 * ($a2_138 - $a3_383) / $t3_190;
                    }
                    
                    *(((void**)((char*)adr_ctc_map2cut_y_now_2 + 0x10))) = $a2_139; // Fixed void pointer dereference
                    uint32_t $a2_140 = data_af27e_2;
                    uint32_t $a3_390 = data_af296_1;
                    void* adr_ev_list_now_57 = adr_ev_list_now;
                    uint32_t $a2_141;
                    
                    if ($a3_390 >= $a2_140)
                    {
                        int32_t $t3_195 = *(adr_ev_list_now_57 + 0x14);
                        int32_t $t0_263 = $t3_195 - ev_now_1;
                        int32_t $t1_225 = *(adr_ev_list_now_57 + 0x18);
                        int32_t $t3_196 = $t1_225 - $t3_195;
                        
                        if ($t3_195 < ev_now_1)
                            $t0_263 = ev_now_1 - $t3_195;
                        
                        
                        if ($t1_225 < $t3_195)
                            $t3_196 = $t3_195 - $t1_225;
                        
                        $a2_141 = $t0_263 * ($a3_390 - $a2_140) / $t3_196 + $a2_140;
                    }
                    else
                    {
                        int32_t $t3_193 = *(adr_ev_list_now_57 + 0x14);
                        int32_t $t0_261 = $t3_193 - ev_now_1;
                        int32_t $t1_223 = *(adr_ev_list_now_57 + 0x18);
                        int32_t $t3_194 = $t1_223 - $t3_193;
                        
                        if ($t3_193 < ev_now_1)
                            $t0_261 = ev_now_1 - $t3_193;
                        
                        
                        if ($t1_223 < $t3_193)
                            $t3_194 = $t3_193 - $t1_223;
                        
                        $a2_141 = $a2_140 - $t0_261 * ($a2_140 - $a3_390) / $t3_194;
                    }
                    
                    *(((void**)((char*)adr_ctc_map2cut_y_now_2 + 0x14))) = $a2_141; // Fixed void pointer dereference
                    uint32_t $a2_142 = data_af280_2;
                    uint32_t $a3_397 = data_af298_1;
                    void* adr_ev_list_now_58 = adr_ev_list_now;
                    uint32_t $a2_143;
                    
                    if ($a3_397 >= $a2_142)
                    {
                        int32_t $t3_199 = *(adr_ev_list_now_58 + 0x14);
                        int32_t $t0_268 = $t3_199 - ev_now_1;
                        int32_t $t1_229 = *(adr_ev_list_now_58 + 0x18);
                        int32_t $t3_200 = $t1_229 - $t3_199;
                        
                        if ($t3_199 < ev_now_1)
                            $t0_268 = ev_now_1 - $t3_199;
                        
                        
                        if ($t1_229 < $t3_199)
                            $t3_200 = $t3_199 - $t1_229;
                        
                        $a2_143 = $t0_268 * ($a3_397 - $a2_142) / $t3_200 + $a2_142;
                    }
                    else
                    {
                        int32_t $t3_197 = *(adr_ev_list_now_58 + 0x14);
                        int32_t $t0_266 = $t3_197 - ev_now_1;
                        int32_t $t1_227 = *(adr_ev_list_now_58 + 0x18);
                        int32_t $t3_198 = $t1_227 - $t3_197;
                        
                        if ($t3_197 < ev_now_1)
                            $t0_266 = ev_now_1 - $t3_197;
                        
                        
                        if ($t1_227 < $t3_197)
                            $t3_198 = $t3_197 - $t1_227;
                        
                        $a2_143 = $a2_142 - $t0_266 * ($a2_142 - $a3_397) / $t3_198;
                    }
                    
                    *(((void**)((char*)adr_ctc_map2cut_y_now_2 + 0x18))) = $a2_143; // Fixed void pointer dereference
                    uint32_t $a2_144 = data_af282_2;
                    uint32_t $a3_404 = data_af29a_1;
                    void* adr_ev_list_now_59 = adr_ev_list_now;
                    uint32_t $a2_145;
                    
                    if ($a3_404 >= $a2_144)
                    {
                        int32_t $t3_203 = *(adr_ev_list_now_59 + 0x14);
                        int32_t $t0_273 = $t3_203 - ev_now_1;
                        int32_t $t1_233 = *(adr_ev_list_now_59 + 0x18);
                        int32_t $t3_204 = $t1_233 - $t3_203;
                        
                        if ($t3_203 < ev_now_1)
                            $t0_273 = ev_now_1 - $t3_203;
                        
                        
                        if ($t1_233 < $t3_203)
                            $t3_204 = $t3_203 - $t1_233;
                        
                        $a2_145 = $t0_273 * ($a3_404 - $a2_144) / $t3_204 + $a2_144;
                    }
                    else
                    {
                        int32_t $t3_201 = *(adr_ev_list_now_59 + 0x14);
                        int32_t $t0_271 = $t3_201 - ev_now_1;
                        int32_t $t1_231 = *(adr_ev_list_now_59 + 0x18);
                        int32_t $t3_202 = $t1_231 - $t3_201;
                        
                        if ($t3_201 < ev_now_1)
                            $t0_271 = ev_now_1 - $t3_201;
                        
                        
                        if ($t1_231 < $t3_201)
                            $t3_202 = $t3_201 - $t1_231;
                        
                        $a2_145 = $a2_144 - $t0_271 * ($a2_144 - $a3_404) / $t3_202;
                    }
                    
                    *(((void**)((char*)adr_ctc_map2cut_y_now_2 + 0x1c))) = $a2_145; // Fixed void pointer dereference
                    $a2_53 = data_af284_2;
                    uint32_t $v1_32 = data_af29c_1;
                    void* adr_ev_list_now_8 = adr_ev_list_now;
                    
                    if ($v1_32 >= $a2_53)
                    {
                        int32_t $v0_13 = $a0_10 - ev_now_1;
                        $a0_10 = *(adr_ev_list_now_8 + 0x14);
                        
                        if ($a0_10 < ev_now_1)
                            $v0_13 = ev_now_1 - $a0_10;
                        
                        $v0_20 = $v0_13 * ($v1_32 - $a2_53);
                        $a3_130 = *(adr_ev_list_now_8 + 0x18);
                        goto label_5a5fc;
                    }
                    
                    $a0_9 = *(adr_ev_list_now_8 + 0x14);
                    int32_t $v0_12 = $a0_9 - ev_now_1;
                    
                    if ($a0_9 < ev_now_1)
                        $v0_12 = ev_now_1 - $a0_9;
                    
                    $v0_17 = $v0_12 * ($a2_53 - $v1_32);
                    $a3_128 = *(adr_ev_list_now_8 + 0x18);
                    goto label_5a5bc;
                }
                
                int32_t $t2_153 = *(adr_ev_list_now_2 + 0x1c);
                
                if ($t2_153 >= ev_now_1)
                {
                    uint32_t $t4_7 = data_af28c;
                    uint32_t $a3_415 = data_af2a0;
                    int32_t $a2_146 = ev_now_1 - $t1_171;
                    int32_t $t3_206 = $t2_153 < $t1_171 ? 1 : 0;
                        int32_t $t1_237 = $t2_153 - $t1_171;
                    uint32_t $a3_418;
                    
                    if ($a3_415 >= $t4_7)
                    {
                        
                        if ($t3_206)
                            $t1_237 = $t1_171 - $t2_153;
                        
                        $a3_418 = ($a3_415 - $t4_7) * $a2_146 / $t1_237 + $t4_7;
                    }
                    else
                    {
                        int32_t $t1_236 = $t2_153 - $t1_171;
                        
                        if ($t3_206)
                            $t1_236 = $t1_171 - $t2_153;
                        
                        $a3_418 = $t4_7 - ($t4_7 - $a3_415) * $a2_146 / $t1_236;
                    }
                    
                    **&adr_ctc_map2cut_y_now = $a3_418;
                    uint32_t $a2_148 = data_af28e_2;
                    uint32_t $a3_421 = data_af2a2_1;
                    adr_ctc_map2cut_y_now_2 = adr_ctc_map2cut_y_now;
                    uint32_t $a2_149;
                    
                    if ($a3_421 >= $a2_148)
                    {
                        void* adr_ev_list_now_61 = adr_ev_list_now;
                        int32_t $t3_209 = *(adr_ev_list_now_61 + 0x18);
                        int32_t $t0_282 = $t3_209 - ev_now_1;
                        int32_t $t1_241 = *(adr_ev_list_now_61 + 0x1c);
                        int32_t $t3_210 = $t1_241 - $t3_209;
                        
                        if ($t3_209 < ev_now_1)
                            $t0_282 = ev_now_1 - $t3_209;
                        
                        
                        if ($t1_241 < $t3_209)
                            $t3_210 = $t3_209 - $t1_241;
                        
                        $a2_149 = $t0_282 * ($a3_421 - $a2_148) / $t3_210 + $a2_148;
                    }
                    else
                    {
                        void* adr_ev_list_now_60 = adr_ev_list_now;
                        int32_t $t3_207 = *(adr_ev_list_now_60 + 0x18);
                        int32_t $t0_280 = $t3_207 - ev_now_1;
                        int32_t $t1_239 = *(adr_ev_list_now_60 + 0x1c);
                        int32_t $t3_208 = $t1_239 - $t3_207;
                        
                        if ($t3_207 < ev_now_1)
                            $t0_280 = ev_now_1 - $t3_207;
                        
                        
                        if ($t1_239 < $t3_207)
                            $t3_208 = $t3_207 - $t1_239;
                        
                        $a2_149 = $a2_148 - $t0_280 * ($a2_148 - $a3_421) / $t3_208;
                    }
                    
                    *(((void**)((char*)adr_ctc_map2cut_y_now_2 + 4))) = $a2_149; // Fixed void pointer dereference
                    uint32_t $a2_150 = data_af290_2;
                    uint32_t $a3_428 = data_af2a4_1;
                    void* adr_ev_list_now_62 = adr_ev_list_now;
                    uint32_t $a2_151;
                    
                    if ($a3_428 >= $a2_150)
                    {
                        int32_t $t3_213 = *(adr_ev_list_now_62 + 0x18);
                        int32_t $t0_287 = $t3_213 - ev_now_1;
                        int32_t $t1_245 = *(adr_ev_list_now_62 + 0x1c);
                        int32_t $t3_214 = $t1_245 - $t3_213;
                        
                        if ($t3_213 < ev_now_1)
                            $t0_287 = ev_now_1 - $t3_213;
                        
                        
                        if ($t1_245 < $t3_213)
                            $t3_214 = $t3_213 - $t1_245;
                        
                        $a2_151 = $t0_287 * ($a3_428 - $a2_150) / $t3_214 + $a2_150;
                    }
                    else
                    {
                        int32_t $t3_211 = *(adr_ev_list_now_62 + 0x18);
                        int32_t $t0_285 = $t3_211 - ev_now_1;
                        int32_t $t1_243 = *(adr_ev_list_now_62 + 0x1c);
                        int32_t $t3_212 = $t1_243 - $t3_211;
                        
                        if ($t3_211 < ev_now_1)
                            $t0_285 = ev_now_1 - $t3_211;
                        
                        
                        if ($t1_243 < $t3_211)
                            $t3_212 = $t3_211 - $t1_243;
                        
                        $a2_151 = $a2_150 - $t0_285 * ($a2_150 - $a3_428) / $t3_212;
                    }
                    
                    *(((void**)((char*)adr_ctc_map2cut_y_now_2 + 8))) = $a2_151; // Fixed void pointer dereference
                    uint32_t $a2_152 = data_af292_2;
                    uint32_t $a3_435 = data_af2a6_1;
                    void* adr_ev_list_now_63 = adr_ev_list_now;
                    uint32_t $a2_153;
                    
                    if ($a3_435 >= $a2_152)
                    {
                        int32_t $t3_217 = *(adr_ev_list_now_63 + 0x18);
                        int32_t $t0_292 = $t3_217 - ev_now_1;
                        int32_t $t1_249 = *(adr_ev_list_now_63 + 0x1c);
                        int32_t $t3_218 = $t1_249 - $t3_217;
                        
                        if ($t3_217 < ev_now_1)
                            $t0_292 = ev_now_1 - $t3_217;
                        
                        
                        if ($t1_249 < $t3_217)
                            $t3_218 = $t3_217 - $t1_249;
                        
                        $a2_153 = $t0_292 * ($a3_435 - $a2_152) / $t3_218 + $a2_152;
                    }
                    else
                    {
                        int32_t $t3_215 = *(adr_ev_list_now_63 + 0x18);
                        int32_t $t0_290 = $t3_215 - ev_now_1;
                        int32_t $t1_247 = *(adr_ev_list_now_63 + 0x1c);
                        int32_t $t3_216 = $t1_247 - $t3_215;
                        
                        if ($t3_215 < ev_now_1)
                            $t0_290 = ev_now_1 - $t3_215;
                        
                        
                        if ($t1_247 < $t3_215)
                            $t3_216 = $t3_215 - $t1_247;
                        
                        $a2_153 = $a2_152 - $t0_290 * ($a2_152 - $a3_435) / $t3_216;
                    }
                    
                    *(((void**)((char*)adr_ctc_map2cut_y_now_2 + 0xc))) = $a2_153; // Fixed void pointer dereference
                    uint32_t $a2_154 = data_af294_2;
                    uint32_t $a3_442 = data_af2a8_1;
                    void* adr_ev_list_now_64 = adr_ev_list_now;
                    uint32_t $a2_155;
                    
                    if ($a3_442 >= $a2_154)
                    {
                        int32_t $t3_221 = *(adr_ev_list_now_64 + 0x18);
                        int32_t $t0_297 = $t3_221 - ev_now_1;
                        int32_t $t1_253 = *(adr_ev_list_now_64 + 0x1c);
                        int32_t $t3_222 = $t1_253 - $t3_221;
                        
                        if ($t3_221 < ev_now_1)
                            $t0_297 = ev_now_1 - $t3_221;
                        
                        
                        if ($t1_253 < $t3_221)
                            $t3_222 = $t3_221 - $t1_253;
                        
                        $a2_155 = $t0_297 * ($a3_442 - $a2_154) / $t3_222 + $a2_154;
                    }
                    else
                    {
                        int32_t $t3_219 = *(adr_ev_list_now_64 + 0x18);
                        int32_t $t0_295 = $t3_219 - ev_now_1;
                        int32_t $t1_251 = *(adr_ev_list_now_64 + 0x1c);
                        int32_t $t3_220 = $t1_251 - $t3_219;
                        
                        if ($t3_219 < ev_now_1)
                            $t0_295 = ev_now_1 - $t3_219;
                        
                        
                        if ($t1_251 < $t3_219)
                            $t3_220 = $t3_219 - $t1_251;
                        
                        $a2_155 = $a2_154 - $t0_295 * ($a2_154 - $a3_442) / $t3_220;
                    }
                    
                    *(((void**)((char*)adr_ctc_map2cut_y_now_2 + 0x10))) = $a2_155; // Fixed void pointer dereference
                    uint32_t $a2_156 = data_af296_2;
                    uint32_t $a3_449 = data_af2aa_1;
                    void* adr_ev_list_now_65 = adr_ev_list_now;
                    uint32_t $a2_157;
                    
                    if ($a3_449 >= $a2_156)
                    {
                        int32_t $t3_225 = *(adr_ev_list_now_65 + 0x18);
                        int32_t $t0_302 = $t3_225 - ev_now_1;
                        int32_t $t1_257 = *(adr_ev_list_now_65 + 0x1c);
                        int32_t $t3_226 = $t1_257 - $t3_225;
                        
                        if ($t3_225 < ev_now_1)
                            $t0_302 = ev_now_1 - $t3_225;
                        
                        
                        if ($t1_257 < $t3_225)
                            $t3_226 = $t3_225 - $t1_257;
                        
                        $a2_157 = $t0_302 * ($a3_449 - $a2_156) / $t3_226 + $a2_156;
                    }
                    else
                    {
                        int32_t $t3_223 = *(adr_ev_list_now_65 + 0x18);
                        int32_t $t0_300 = $t3_223 - ev_now_1;
                        int32_t $t1_255 = *(adr_ev_list_now_65 + 0x1c);
                        int32_t $t3_224 = $t1_255 - $t3_223;
                        
                        if ($t3_223 < ev_now_1)
                            $t0_300 = ev_now_1 - $t3_223;
                        
                        
                        if ($t1_255 < $t3_223)
                            $t3_224 = $t3_223 - $t1_255;
                        
                        $a2_157 = $a2_156 - $t0_300 * ($a2_156 - $a3_449) / $t3_224;
                    }
                    
                    *(((void**)((char*)adr_ctc_map2cut_y_now_2 + 0x14))) = $a2_157; // Fixed void pointer dereference
                    uint32_t $a2_158 = data_af298_2;
                    uint32_t $a3_456 = data_af2ac_1;
                    void* adr_ev_list_now_66 = adr_ev_list_now;
                    uint32_t $a2_159;
                    
                    if ($a3_456 >= $a2_158)
                    {
                        int32_t $t3_229 = *(adr_ev_list_now_66 + 0x18);
                        int32_t $t0_307 = $t3_229 - ev_now_1;
                        int32_t $t1_261 = *(adr_ev_list_now_66 + 0x1c);
                        int32_t $t3_230 = $t1_261 - $t3_229;
                        
                        if ($t3_229 < ev_now_1)
                            $t0_307 = ev_now_1 - $t3_229;
                        
                        
                        if ($t1_261 < $t3_229)
                            $t3_230 = $t3_229 - $t1_261;
                        
                        $a2_159 = $t0_307 * ($a3_456 - $a2_158) / $t3_230 + $a2_158;
                    }
                    else
                    {
                        int32_t $t3_227 = *(adr_ev_list_now_66 + 0x18);
                        int32_t $t0_305 = $t3_227 - ev_now_1;
                        int32_t $t1_259 = *(adr_ev_list_now_66 + 0x1c);
                        int32_t $t3_228 = $t1_259 - $t3_227;
                        
                        if ($t3_227 < ev_now_1)
                            $t0_305 = ev_now_1 - $t3_227;
                        
                        
                        if ($t1_259 < $t3_227)
                            $t3_228 = $t3_227 - $t1_259;
                        
                        $a2_159 = $a2_158 - $t0_305 * ($a2_158 - $a3_456) / $t3_228;
                    }
                    
                    *(((void**)((char*)adr_ctc_map2cut_y_now_2 + 0x18))) = $a2_159; // Fixed void pointer dereference
                    uint32_t $a2_160 = data_af29a_2;
                    uint32_t $a3_463 = data_af2ae_1;
                    void* adr_ev_list_now_67 = adr_ev_list_now;
                    uint32_t $a2_161;
                    
                    if ($a3_463 >= $a2_160)
                    {
                        int32_t $t3_233 = *(adr_ev_list_now_67 + 0x18);
                        int32_t $t0_312 = $t3_233 - ev_now_1;
                        int32_t $t1_265 = *(adr_ev_list_now_67 + 0x1c);
                        int32_t $t3_234 = $t1_265 - $t3_233;
                        
                        if ($t3_233 < ev_now_1)
                            $t0_312 = ev_now_1 - $t3_233;
                        
                        
                        if ($t1_265 < $t3_233)
                            $t3_234 = $t3_233 - $t1_265;
                        
                        $a2_161 = $t0_312 * ($a3_463 - $a2_160) / $t3_234 + $a2_160;
                    }
                    else
                    {
                        int32_t $t3_231 = *(adr_ev_list_now_67 + 0x18);
                        int32_t $t0_310 = $t3_231 - ev_now_1;
                        int32_t $t1_263 = *(adr_ev_list_now_67 + 0x1c);
                        int32_t $t3_232 = $t1_263 - $t3_231;
                        
                        if ($t3_231 < ev_now_1)
                            $t0_310 = ev_now_1 - $t3_231;
                        
                        
                        if ($t1_263 < $t3_231)
                            $t3_232 = $t3_231 - $t1_263;
                        
                        $a2_161 = $a2_160 - $t0_310 * ($a2_160 - $a3_463) / $t3_232;
                    }
                    
                    *(((void**)((char*)adr_ctc_map2cut_y_now_2 + 0x1c))) = $a2_161; // Fixed void pointer dereference
                    $a2_53 = data_af29c_2;
                    uint32_t $v1_35 = data_af2b0_1;
                    void* adr_ev_list_now_9 = adr_ev_list_now;
                    
                    if ($v1_35 >= $a2_53)
                    {
                        int32_t $v0_15 = $a0_10 - ev_now_1;
                        $a0_10 = *(adr_ev_list_now_9 + 0x18);
                        
                        if ($a0_10 < ev_now_1)
                            $v0_15 = ev_now_1 - $a0_10;
                        
                        $v0_20 = $v0_15 * ($v1_35 - $a2_53);
                        $a3_130 = *(adr_ev_list_now_9 + 0x1c);
                        goto label_5a5fc;
                    }
                    
                    $a0_9 = *(adr_ev_list_now_9 + 0x18);
                    int32_t $v0_14 = $a0_9 - ev_now_1;
                    
                    if ($a0_9 < ev_now_1)
                        $v0_14 = ev_now_1 - $a0_9;
                    
                    $v0_17 = $v0_14 * ($a2_53 - $v1_35);
                    $a3_128 = *(adr_ev_list_now_9 + 0x1c);
                    goto label_5a5bc;
                }
                
                int32_t $t3_205 = *(adr_ev_list_now_2 + 0x20);
                
                if ($t3_205 < ev_now_1)
                {
                    **&adr_ctc_map2cut_y_now = data_af2b4;
                    adr_ctc_map2cut_y_now_1 = adr_ctc_map2cut_y_now;
                    *(((void**)((char*)adr_ctc_map2cut_y_now_1 + 4))) = data_af2b6; // Fixed void pointer dereference
                    *(((void**)((char*)adr_ctc_map2cut_y_now_1 + 8))) = data_af2b8[0]; // Fixed void pointer dereference
                    *(((void**)((char*)adr_ctc_map2cut_y_now_1 + 0xc))) = data_af2b8[2][0]; // Fixed void pointer dereference
                    *(((void**)((char*)adr_ctc_map2cut_y_now_1 + 0x10))) = data_af2b8[4][0]; // Fixed void pointer dereference
                    *(((void**)((char*)adr_ctc_map2cut_y_now_1 + 0x14))) = data_af2b8[6][0]; // Fixed void pointer dereference
                    *(((void**)((char*)adr_ctc_map2cut_y_now_1 + 0x18))) = data_af2b8[8][0]; // Fixed void pointer dereference
                    *(((void**)((char*)adr_ctc_map2cut_y_now_1 + 0x1c))) = data_af2c2; // Fixed void pointer dereference
                    *(((void**)((char*)adr_ctc_map2cut_y_now_1 + 0x20))) = data_af2c4; // Fixed void pointer dereference
                }
                else
                {
                    uint32_t $t4_8 = data_af2a0;
                    uint32_t $a2_163 = data_af2b4;
                    int32_t $t1_268 = ev_now_1 - $t2_153;
                    int32_t $t0_314 = $t3_205 < $t2_153 ? 1 : 0;
                        int32_t $t1_270 = $t3_205 - $t2_153;
                    uint32_t $a2_166;
                    
                    if ($a2_163 >= $t4_8)
                    {
                        
                        if ($t0_314)
                            $t1_270 = $t2_153 - $t3_205;
                        
                        $a2_166 = ($a2_163 - $t4_8) * $t1_268 / $t1_270 + $t4_8;
                    }
                    else
                    {
                        int32_t $t1_269 = $t3_205 - $t2_153;
                        
                        if ($t0_314)
                            $t1_269 = $t2_153 - $t3_205;
                        
                        $a2_166 = $t4_8 - ($t4_8 - $a2_163) * $t1_268 / $t1_269;
                    }
                    
                    **&adr_ctc_map2cut_y_now = $a2_166;
                    uint32_t $a2_169 = data_af2a2_2;
                    uint32_t $a3_478 = data_af2b6_1;
                    adr_ctc_map2cut_y_now_2 = adr_ctc_map2cut_y_now;
                    uint32_t $a2_170;
                    
                    if ($a3_478 >= $a2_169)
                    {
                        void* adr_ev_list_now_69 = adr_ev_list_now;
                        int32_t $t3_237 = *(adr_ev_list_now_69 + 0x1c);
                        int32_t $t0_318 = $t3_237 - ev_now_1;
                        int32_t $t1_274 = *(adr_ev_list_now_69 + 0x20);
                        int32_t $t3_238 = $t1_274 - $t3_237;
                        
                        if ($t3_237 < ev_now_1)
                            $t0_318 = ev_now_1 - $t3_237;
                        
                        
                        if ($t1_274 < $t3_237)
                            $t3_238 = $t3_237 - $t1_274;
                        
                        $a2_170 = $t0_318 * ($a3_478 - $a2_169) / $t3_238 + $a2_169;
                    }
                    else
                    {
                        void* adr_ev_list_now_68 = adr_ev_list_now;
                        int32_t $t3_235 = *(adr_ev_list_now_68 + 0x1c);
                        int32_t $t0_316 = $t3_235 - ev_now_1;
                        int32_t $t1_272 = *(adr_ev_list_now_68 + 0x20);
                        int32_t $t3_236 = $t1_272 - $t3_235;
                        
                        if ($t3_235 < ev_now_1)
                            $t0_316 = ev_now_1 - $t3_235;
                        
                        
                        if ($t1_272 < $t3_235)
                            $t3_236 = $t3_235 - $t1_272;
                        
                        $a2_170 = $a2_169 - $t0_316 * ($a2_169 - $a3_478) / $t3_236;
                    }
                    
                    *(((void**)((char*)adr_ctc_map2cut_y_now_2 + 4))) = $a2_170; // Fixed void pointer dereference
                    uint32_t $a2_171 = data_af2a4_2;
                    uint32_t $a3_485 = data_af2b8_1[0];
                    void* adr_ev_list_now_70 = adr_ev_list_now;
                    uint32_t $a2_172;
                    
                    if ($a3_485 >= $a2_171)
                    {
                        int32_t $t3_241 = *(adr_ev_list_now_70 + 0x1c);
                        int32_t $t0_323 = $t3_241 - ev_now_1;
                        int32_t $t1_278 = *(adr_ev_list_now_70 + 0x20);
                        int32_t $t3_242 = $t1_278 - $t3_241;
                        
                        if ($t3_241 < ev_now_1)
                            $t0_323 = ev_now_1 - $t3_241;
                        
                        
                        if ($t1_278 < $t3_241)
                            $t3_242 = $t3_241 - $t1_278;
                        
                        $a2_172 = $t0_323 * ($a3_485 - $a2_171) / $t3_242 + $a2_171;
                    }
                    else
                    {
                        int32_t $t3_239 = *(adr_ev_list_now_70 + 0x1c);
                        int32_t $t0_321 = $t3_239 - ev_now_1;
                        int32_t $t1_276 = *(adr_ev_list_now_70 + 0x20);
                        int32_t $t3_240 = $t1_276 - $t3_239;
                        
                        if ($t3_239 < ev_now_1)
                            $t0_321 = ev_now_1 - $t3_239;
                        
                        
                        if ($t1_276 < $t3_239)
                            $t3_240 = $t3_239 - $t1_276;
                        
                        $a2_172 = $a2_171 - $t0_321 * ($a2_171 - $a3_485) / $t3_240;
                    }
                    
                    *(((void**)((char*)adr_ctc_map2cut_y_now_2 + 8))) = $a2_172; // Fixed void pointer dereference
                    uint32_t $a2_173 = data_af2a6_2;
                    uint32_t $a3_492 = data_af2b8_2[2][0];
                    void* adr_ev_list_now_71 = adr_ev_list_now;
                    uint32_t $a2_174;
                    
                    if ($a3_492 >= $a2_173)
                    {
                        int32_t $t3_245 = *(adr_ev_list_now_71 + 0x1c);
                        int32_t $t0_328 = $t3_245 - ev_now_1;
                        int32_t $t1_282 = *(adr_ev_list_now_71 + 0x20);
                        int32_t $t3_246 = $t1_282 - $t3_245;
                        
                        if ($t3_245 < ev_now_1)
                            $t0_328 = ev_now_1 - $t3_245;
                        
                        
                        if ($t1_282 < $t3_245)
                            $t3_246 = $t3_245 - $t1_282;
                        
                        $a2_174 = $t0_328 * ($a3_492 - $a2_173) / $t3_246 + $a2_173;
                    }
                    else
                    {
                        int32_t $t3_243 = *(adr_ev_list_now_71 + 0x1c);
                        int32_t $t0_326 = $t3_243 - ev_now_1;
                        int32_t $t1_280 = *(adr_ev_list_now_71 + 0x20);
                        int32_t $t3_244 = $t1_280 - $t3_243;
                        
                        if ($t3_243 < ev_now_1)
                            $t0_326 = ev_now_1 - $t3_243;
                        
                        
                        if ($t1_280 < $t3_243)
                            $t3_244 = $t3_243 - $t1_280;
                        
                        $a2_174 = $a2_173 - $t0_326 * ($a2_173 - $a3_492) / $t3_244;
                    }
                    
                    *(((void**)((char*)adr_ctc_map2cut_y_now_2 + 0xc))) = $a2_174; // Fixed void pointer dereference
                    uint32_t $a2_175 = data_af2a8_2;
                    uint32_t $a3_499 = data_af2b8_3[4][0];
                    void* adr_ev_list_now_72 = adr_ev_list_now;
                    uint32_t $a2_176;
                    
                    if ($a3_499 >= $a2_175)
                    {
                        int32_t $t3_249 = *(adr_ev_list_now_72 + 0x1c);
                        int32_t $t0_333 = $t3_249 - ev_now_1;
                        int32_t $t1_286 = *(adr_ev_list_now_72 + 0x20);
                        int32_t $t3_250 = $t1_286 - $t3_249;
                        
                        if ($t3_249 < ev_now_1)
                            $t0_333 = ev_now_1 - $t3_249;
                        
                        
                        if ($t1_286 < $t3_249)
                            $t3_250 = $t3_249 - $t1_286;
                        
                        $a2_176 = $t0_333 * ($a3_499 - $a2_175) / $t3_250 + $a2_175;
                    }
                    else
                    {
                        int32_t $t3_247 = *(adr_ev_list_now_72 + 0x1c);
                        int32_t $t0_331 = $t3_247 - ev_now_1;
                        int32_t $t1_284 = *(adr_ev_list_now_72 + 0x20);
                        int32_t $t3_248 = $t1_284 - $t3_247;
                        
                        if ($t3_247 < ev_now_1)
                            $t0_331 = ev_now_1 - $t3_247;
                        
                        
                        if ($t1_284 < $t3_247)
                            $t3_248 = $t3_247 - $t1_284;
                        
                        $a2_176 = $a2_175 - $t0_331 * ($a2_175 - $a3_499) / $t3_248;
                    }
                    
                    *(((void**)((char*)adr_ctc_map2cut_y_now_2 + 0x10))) = $a2_176; // Fixed void pointer dereference
                    uint32_t $a2_177 = data_af2aa_2;
                    uint32_t $a3_506 = data_af2b8_4[6][0];
                    void* adr_ev_list_now_73 = adr_ev_list_now;
                    uint32_t $a2_178;
                    
                    if ($a3_506 >= $a2_177)
                    {
                        int32_t $t3_253 = *(adr_ev_list_now_73 + 0x1c);
                        int32_t $t0_338 = $t3_253 - ev_now_1;
                        int32_t $t1_290 = *(adr_ev_list_now_73 + 0x20);
                        int32_t $t3_254 = $t1_290 - $t3_253;
                        
                        if ($t3_253 < ev_now_1)
                            $t0_338 = ev_now_1 - $t3_253;
                        
                        
                        if ($t1_290 < $t3_253)
                            $t3_254 = $t3_253 - $t1_290;
                        
                        $a2_178 = $t0_338 * ($a3_506 - $a2_177) / $t3_254 + $a2_177;
                    }
                    else
                    {
                        int32_t $t3_251 = *(adr_ev_list_now_73 + 0x1c);
                        int32_t $t0_336 = $t3_251 - ev_now_1;
                        int32_t $t1_288 = *(adr_ev_list_now_73 + 0x20);
                        int32_t $t3_252 = $t1_288 - $t3_251;
                        
                        if ($t3_251 < ev_now_1)
                            $t0_336 = ev_now_1 - $t3_251;
                        
                        
                        if ($t1_288 < $t3_251)
                            $t3_252 = $t3_251 - $t1_288;
                        
                        $a2_178 = $a2_177 - $t0_336 * ($a2_177 - $a3_506) / $t3_252;
                    }
                    
                    *(((void**)((char*)adr_ctc_map2cut_y_now_2 + 0x14))) = $a2_178; // Fixed void pointer dereference
                    uint32_t $a2_179 = data_af2ac_2;
                    uint32_t $a3_513 = data_af2b8_5[8][0];
                    void* adr_ev_list_now_74 = adr_ev_list_now;
                    uint32_t $a2_180;
                    
                    if ($a3_513 >= $a2_179)
                    {
                        int32_t $t3_257 = *(adr_ev_list_now_74 + 0x1c);
                        int32_t $t0_343 = $t3_257 - ev_now_1;
                        int32_t $t1_294 = *(adr_ev_list_now_74 + 0x20);
                        int32_t $t3_258 = $t1_294 - $t3_257;
                        
                        if ($t3_257 < ev_now_1)
                            $t0_343 = ev_now_1 - $t3_257;
                        
                        
                        if ($t1_294 < $t3_257)
                            $t3_258 = $t3_257 - $t1_294;
                        
                        $a2_180 = $t0_343 * ($a3_513 - $a2_179) / $t3_258 + $a2_179;
                    }
                    else
                    {
                        int32_t $t3_255 = *(adr_ev_list_now_74 + 0x1c);
                        int32_t $t0_341 = $t3_255 - ev_now_1;
                        int32_t $t1_292 = *(adr_ev_list_now_74 + 0x20);
                        int32_t $t3_256 = $t1_292 - $t3_255;
                        
                        if ($t3_255 < ev_now_1)
                            $t0_341 = ev_now_1 - $t3_255;
                        
                        
                        if ($t1_292 < $t3_255)
                            $t3_256 = $t3_255 - $t1_292;
                        
                        $a2_180 = $a2_179 - $t0_341 * ($a2_179 - $a3_513) / $t3_256;
                    }
                    
                    *(((void**)((char*)adr_ctc_map2cut_y_now_2 + 0x18))) = $a2_180; // Fixed void pointer dereference
                    uint32_t $a2_181 = data_af2ae_2;
                    uint32_t $a3_520 = data_af2c2_1;
                    void* adr_ev_list_now_75 = adr_ev_list_now;
                    uint32_t $a2_182;
                    
                    if ($a3_520 >= $a2_181)
                    {
                        int32_t $t3_261 = *(adr_ev_list_now_75 + 0x1c);
                        int32_t $t0_348 = $t3_261 - ev_now_1;
                        int32_t $t1_298 = *(adr_ev_list_now_75 + 0x20);
                        int32_t $t3_262 = $t1_298 - $t3_261;
                        
                        if ($t3_261 < ev_now_1)
                            $t0_348 = ev_now_1 - $t3_261;
                        
                        
                        if ($t1_298 < $t3_261)
                            $t3_262 = $t3_261 - $t1_298;
                        
                        $a2_182 = $t0_348 * ($a3_520 - $a2_181) / $t3_262 + $a2_181;
                    }
                    else
                    {
                        int32_t $t3_259 = *(adr_ev_list_now_75 + 0x1c);
                        int32_t $t0_346 = $t3_259 - ev_now_1;
                        int32_t $t1_296 = *(adr_ev_list_now_75 + 0x20);
                        int32_t $t3_260 = $t1_296 - $t3_259;
                        
                        if ($t3_259 < ev_now_1)
                            $t0_346 = ev_now_1 - $t3_259;
                        
                        
                        if ($t1_296 < $t3_259)
                            $t3_260 = $t3_259 - $t1_296;
                        
                        $a2_182 = $a2_181 - $t0_346 * ($a2_181 - $a3_520) / $t3_260;
                    }
                    
                    *(((void**)((char*)adr_ctc_map2cut_y_now_2 + 0x1c))) = $a2_182; // Fixed void pointer dereference
                    $a2_53 = data_af2b0_2;
                    uint32_t $v1_38 = data_af2c4_1;
                    void* adr_ev_list_now_10 = adr_ev_list_now;
                    
                    if ($v1_38 >= $a2_53)
                    {
                        int32_t $v0_19 = $a0_10 - ev_now_1;
                        int32_t $a0_13 = $a3_130 - $a0_10;
                        $a0_10 = *(adr_ev_list_now_10 + 0x1c);
                        
                        if ($a0_10 < ev_now_1)
                            $v0_19 = ev_now_1 - $a0_10;
                        
                        $v0_20 = $v0_19 * ($v1_38 - $a2_53);
                        $a3_130 = *(adr_ev_list_now_10 + 0x20);
                    label_5a5fc:
                        
                        if ($a3_130 < $a0_10)
                            $a0_13 = $a0_10 - $a3_130;
                        
                        *(((void**)((char*)adr_ctc_map2cut_y_now_2 + 0x20))) = $v0_20 / $a0_13 + $a2_53; // Fixed void pointer dereference
                    }
                    else
                    {
                        int32_t $v0_16 = $a0_9 - ev_now_1;
                        int32_t $a0_12 = $a3_128 - $a0_9;
                        $a0_9 = *(adr_ev_list_now_10 + 0x1c);
                        
                        if ($a0_9 < ev_now_1)
                            $v0_16 = ev_now_1 - $a0_9;
                        
                        $v0_17 = $v0_16 * ($a2_53 - $v1_38);
                        $a3_128 = *(adr_ev_list_now_10 + 0x20);
                    label_5a5bc:
                        
                        if ($a3_128 < $a0_9)
                            $a0_12 = $a0_9 - $a3_128;
                        
                        *(((void**)((char*)adr_ctc_map2cut_y_now_2 + 0x20))) = $a2_53 - $v0_17 / $a0_12; // Fixed void pointer dereference
                    }
                }
            }
            else
            {
                **&adr_ctc_map2cut_y_now = param_adr_gam_y_array;
                adr_ctc_map2cut_y_now_1 = adr_ctc_map2cut_y_now;
                *(((void**)((char*)adr_ctc_map2cut_y_now_1 + 4))) = data_af20e; // Fixed void pointer dereference
                *(((void**)((char*)adr_ctc_map2cut_y_now_1 + 8))) = data_af210; // Fixed void pointer dereference
                *(((void**)((char*)adr_ctc_map2cut_y_now_1 + 0xc))) = data_af212; // Fixed void pointer dereference
                *(((void**)((char*)adr_ctc_map2cut_y_now_1 + 0x10))) = data_af214; // Fixed void pointer dereference
                *(((void**)((char*)adr_ctc_map2cut_y_now_1 + 0x14))) = data_af216; // Fixed void pointer dereference
                *(((void**)((char*)adr_ctc_map2cut_y_now_1 + 0x18))) = data_af218; // Fixed void pointer dereference
                *(((void**)((char*)adr_ctc_map2cut_y_now_1 + 0x1c))) = data_af21a; // Fixed void pointer dereference
                *(((void**)((char*)adr_ctc_map2cut_y_now_1 + 0x20))) = data_af21c; // Fixed void pointer dereference
            }
        }
    }
    
    int32_t $v0_23 = 0;
    
    for (int32_t i = 0; (uintptr_t)i != 0xb; )
    {
        *(((void**)((char*)&min_kneepoint_x + $v0_23))) = *(&param_adr_min_kneepoint_array_def + $v0_23); // Fixed void pointer dereference
        *(((void**)((char*)&map_kneepoint_x + $v0_23))) = *(&param_adr_map_kneepoint_array + $v0_23); // Fixed void pointer dereference
        
        if (i < 9)
            *(((void**)((char*)&ctc_kneepoint_x + $v0_23))) = *(&param_adr_ctc_kneepoint_array + $v0_23); // Fixed void pointer dereference
        
        i += 1;
        $v0_23 += 4;
    }
    
    TizianoAdrFpgaStructMe = &ctc_kneepoint_x;
    data_d03f0_1 = &adr_hist;
    data_d03f4_1 = &adr_block_y;
    data_d03f8_1 = &adr_block_hist;
    data_d03fc_1 = &adr_tm_base_lut;
    data_d0400_1 = &param_adr_gam_x_array;
    data_d0404_1 = &param_adr_gam_y_array_def;
    data_d0408_1 = adr_ctc_map2cut_y_now;
    data_d040c_1 = adr_map_mode_now;
    void* adr_light_end_now_1 = adr_light_end_now;
    data_d03dc_1 = &ctc_kneepoint_y;
    data_d0410_1 = adr_light_end_now_1;
    void* adr_block_light_now_3 = adr_block_light_now;
    data_d03e4_1 = &min_kneepoint_y;
    data_d0414_1 = adr_block_light_now_3;
    data_d03e0_1 = &min_kneepoint_x;
    data_d03e8_1 = &map_kneepoint_x;
    data_d03ec_1 = &map_kneepoint_y;
    
    for (int32_t i_1 = 0; (uintptr_t)i_1 < 0x30; i_1 += 1)
    {
        char var_58[0x34];
        var_58[i_1] = *(&data_d03e8 + i_1);
    }
    
    Tiziano_adr_fpga(TizianoAdrFpgaStructMe, data_d03dc_2, data_d03e0_2, data_d03e4_2);
    int32_t $t7_7 = data_ace80_1;
    
    if ($t7_7 == 1)
    {
        int32_t $t0_354 = data_ace84;
        int32_t $s0_5 = data_ace7c;
        void* $a0_24 = &map_kneepoint_y;
        void* i_2 = &map_kneepoint_y_pre;
        int32_t $t5_13 = 0;
            int32_t j = 0;
            int32_t $v0_24 = 0;
                int32_t $t2_242 = *($a0_24 + j);
                int32_t $a1_46 = *(i_2 + j);
                int32_t $a1_47 = $a1_46 - $t2_242;
        
        do
        {
            
            do
            {
                j += 4;
                
                if ($a1_46 < $t2_242)
                    $a1_47 = $t2_242 - $a1_46;
                
                if ($v0_24 < $a1_47)
                    $v0_24 = $a1_47;
            } while ((uintptr_t)j != 0x2c);
            
            int32_t $v1_44;
            
            if ($(uintptr_t)t0_354 >= 0x201)
            {
                $t5_13 = 1;
                $t0_354 = 0x200;
                $v1_44 = $t0_354 < $v0_24 ? 1 : 0;
            }
            else
            {
                $v1_44 = $t0_354 < $v0_24 ? 1 : 0;
                
                if ($s0_5 != 9 && !$t0_354)
                {
                    $t0_354 = $t7_7;
                    $t5_13 = 1;
                    $v1_44 = $t0_354 < $v0_24 ? 1 : 0;
                }
            }
            
            void* $v1_50 = $a0_24;
            
            if (!$v1_44)
            {
                    void* $a1_53 = i_2 + (j_1 << 2);
                for (int32_t j_1 = 0xff5; (uintptr_t)j_1 != 0x1000; )
                {
                    if (j_1 < *$v1_50)
                        *$v1_50 = j_1;
                    
                    j_1 += 1;
                    *($a1_53 - 0x3fd4) = *$v1_50;
                    $v1_50 += 4;
                }
                
                i_2 += 0x2c;
            }
            else
            {
                uint32_t $lo_147 = ($t0_354 << 0x15) / $v0_24;
                void* i_3 = i_2;
                void* $t3_268 = $a0_24;
                    int32_t $v0_25 = *$t3_268;
                    int32_t $s3_2 = *i_3;
                
                for (int32_t j_2 = 0xff5; (uintptr_t)j_2 != 0x1000; )
                {
                    int32_t j_3;
                    
                    if ($s3_2 >= $v0_25)
                    {
                        j_3 = $s3_2 - (($s3_2 - $v0_25) * $lo_147 + 0x100000) / 0x200000;
                        
                        if (j_2 < j_3)
                            j_3 = j_2;
                    }
                    else
                    {
                        j_3 = (($v0_25 - $s3_2) * $lo_147 + 0x100000) / 0x200000 + $s3_2;
                        
                        if (j_2 < j_3)
                            j_3 = j_2;
                    }
                    
                    j_2 += 1;
                    *$t3_268 = j_3;
                    *i_3 = j_3;
                    $t3_268 += 4;
                    i_3 += 4;
                }
                
                i_2 += 0x2c;
            }
            
            $a0_24 += 0x2c;
        } while (&map_kneepoint_y != i_2);
        
        if ($t5_13)
            data_ace84_1 = $t0_354;
    }
    
    return 0;
}

