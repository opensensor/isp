#include "include/main.h"


  uint32_t* ae0_tune2(int32_t* arg1, int32_t arg2, void* arg3, void* arg4, int32_t* arg5, uint32_t* arg6, int32_t* arg7, void* arg8, int32_t* arg9, int32_t* arg10, int32_t* arg11, int32_t* arg12, int32_t* arg13, int32_t* arg14, int32_t* arg15, int32_t* arg16, int32_t* arg17, int32_t* arg18, int32_t* arg19, int32_t* arg20, int32_t* arg21, int32_t* arg22, int32_t* arg23, int32_t* arg24, int32_t* arg25, int32_t* arg26, uint32_t arg27, int32_t* arg28, int32_t arg29, int32_t arg30, int32_t arg31, int32_t arg32, int32_t arg33, int32_t arg34)

{
    void* arg_8 = arg3;
    int32_t arg_c = $a3;
    int32_t* arg_0 = arg1;
    int32_t arg_4 = arg2;
    int32_t var_dc = *arg11;
    int32_t var_e0 = arg11[1];
    int32_t $a3_3 = arg11[2];
    int32_t $a3_4 = arg11[3];
    int32_t $v0_1 = arg11[4];
    int32_t $v0_2 = *arg12;
    int32_t* $fp = arg18;
    int32_t $v0_3 = arg12[1];
    int32_t $v0_4 = arg12[2];
    int32_t $v0_5 = arg12[3];
    int32_t $v0_6 = *arg13;
    int32_t $v0_7 = arg13[1];
    int32_t $v0_8 = arg13[2];
    int32_t $v0_9 = arg13[3];
    int32_t $s0 = *arg14;
    int32_t var_b0 = *arg16;
    int32_t $s2 = arg1[2];
    int32_t $v0_15 = *arg10;
    int32_t $v0_16 = arg10[1];
    int32_t $v0_17 = arg10[2];
    uint32_t $v0_19 = *arg21;
    int32_t $v0_21 = *arg22;
    int32_t $v0_23 = *arg23;
    uint32_t var_c4 = *arg24;
    int32_t var_d4 = *arg25;
    int32_t var_d0 = *arg26;
    uint32_t $v0_30 = *arg5;
    int32_t $v0_31 = arg5[1];
    int32_t $v0_32 = arg5[2];
    int32_t $s5 = *arg7;
    int32_t $v0_34 = arg7[1];
    int32_t var_c8 = arg7[3];
    int32_t $v0_38 = arg7[4];
    uint32_t var_b8 = fix_point_mult3_32($s0, $v0_31, $v0_32);
    int32_t $v0_40 = arg1[7];
    int32_t $v0_41 = arg1[8];
    int32_t $v0_42 = *arg1;
    int32_t $v0_43 = arg1[1];
        int32_t $a1_5 = *(arg4 + i);
        void* $a0_3 = &var_130 + i;
    int32_t $a3;
    int32_t (* var_84)(int32_t arg1, int32_t arg2, int32_t arg3) = fix_point_mult3_32;
    void var_130;
    void var_108;
    
    for (int32_t i = 0; (uintptr_t)i != 0x28; )
    {
        *(((void**)((char*)&var_108 + i))) = *(arg3 + i); // Fixed void pointer dereference
        i += 4;
        *$a0_3 = $a1_5;
    }
    
    int32_t $a0_4;
    
    if (data_aa2fc_1 != 1)
        $a0_4 = *arg17;
    else
    {
        int32_t* $v0_44 = arg15;
        int32_t i_1 = 0;
        int32_t $s3_1 = 0;
            int32_t $a0_5 = *$v0_44;
            int32_t $a1_6 = i_1 * $a0_5;
        void* i_2 = &var_108;
        uint32_t $lo_1 = $s3_1 / (data_b0d58[0] * data_b0d58[0xf] * *data_b0d4c * *data_b0d54);
        uint32_t $v0_49 = 0xff;
        int32_t $v0_50 =
        void* $s3_3 = &var_130;
        void* const $t1_1 = &data_20000;
            int32_t $v0_51 = ($t1_1 + 0xfd4)($s0, *i_2 << ($s0 & 0x1f), $v0_50);
            int32_t $a1_12 = *$s3_3;
        int32_t var_158_1 = 8;
        uint32_t var_150_1 = $v0_49;
        int32_t var_14c_1 = 0;
        
        do
        {
            $v0_44 = &$v0_44[1];
            i_1 += 1;
            $s3_1 += $a1_6;
        } while ((uintptr_t)i_1 != 0x100);
        
        
        if ($(uintptr_t)lo_1 < 0x100)
            $v0_49 = $lo_1;
        
            fix_point_div_32($s0, (i_1 - ($v0_49 >> 1)) << ($s0 & 0x1f), i_1 << ($s0 & 0x1f));
        
        do
        {
            *i_2 = $v0_51 >> ($s0 & 0x1f);
            *$s3_3 = ($t1_1 + 0xfd4)($s0, $a1_12 << ($s0 & 0x1f), $v0_50) >> ($s0 & 0x1f);
            i_2 += 4;
            $s3_3 += 4;
        } while (i_2 != &var_e0);
        
        void var_160;
        tisp_event_push(&var_160);
        $a0_4 = *arg17;
    }
    
    int32_t i_3 = 0;
    int32_t* i_5;
    
    if ($a0_4 != 1)
    {
            int32_t $a1_18 = *(&var_130 + i_3);
            void* $a0_13 = arg19 + i_3;
        do
        {
            *(((void**)((char*)$fp + i_3))) = *(&var_108 + i_3); // Fixed void pointer dereference
            i_3 += 4;
            *$a0_13 = $a1_18;
        } while ((uintptr_t)i_3 != 0x28);
        
        i_5 = arg8 + 4;
    }
    else
    {
        int32_t* i_4 = arg20;
        int32_t* $t0_4 = arg19;
        void* $a1_14 = &var_108;
        void* $a0_10 = &var_130;
            int32_t $v0_56 = arg17[1];
            int32_t $t3_1 = arg17[2];
            uint32_t $a2_10 = ($v0_56 * *$a1_14) >> 7;
            uint32_t $v1_7 = ($v0_56 * *$a0_10) >> 7;
            int32_t $v0_58 = *i_4;
            uint32_t $v0_61 = ($v1_7 * $v0_58 * $t3_1) >> 0xe;
        
        do
        {
            *$a1_14 = $a2_10;
            *$a0_10 = $v0_61;
            
            if (!$a2_10)
                *$a1_14 = 1;
            
            if (!$v0_61)
                *$a0_10 = 1;
            
            i_4 = &i_4[1];
            *$fp = *$a1_14;
            $a1_14 += 4;
            *$t0_4 = *$a0_10;
            $a0_10 += 4;
            $fp = &$fp[1];
            $t0_4 = &$t0_4[1];
        } while (i_4 != &i_4[0xa]);
        
        i_5 = arg8 + 4;
    }
    
    do
    {
        int32_t $a0_14 = *i_5;
    uint32_t IspAeFlag_1 = IspAeFlag;
        uint32_t ftune_wmeans.32574_1 = ftune_wmeans.32574;
        i_5 = &i_5[1];
        *(i_5 - 8) = $a0_14;
    } while (i_5 != arg8 + 0x3c);
    
    *(((void**)((char*)arg8 + 0x38))) = arg27; // Fixed void pointer dereference
    uint32_t $s6_2;
    
    if (IspAeFlag_1 != 1)
    {
        
        if ($v0_34 == 1)
        {
        label_60560:
            ftune_wmeans.32574 = 0;
            ftune_wmeans.32574_1 = ftune_wmeans.32574;
        }
        
        if (ftune_wmeans.32574_1 == 1)
            $s6_2 = arg27;
        else
        {
            int32_t i_6 = 0xf - $s2;
            void* $s1_1 = arg8 + (i_6 << 2);
            int32_t $a0_17 = 0;
            int32_t $v0_66 = 0;
            if ($(uintptr_t)s2 >= 0x10)
                $s2 = 0xf;
            else if (!$s2)
                $s2 = 1;
            
            
            do
            {
                i_6 += 1;
                $a0_17 += i_6;
                $v0_66 += i_6 * *$s1_1;
                $s1_1 += 4;
            } while ((uintptr_t)i_6 != 0xf);
            
            $s6_2 = $v0_66 / $a0_17;
        }
    }
    else
    {
        ftune_wmeans.32574 = IspAeFlag_1;
        
        if ($v0_34 == IspAeFlag_1)
            goto label_60560;
        
        $s6_2 = arg27;
    }
    
    if (!$s6_2)
        $s6_2 = 1;
    
    if (IspAeFlag_1 == 1)
    {
            uint32_t ae_ev_init_strict_1 = ae_ev_init_strict;
        uint32_t $a0_19;
        
        if (ae_ev_init_en != IspAeFlag_1)
            $a0_19 = var_b8;
        else
        {
            data_b0e0c = 0;
            var_b8 = ae_ev_init_strict_1 << ($s0 & 0x1f);
            ae_ev_init_en = 0;
            $a0_19 = var_b8;
        }
        
        $s5 = tisp_ae_target($a0_19, &var_108_3, &var_130_4, $s0);
    }
    
    int32_t $s2_1 = $s6_2 << ($s0 & 0x1f);
    int32_t $v0_68 = fix_point_div_32($s0, $s5 << ($s0 & 0x1f), $s2_1);
    int32_t $v0_69 = fix_point_mult2_32($s0, var_b8_13, $v0_68);
    int32_t var_170_4;
    int32_t var_16c_13;
    int32_t var_168_6;
    int32_t var_164_7;
    int32_t var_cc_2;
    uint32_t $s2_2;
    
    if ($v0_34 != 1)
    {
        if (!$v0_34)
            goto label_606b8;
        
        $s2_2 = 0;
        var_cc_2 = $v0_34;
        $s5 = 0;
    }
    else
    {
        int32_t* $v1_13 = arg7;
        
        if ($v0_68 >= $v0_6)
            goto label_6068c;
        
        if ($v0_7 < $v0_68)
        {
            $s2_2 = var_b8;
            arg7[2] = arg27;
            var_cc_2 = 1;
        }
        else
        {
                int32_t $a1_25 = $s5 - $s6_2;
                int32_t $v0_75 = fix_point_div_32($s0, $a1_25 << ($s0 & 0x1f), $s2_1);
            $v1_13 = arg7;
        label_6068c:
            $v1_13[1] = 0;
        label_606b8:
            
            if ($s6_2 < $s5 - $v0_8)
            {
                var_cc_2 = 0;
            label_6071c:
                data_b0e04 = 0;
                $s5 = tisp_ae_target($v0_69, &var_108, &var_130, $s0);
                *arg7 = $s5;
                
                if ($s6_2 >= $s5)
                    $a1_25 = $s6_2 - $s5;
                
                $s2_2 = 1 << ($s0 & 0x1f);
                int32_t $a3_8;
                
                if (!$v0_15)
                {
                    $a3_8 = $v0_40;
                label_60774:
                    
                    if ($a3_8 > 0)
                        tisp_ae_tune(arg28, &var_dc, &var_e0, $a3_8, $s0, $s2_2);
                }
                else if ($v0_15 == 1)
                {
                    $a3_8 = $v0_41;
                    goto label_60774;
                }
                int32_t (* var_94_1_1)(int32_t arg1, int32_t arg2, int32_t arg3, int32_t arg4, 
                    int32_t arg5, int32_t arg6) = fix_point_mult2_64;
                int32_t var_170_2_1;
                int32_t var_16c_2_1;
                int32_t $t0_13;
                int32_t $fp_2;
                
                if ($s6_2 >= $s5)
                {
                        int32_t var_70_4 = $v0_99;
                        int32_t var_6c_2 = $v1_41;
                    int32_t $v0_97;
                    int32_t $t1_6;
                    int32_t $fp_3;
                    
                    if (-($v0_1) >= -($v0_75))
                    {
                        int32_t $v0_98;
                        int32_t $v1_40;
                        $v0_98 = __ashldi3($s2_2, 0, $s0);
                        int32_t $v0_99;
                        int32_t $v1_41;
                        $v0_99 = __ashldi3(var_dc, 0, $s0);
                        int32_t $v0_100;
                        int32_t $v1_42;
                        $v0_100 = __ashldi3($v0_75, 0, $s0);
                        var_170_2 = $v0_100;
                        var_16c_2 = $v1_42;
                        int32_t $v0_102;
                        int32_t $v1_43;
                        $v0_102 = var_94_1($s0 << 1);
                        $v0_97 = $v0_98 - $v0_102;
                        $t1_6 = $v0_98 < $v0_97 ? 1 : 0;
                        $fp_3 = $v1_40 - $v1_43;
                    }
                    else
                    {
                        int32_t $t0_8 = $s0 << 1;
                        int32_t $v0_91;
                        int32_t $v1_34;
                        $v0_91 = __ashldi3($s2_2, 0, $s0);
                        int32_t $v0_92;
                        int32_t $v1_35;
                        $v0_92 = __ashldi3($v0_75, 0, $s0);
                        int32_t $v0_93;
                        int32_t $v1_36;
                        int32_t $a1_29;
                        $v0_93 = __ashldi3(var_dc, 0, $s0);
                        var_168 = $v0_92;
                        var_164 = $v1_35;
                        int32_t $v0_94;
                        int32_t $v1_37;
                        $v0_94 = fix_point_mult3_64($t0_8, $a1_29, $v0_93, $v1_36, $v0_92, $v1_35, 
                            var_168, var_164);
                        int32_t $v0_95;
                        int32_t $v1_38;
                        int32_t $a1_30;
                        $v0_95 = __ashldi3($s2_2 - $v0_75, 0, $s0);
                        var_170_2 = $v0_95;
                        var_16c_2 = $v1_38;
                        int32_t $v0_96;
                        int32_t $v1_39;
                        $v0_96 =
                            fix_point_div_64($t0_8, $a1_30, $v0_94, $v1_37, var_170_2, var_16c_2);
                        $v0_97 = $v0_91 - $v0_96;
                        $t1_6 = $v0_91 < $v0_97 ? 1 : 0;
                        $fp_3 = $v1_34 - $v1_39;
                    }
                    
                    $t0_13 = $v0_97;
                    $fp_2 = $fp_3 - $t1_6;
                }
                else
                {
                    int32_t $v0_84;
                    int32_t $v1_27;
                    int32_t $t1_3;
                    
                    if ($a3_4 >= $v0_75)
                    {
                        int32_t $v0_85;
                        int32_t $v1_28;
                        $v0_85 = __ashldi3($s2_2, 0, $s0);
                        int32_t $v0_86;
                        int32_t $v1_29;
                        $v0_86 = __ashldi3($v0_75, 0, $s0);
                        int32_t $v0_87;
                        int32_t $v1_30;
                        int32_t $a1_28;
                        $v0_87 = __ashldi3(var_e0, 0, $s0);
                        var_168 = $v0_86;
                        var_164 = $v1_29;
                        var_170_2 = $v0_86;
                        var_16c_2 = $v1_29;
                        int32_t $v0_88;
                        int32_t $v1_31;
                        $v0_88 = fix_point_mult3_64($s0 << 1, $a1_28, $v0_87, $v1_30, var_170_2, 
                            var_16c_2, var_168, var_164);
                        $v0_84 = $v0_88 + $v0_85;
                        $t1_3 = $v0_84 < $v0_85 ? 1 : 0;
                        $v1_27 = $v1_31 + $v1_28;
                    }
                    else
                    {
                        int32_t var_70_1 = $v0_80;
                        int32_t var_6c_1 = $v1_24;
                        int32_t $v0_79;
                        int32_t $v1_23;
                        $v0_79 = __ashldi3($s2_2, 0, $s0);
                        int32_t $v0_80;
                        int32_t $v1_24;
                        $v0_80 = __ashldi3(var_e0, 0, $s0);
                        int32_t $v0_81;
                        int32_t $v1_25;
                        $v0_81 = __ashldi3($v0_75, 0, $s0);
                        var_170_2 = $v0_81;
                        var_16c_2 = $v1_25;
                        int32_t $v0_83;
                        int32_t $v1_26;
                        $v0_83 = var_94_1($s0 << 1);
                        $v0_84 = $v0_83 + $v0_79;
                        $t1_3 = $v0_84 < $v0_79 ? 1 : 0;
                        $v1_27 = $v1_26 + $v1_23;
                    }
                    
                    $t0_13 = $v0_84;
                    $fp_2 = $t1_3 + $v1_27;
                }
                
                int32_t $v0_103;
                int32_t $v1_44;
                $v0_103 = __ashldi3($s2_2, 0, $s0);
                int32_t $v0_104;
                int32_t $v1_45;
                $v0_104 = __ashldi3($a3_3, 0, $s0);
                int32_t $a3_12 = $v0_103;
                int32_t $a1_31 = $v0_104 + $a3_12;
                int32_t $a0_47 = ($a1_31 < $a3_12 ? 1 : 0) + $v1_45 + $v1_44;
                int32_t $t0_14 = $t0_13;
                uint32_t $a0_51;
                int32_t $t1_10;
                int32_t $v0_105;
                
                if ($fp_2 < $a0_47)
                {
                label_60ab4:
                    
                    if ($v1_44 < $fp_2)
                    {
                        $t0_14 = $a1_31;
                        $t1_10 = $s0 << 1;
                        $a0_51 = var_b8;
                    }
                    else
                    {
                        if ($fp_2 != $v1_44)
                        {
                            $v0_105 = $a3_12 - $v0_104;
                            goto label_60ae0;
                        }
                        
                        if ($a3_12 >= $t0_14)
                        {
                            $v0_105 = $a3_12 - $v0_104;
                            goto label_60ae0;
                        }
                        
                        $t0_14 = $a1_31;
                        $t1_10 = $s0 << 1;
                        $a0_51 = var_b8_14;
                    }
                }
                else
                {
                    if ($a0_47 == $fp_2)
                    {
                        if ($t0_14 < $a1_31)
                            goto label_60ab4;
                        
                        $v0_105 = $a3_12 - $v0_104;
                        goto label_60ae0;
                    }
                    
                    $v0_105 = $a3_12 - $v0_104;
                label_60ae0:
                    int32_t $v1_47 = $v1_44 - $v1_45 - ($a3_12 < $v0_105 ? 1 : 0);
                    int32_t $t1_9;
                    
                    if ($v1_47 < $fp_2)
                    {
                        $t1_9 = $fp_2 < $v1_44 ? 1 : 0;
                    label_60b08:
                        
                        if ($t1_9)
                        {
                            $t0_14 = $v0_105;
                            $t1_10 = $s0 << 1;
                            $a0_51 = var_b8;
                        }
                        else
                        {
                            $t1_10 = $s0 << 1;
                            
                            if ($v1_44 != $fp_2)
                                $a0_51 = var_b8;
                            else
                            {
                                $a3_12 = $t0_14 < $a3_12 ? 1 : 0;
                                
                                if ($a3_12)
                                {
                                    $t0_14 = $v0_105;
                                    $t1_10 = $s0 << 1;
                                    $a0_51 = var_b8;
                                }
                                else
                                    $a0_51 = var_b8_15;
                            }
                        }
                    }
                    else if ($fp_2 != $v1_47)
                    {
                        $t1_10 = $s0 << 1;
                        $a0_51 = var_b8;
                    }
                    else
                    {
                        if ($v0_105 < $t0_14)
                        {
                            $t1_9 = $fp_2 < $v1_44 ? 1 : 0;
                            goto label_60b08;
                        }
                        
                        $t1_10 = $s0 << 1;
                        $a0_51 = var_b8_16;
                    }
                }
                int32_t var_78_7_1 = $t0_14;
                int32_t $v0_106;
                int32_t $v1_48;
                $v0_106 = __ashldi3($a0_51, 0, $s0, $a3_12, var_170_2_2, var_16c_2_2, var_168_7, var_164_8);
                var_170_5 = $v0_106;
                var_16c_14 = $v1_48;
                int32_t $v0_108;
                int32_t $v1_49;
                $v0_108 = var_94_1_2($t1_10);
                uint32_t $v0_109;
                int32_t $v1_50;
                $v0_109 = __lshrdi3($v0_108, $v1_49, $s0);
                
                if ($v1_50)
                    $s2_2 = 0xffffffff;
                else if ($v0_109 >= $s2_2)
                    $s2_2 = $v0_109;
            }
            else
            {
                if ($s5 + $v0_9 < $s6_2)
                {
                    var_cc_2 = 0;
                    goto label_6071c;
                }
                
                arg7[1] = 1;
                arg7[2] = arg27;
                *arg7 = $s5;
                
                if (data_b0e04_1 == 1)
                {
                    var_cc_2 = 1;
                    goto label_6071c;
                }
                
                $s2_2 = var_b8_17;
                var_cc_2_1 = 1;
                $s5 = 0;
            }
        }
    }
    
    _ae_ev = $s2_2;
    int32_t var_c0_1_4;
    int32_t $v0_174;
    int32_t $s4_2;
    
    if (var_b8_18 != $s2_2 || data_b0e08_1 || !data_b0e0c_1)
    {
        int32_t $v0_115 = data_c46e0;
        data_b0e0c = 1;
        data_b0e08 = 0;
        int32_t* $v0_165;
        
        if ($v0_115)
        {
            if ($v0_115 != 1)
            {
                var_d0 = 0;
                var_d4 = 0;
                var_c4 = 0;
                goto label_611d0;
            }
            
            uint32_t $a0_88 = data_c46b8_2;
            uint32_t $v0_168 = var_c4_3;
            
            if (var_c4_4 < $a0_88)
            {
                uint32_t $a1_56 = $v0_19;
                
                if ($a0_88 < $v0_19)
                    $a1_56 = $a0_88;
                
                var_c4 = $a1_56;
                $v0_168 = var_c4;
            }
            
            data_c46b8_3 = $v0_168;
            uint32_t $s4_4 = $v0_168 << ($s0 & 0x1f);
            $v0_174 = var_cc_2_2;
            
            if (var_84_2($s0, $s4_4, var_d4, var_d0_2, var_170_6, var_16c_15, var_168_8, var_164_9) >= $s2_2)
                goto label_611d8;
            
            int32_t $v0_175 = fix_point_div_32($s0, $s2_2, $s4_4);
            
            if (fix_point_mult2_32($s0, $v0_21, var_d0_3) >= $v0_175)
            {
                var_d4 = fix_point_div_32($s0, $s2_2, fix_point_mult2_32($s0, $s4_4, var_d0));
                goto label_611d0;
            }
            
            int32_t $v0_181 = fix_point_div_32($s0, $s2_2, fix_point_mult2_32($s0, $s4_4, $v0_21));
            int32_t $a0_96 = $v0_23;
            
            if ($v0_181 < $v0_23)
                $a0_96 = $v0_181;
            
            var_d0_4 = $a0_96;
            int32_t* $v0_183;
            
            if (!var_cc_2_3)
            {
                if ($v0_34 == 1)
                {
                    $v0_183 = arg7;
                    goto label_61274;
                }
                
                $s4_2 = 1;
                var_d4_2 = $v0_21;
            label_610b8:
                
                if ($s6_2 >= $s5)
                {
                    int32_t $s2_9 = arg33 << ($s0 & 0x1f);
                    int32_t $s1_4 = arg34 << ($s0 & 0x1f);
                    $v0_165 = arg7;
                    
                    if (arg30 >= arg29)
                        goto label_611a0;
                    
                    var_c8 = 2;
                    
                    if (fix_point_mult2_32($s0, $s1_4, $v0_5) >= $s2_9)
                        var_c8 = fix_point_mult2_32($s0, $s2_9, $v0_3) < $s1_4 ? 1 : 0;
                }
                else
                {
                    int32_t $s2_8 = arg33 << ($s0 & 0x1f);
                    int32_t $s1_2 = arg34 << ($s0 & 0x1f);
                    int32_t $v0_189 = fix_point_mult2_32($s0, $s1_2, $v0_4) < $s2_8 ? 1 : 0;
                    $v0_165 = arg7;
                    
                    if (arg32 >= arg31)
                        goto label_611a0;
                    
                    
                    if ($v0_189)
                        var_c8 = 2;
                    
                    if (!$v0_189 || (uintptr_t)arg33 < 0x29)
                    {
                        var_c8 = 0;
                        
                        if (fix_point_mult2_32($s0, $s2_8, $v0_2) < $s1_2)
                            var_c8 = ((uintptr_t)arg34 < 0x29 ? 1 : 0) ^ 1;
                    }
                }
                
                $v0_165 = arg7;
                goto label_611a0;
            }
            
            $v0_183 = arg7;
        label_61274:
            $v0_183[3] = var_c8;
            var_d4_3 = $v0_21;
            var_c0_1_5 = $v0_38 + ($(uintptr_t)v0_38 < 0xff ? 1 : 0);
        }
        else
        {
            int32_t $v0_116 = fix_point_mult2_32($s0, var_d4, var_d0);
            int32_t $v0_118 = var_c4 << ($s0 & 0x1f);
            int32_t $v0_121 = fix_point_div_32($s0, $s2_2, $v0_116);
            int32_t $s2_3 = $v0_19 << ($s0 & 0x1f);
            int32_t $fp_4 = fix_point_div_32($s0, $v0_21, var_d4);
            int32_t $s7_1 = fix_point_div_32($s0, $v0_23, var_d0);
            int32_t $v0_126 = fix_point_div_32($s0, $v0_42 << ($s0 & 0x1f), $v0_116);
            
            if (fix_point_mult2_32($s0, $v0_116, $v0_118) >= $s2_2)
                goto label_611d0;
            
            int32_t $a3_17;
            
            if ($v0_15)
                $a3_17 = *arg9 << ($s0 & 0x1f);
            
            int32_t $a2_49;
            
            if ($v0_15 && $s2_3 >= $a3_17)
            {
                if ($v0_15 != 1)
                {
                    $s4_2 = 0;
                    $s7_1 = 0;
                    $fp_4 = 0;
                    $s2_3 = 0;
                }
                else if ($v0_121 >= $a3_17)
                {
                    void* $s4_3 = &arg9[var_b0];
                    int32_t i_7 = fix_point_div_32($s0, *$s4_3 << ($s0 & 0x1f), $v0_118);
                    void* $a0_76 = $s4_3;
                        int32_t $v0_151 = var_b0 - 1;
                    
                    for (; (0x14 << ($s0 & 0x1f)) + $s2_3 < i_7; i_7 = *$a0_76 << ($s0 & 0x1f))
                    {
                        $a0_76 -= 4;
                        var_b0 = $v0_151;
                        
                        if (!$v0_151)
                        {
                            var_b0 = 1;
                            break;
                        }
                    }
                    
                    void* $a1_51 = &arg9[1];
                    int32_t $v0_154 = 1;
                    int32_t* $a0_79;
                    
                    while (true)
                    {
                        int32_t $a0_82 = $v0_121 < *$a1_51 << ($s0 & 0x1f) ? 1 : 0;
                        $a0_79 = arg9;
                        
                        if (var_b0 < $v0_154)
                            break;
                        
                        $a1_51 += 4;
                        
                        if ($a0_82)
                        {
                            $a0_79 = arg9;
                            break;
                        }
                        
                        $v0_154 += 1;
                    }
                    
                    int32_t $v0_158 = *(&$a0_79[$v0_154] - 4) << ($s0 & 0x1f);
                    
                    if ($v0_158 < $s2_3)
                        $s2_3 = $v0_158;
                    
                    int32_t $v0_159 = fix_point_div_32($s0, $v0_121, $s2_3);
                    
                    if ($fp_4 >= $v0_159)
                    {
                        $s7_1 = 1 << ($s0 & 0x1f);
                        $fp_4 = $v0_159;
                        $s4_2 = 0;
                    }
                    else
                    {
                        int32_t $v0_160 = fix_point_div_32($s0, $v0_159, $fp_4);
                        
                        if ($v0_160 < $s7_1)
                            $s7_1 = $v0_160;
                        
                        $s4_2 = 1;
                    }
                }
                else if (!$v0_16)
                {
                    $s2_3 = $v0_121 >> ($s0 & 0x1f) << ($s0 & 0x1f);
                    $fp_4 = fix_point_div_32($s0, $v0_121, $s2_3);
                    $s7_1 = $v0_15 << ($s0 & 0x1f);
                    $s4_2 = 0;
                }
                else if ($v0_16 != $v0_15)
                {
                    int32_t $v0_144 = fix_point_mult2_32($s0, $a3_17, 
                        fix_point_div_32($s0, $v0_17 << ($s0 & 0x1f), $s5 << ($s0 & 0x1f)))
                        >> ($s0 & 0x1f) << ($s0 & 0x1f);
                    
                    if ($v0_144 >= $a3_17)
                        $v0_144 = $a3_17;
                    
                    $s2_3 = $v0_144;
                    $fp_4 = $v0_15 << ($s0 & 0x1f);
                    
                    if ($v0_121 < $v0_144)
                    {
                        $s7_1 = 1 << ($s0 & 0x1f);
                        $s4_2 = 0;
                    }
                    else
                    {
                        $s2_3 = $v0_121 >> ($s0 & 0x1f) << ($s0 & 0x1f);
                    label_60e38:
                        $a2_49 = $s2_3;
                    label_60e48:
                        $fp_4 = fix_point_div_32($s0, $v0_121, $a2_49);
                        $s7_1 = 1 << ($s0 & 0x1f);
                        $s4_2 = 0;
                    }
                }
                else
                {
                    $fp_4 = $v0_16 << ($s0 & 0x1f);
                    $s7_1 = $fp_4;
                    $s2_3 = $a3_17;
                    $s4_2 = 0;
                }
            }
            else if ($s2_3 < $v0_121)
            {
                $a2_49 = $s2_3;
                
                if (fix_point_mult2_32($s0, $s2_3, $fp_4) >= $v0_121)
                    goto label_60e48;
                
                $s4_2 = 1;
                
                if (var_84($s0, $s2_3, $fp_4, $s7_1, var_170, var_16c, var_168, var_164) >= $v0_121)
                {
                    $s7_1 = fix_point_div_32($s0, $v0_121, fix_point_mult2_32($s0, $s2_3, $fp_4));
                    $s4_2 = 0;
                }
            }
            else
            {
                $s2_3 = $v0_121 >> ($s0 & 0x1f) << ($s0 & 0x1f);
                
                if ($v0_126 < $v0_121)
                    goto label_60e38;
                
                $fp_4 = 1 << ($s0 & 0x1f);
                $s7_1 = 1 << ($s0 & 0x1f);
                $s4_2 = 0;
            }
            var_c4_5 = $s2_3 >> ($s0 & 0x1f);
            var_d4_4 = fix_point_mult2_32($s0, $fp_4, var_d4_5);
            var_d0_5 = fix_point_mult2_32($s0, $s7_1, var_d0_6);
            
            if (!var_cc_2_4)
            {
                $v0_165 = arg7;
                
                if ($v0_34 == 1)
                    goto label_611a0;
                
                goto label_610b8;
            }
            
            $v0_165 = arg7;
        label_611a0:
            $v0_165[3] = var_c8_2;
            
            var_c0_1_6 = !$s4_2 ? 0 : $v0_38 + ($(uintptr_t)v0_38 < 0xff ? 1 : 0);
        }
    }
    else
    {
        var_d0 = $v0_32;
        var_d4 = $v0_31;
        var_c4 = $v0_30;
    label_611d0:
        $v0_174 = var_cc_2;
    label_611d8:
        
        if (!$v0_174)
        {
            $s4_2 = 0;
            
            if ($v0_34 != 1)
                goto label_610b8;
            
            arg7[3] = var_c8;
            var_c0_1 = 0;
        }
        else
        {
            arg7[3] = var_c8;
            var_c0_1 = 0;
        }
    }
    int32_t $a0_101 = var_cc_2_5;
    
    if (var_c0_1_7 == $v0_43)
        $a0_101 = 1;
    
    arg7[1] = $a0_101;
    arg7[4] = var_c0_1_8;
    int32_t var_cc_3 = $a0_101;
    *arg6 = var_c4_6;
    arg6[1] = var_d4_6;
    arg6[2] = var_d0_7;
    return arg6;
}

