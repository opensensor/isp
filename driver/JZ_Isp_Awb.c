#include "include/main.h"


  int32_t JZ_Isp_Awb()

{
    int32_t $s4 = data_983b0_2;
    _awb_ct_last = _awb_ct;
    uint64_t* $v1_8;
    
    if ($s4 < _awb_mode << 0xa)
    {
        data_b56fc_1 = &_rgbg_weight_ot;
        IspAwbCtDetectParam = &_light_src;
        data_b56f0_1 = 0;
        data_b56f4_1 = &_awb_ct_th_ot_luxhigh;
        $v1_8 = &_awb_ct_para_ot;
    }
    else if ($s4 >= data_a9f80_1 << 0xa)
    {
        data_b56fc_2 = &_rgbg_weight;
        uint32_t _light_src_num_1 = _light_src_num;
        IspAwbCtDetectParam = &_light_src;
        data_b56f0_2 = _light_src_num_1;
        data_b56f4_2 = &_awb_ct_th_in;
        $v1_8 = &_awb_ct_para_in;
    }
    else
    {
        data_b56fc_3 = &_rgbg_weight_ot;
        IspAwbCtDetectParam = &_light_src;
        data_b56f0_3 = 0;
        data_b56f4_3 = &_awb_ct_th_ot_luxlow;
        $v1_8 = &_awb_ct_para_ot;
    }
    
    int32_t $a0 = data_a9f68_1;
    data_b56f8_1 = $v1_8;
    
    if ($a0 == 1)
        data_b56fc_4 = &_rgbg_weight_global;
    
    data_b5700_1 = data_a9fc8_1;
    data_b5704_1 = data_a9fc0_1;
    data_b5708_1 = &_rg_pos;
    data_b570c_1 = &_bg_pos;
    data_b5710_1 = &_color_temp_mesh;
    data_b5714_1 = &_awb_wght;
    data_b5718_1 = &_awb_dis_tw;
    data_b571c_1 = &_awb_ct;
    data_b5720_1 = &_ls_w_lut;
    IspAwbFpgaParam = &awb_array_r;
    data_b5730_1 = &awb_array_g;
    data_b5734_1 = &awb_array_b;
    data_b5738_1 = &awb_array_p;
    data_b573c_1 = &_awb_cof;
    data_b5740_1 = &_awb_mf_para;
    data_b5748_1 = _pixel_cnt_th;
    data_b5728_1 = &_awb_cluster;
    data_b574c_1 = &_wb_static;
    data_b5754_1 = &_awb_cluster;
    data_b5724_1 = &_AwbPointPos;
    data_b5744_1 = &_awb_parameter;
    data_b5750_1 = &_AwbPointPos;
    
    for (int32_t i = 0; i < 0x40; i += 1)
    {
        char var_8c_10[0x44];
        var_8c_11[i] = *(&IspAwbCtDetectParam + i);
    }
    
    for (int32_t i_1 = 0; i_1 < 0x1c; i_1 += 1)
    {
        char var_a8_22[0x1c];
        var_a8_23[i_1] = *(&data_b573c_2 + i_1);
    }
    
    Tiziano_awb_fpga(IspAwbFpgaParam, data_b5730_2, data_b5734_2, data_b5738_2);
    
    if (tawb_custom_en == 1)
        private_complete(&awb_algo_comp);
    
    int32_t tisp_wb_attr_1 = tisp_wb_attr;
    int32_t $v0_1;
    
    if (tisp_wb_attr_1 == 1)
    {
        uint32_t $v0 = awb_ct_manual;
        
        if (!$v0)
            $v0 = _awb_ct;
        
        _awb_ct = $v0;
        $v0_1 = data_a9f68_2;
    }
    else if (tisp_wb_attr_1)
        $v0_1 = data_a9f68_3;
    else
    {
        awb_ct_manual = _awb_ct;
        $v0_1 = data_a9f68_4;
    }
    
    if (!$v0_1)
    {
        if (data_a9f84_1 << 0xa >= $s4)
        {
            if (ModeFlag == 1)
            {
                ModeFlag = 0;
                
                if (!awb_frz)
                {
                    system_reg_write_awb(1, 0xb028, data_aa048_1 << 0x10 | data_aa044_1);
                    system_reg_write_awb(1, 0xb02c, data_aa050_1 << 0x10 | data_aa04c_1);
                }
            }
        }
        else if (!ModeFlag)
        {
            ModeFlag = 1;
            
            if (!awb_frz)
            {
                system_reg_write_awb(1, 0xb028, 
                    *(_awb_lowlight_rg_th + 4) << 0x10 | *_awb_lowlight_rg_th);
                system_reg_write_awb(1, 0xb02c, 0x3ff0001);
            }
        }
    }
    
    int32_t var_40_22 = 9;
    uint32_t _awb_ct_1 = _awb_ct;
    int32_t var_34_16 = 0;
    void var_48_25;
    tisp_event_push(&var_48_26);
    return 0;
}

