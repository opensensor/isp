#include "include/main.h"


  int32_t tiziano_adr_params_init()

{
    void* $v0_1;
    
    if (adr_wdr_en)
    {
        adr_ctc_map2cut_y_now = &adr_ctc_map2cut_y_wdr;
        adr_light_end_now = &adr_light_end_wdr;
        adr_map_mode_now = &adr_map_mode_wdr;
        adr_ev_list_now = &adr_ev_list_wdr;
        adr_ligb_list_now = &adr_ligb_list_wdr;
        adr_mapb1_list_now = &adr_mapb1_list_wdr;
        adr_mapb2_list_now = &adr_mapb2_list_wdr;
        adr_mapb3_list_now = &adr_mapb3_list_wdr;
        adr_mapb4_list_now = &adr_mapb4_list_wdr;
        adr_block_light_now = &adr_block_light_wdr;
        $v0_1 = &adr_blp2_list_wdr;
    }
    else
    {
        adr_ctc_map2cut_y_now = &adr_ctc_map2cut_y;
        adr_light_end_now = &adr_light_end;
        adr_map_mode_now = &adr_map_mode;
        adr_ev_list_now = &adr_ev_list;
        adr_ligb_list_now = &adr_ligb_list;
        adr_mapb1_list_now = &adr_mapb1_list;
        adr_mapb2_list_now = &adr_mapb2_list;
        adr_mapb3_list_now = &adr_mapb3_list;
        adr_mapb4_list_now = &adr_mapb4_list;
        adr_block_light_now = &adr_block_light;
        $v0_1 = &adr_blp2_list;
    }
    
    adr_blp2_list_now = $v0_1;
    system_reg_write(0x4004, param_adr_para_array << 4 | data_afae4_1 << 0x10 | data_afacc_1);
    system_reg_write(0x4448, data_afad4_1 << 0x10 | data_afad0_1);
    system_reg_write(0x444c, data_afadc_1 << 0x10 | data_afad8_1);
    system_reg_write(0x4450, data_afae0_1);
    system_reg_write(0x402c, data_af550_1 << 0x10 | param_adr_centre_w_dis_array);
    system_reg_write(0x4030, data_af558_1 << 0x10 | data_af554_1);
    system_reg_write(0x4034, data_af560_1 << 0x10 | data_af55c_1);
    system_reg_write(0x4038, data_af568_1 << 0x10 | data_af564_1);
    system_reg_write(0x403c, data_af570_1 << 0x10 | data_af56c_1);
    system_reg_write(0x4040, data_af578_1 << 0x10 | data_af574_1);
    system_reg_write(0x4044, data_af580_1 << 0x10 | data_af57c_1);
    system_reg_write(0x4048, data_af588_1 << 0x10 | data_af584_1);
    system_reg_write(0x404c, data_af590_1 << 0x10 | data_af58c_1);
    system_reg_write(0x4050, data_af598_1 << 0x10 | data_af594_1);
    system_reg_write(0x4054, data_af5a0_1 << 0x10 | data_af59c_1);
    system_reg_write(0x4058, data_af5a8_1 << 0x10 | data_af5a4_1);
    system_reg_write(0x405c, data_af5b0_1 << 0x10 | data_af5ac_1);
    system_reg_write(0x4060, data_af5b8_1 << 0x10 | data_af5b4_1);
    system_reg_write(0x4064, data_af5c0_1 << 0x10 | data_af5bc_1);
    system_reg_write(0x4068, data_af5c4_1);
    system_reg_write(0x4340, data_af808_1 << 0x10 | param_adr_ctc_kneepoint_array);
    system_reg_write(0x4344, data_af810_1 << 0x10 | data_af80c_1);
    system_reg_write(0x4348, data_af818_1 << 0x10 | data_af814_1);
    system_reg_write(0x434c, data_af820_1 << 0x10 | data_af81c_1);
    system_reg_write(0x4350, data_af824_1);
    system_reg_write(0x4368, data_af82c_1 << 0x10 | data_af828_1);
    system_reg_write(0x436c, data_af834_1[0] << 0x10 | data_af830_1);
    system_reg_write(0x4370, data_af834_2[2] << 0x10 | data_af834_3[1]);
    system_reg_write(0x4374, data_af844_1 << 0x10 | data_af834_4[3]);
    system_reg_write(0x406c, data_af6f4_1 << 0x10 | param_adr_map_kneepoint_array);
    system_reg_write(0x4070, data_af6fc_1 << 0x10 | data_af6f8_1);
    system_reg_write(0x4074, data_af704_1 << 0x10 | data_af700_1);
    system_reg_write(0x4078, data_af70c_1 << 0x10 | data_af708_1);
    system_reg_write(0x407c, data_af714_1 << 0x10 | data_af710_1);
    system_reg_write(0x4080, data_af718_1);
    system_reg_write(0x4334, 
        data_af728_1 << 0x18 | data_af724_1 << 0x10 | data_af71c_1 | data_af720_1 << 8);
    system_reg_write(0x4338, 
        data_af738_1 << 0x18 | data_af734_1 << 0x10 | data_af72c_1 | data_af730_1 << 8);
    system_reg_write(0x433c, 
        data_af748_1 << 0x18 | data_af744_1 << 0x10 | data_af73c_1 | data_af740_1 << 8);
    system_reg_write(0x4294, 
        data_afa54_1 << 0x18 | data_afa50_1 << 0x10 | param_adr_weight_20_lut_array | data_afa4c_1 << 8);
    system_reg_write(0x4298, 
        data_afa64_1 << 0x18 | data_afa60_1 << 0x10 | data_afa58_1 | data_afa5c_1 << 8);
    system_reg_write(0x429c, 
        data_afa74_1 << 0x18 | data_afa70_1 << 0x10 | data_afa68_1 | data_afa6c_1 << 8);
    system_reg_write(0x42a0, 
        data_afa84_1 << 0x18 | data_afa80_1 << 0x10 | data_afa78_1 | data_afa7c_1 << 8);
    system_reg_write(0x42a4, 
        data_afa94_1 << 0x18 | data_afa90_1 << 0x10 | data_afa88_1 | data_afa8c_1 << 8);
    system_reg_write(0x42a8, 
        data_afaa4_1 << 0x18 | data_afaa0_1 << 0x10 | data_afa98_1 | data_afa9c_1 << 8);
    system_reg_write(0x42ac, 
        data_afab4_1 << 0x18 | data_afab0_1 << 0x10 | data_afaa8_1 | data_afaac_1 << 8);
    system_reg_write(0x42b0, 
        data_afac4_1 << 0x18 | data_afac0_1 << 0x10 | data_afab8_1 | data_afabc_1 << 8);
    system_reg_write(0x42b4, 
        data_af9d4_1 << 0x18 | data_af9d0_1 << 0x10 | param_adr_weight_02_lut_array | data_af9cc_1 << 8);
    system_reg_write(0x42b8, 
        data_af9e4_1 << 0x18 | data_af9e0_1 << 0x10 | data_af9d8_1 | data_af9dc_1 << 8);
    system_reg_write(0x42bc, 
        data_af9f4_1 << 0x18 | data_af9f0_1 << 0x10 | data_af9e8_1 | data_af9ec_1 << 8);
    system_reg_write(0x42c0, 
        data_afa04_1 << 0x18 | data_afa00_1 << 0x10 | data_af9f8_1 | data_af9fc_1 << 8);
    system_reg_write(0x42c4, 
        data_afa14_1 << 0x18 | data_afa10_1 << 0x10 | data_afa08_1 | data_afa0c_1 << 8);
    system_reg_write(0x42c8, 
        data_afa24_1 << 0x18 | data_afa20_1 << 0x10 | data_afa18_1 | data_afa1c_1 << 8);
    system_reg_write(0x42cc, 
        data_afa34_1 << 0x18 | data_afa30_1 << 0x10 | data_afa28_1 | data_afa2c_1 << 8);
    system_reg_write(0x42d0, 
        data_afa44_1 << 0x18 | data_afa40_1 << 0x10 | data_afa38_1 | data_afa3c_1 << 8);
    system_reg_write(0x42d4, 
        data_af954_1 << 0x18 | data_af950_1 << 0x10 | param_adr_weight_12_lut_array | data_af94c_1 << 8);
    system_reg_write(0x42d8, 
        data_af964_1 << 0x18 | data_af960_1 << 0x10 | data_af958_1 | data_af95c_1 << 8);
    system_reg_write(0x42dc, 
        data_af974_1 << 0x18 | data_af970_1 << 0x10 | data_af968_1 | data_af96c_1 << 8);
    system_reg_write(0x42e0, 
        data_af984_1 << 0x18 | data_af980_1 << 0x10 | data_af978_1 | data_af97c_1 << 8);
    system_reg_write(0x42e4, 
        data_af994_1 << 0x18 | data_af990_1 << 0x10 | data_af988_1 | data_af98c_1 << 8);
    system_reg_write(0x42e8, 
        data_af9a4_1 << 0x18 | data_af9a0_1 << 0x10 | data_af998_1 | data_af99c_1 << 8);
    system_reg_write(0x42ec, 
        data_af9b4_1 << 0x18 | data_af9b0_1 << 0x10 | data_af9a8_1 | data_af9ac_1 << 8);
    system_reg_write(0x42f0, 
        data_af9c4_1 << 0x18 | data_af9c0_1 << 0x10 | data_af9b8_1 | data_af9bc_1 << 8);
    system_reg_write(0x4314, 
        data_af8d4_1 << 0x18 | data_af8d0_1 << 0x10 | param_adr_weight_22_lut_array | data_af8cc_1 << 8);
    system_reg_write(0x4318, 
        data_af8e4_1 << 0x18 | data_af8e0_1 << 0x10 | data_af8d8_1 | data_af8dc_1 << 8);
    system_reg_write(0x431c, 
        data_af8f4_1 << 0x18 | data_af8f0_1 << 0x10 | data_af8e8_1 | data_af8ec_1 << 8);
    system_reg_write(0x4320, 
        data_af904_1 << 0x18 | data_af900_1 << 0x10 | data_af8f8_1 | data_af8fc_1 << 8);
    system_reg_write(0x4324, 
        data_af914_1 << 0x18 | data_af910_1 << 0x10 | data_af908_1 | data_af90c_1 << 8);
    system_reg_write(0x4328, 
        data_af924_1 << 0x18 | data_af920_1 << 0x10 | data_af918_1 | data_af91c_1 << 8);
    system_reg_write(0x432c, 
        data_af934_1 << 0x18 | data_af930_1 << 0x10 | data_af928_1 | data_af92c_1 << 8);
    system_reg_write(0x4330, 
        data_af944_1 << 0x18 | data_af940_1 << 0x10 | data_af938_1 | data_af93c_1 << 8);
    system_reg_write(0x42f4, 
        data_af854_1 << 0x18 | data_af850_1 << 0x10 | param_adr_weight_21_lut_array | data_af84c_1 << 8);
    system_reg_write(0x42f8, 
        data_af864_1 << 0x18 | data_af860_1 << 0x10 | data_af858_1 | data_af85c_1 << 8);
    system_reg_write(0x42fc, 
        data_af874_1 << 0x18 | data_af870_1 << 0x10 | data_af868_1 | data_af86c_1 << 8);
    system_reg_write(0x4300, 
        data_af884_1 << 0x18 | data_af880_1 << 0x10 | data_af878_1 | data_af87c_1 << 8);
    system_reg_write(0x4304, 
        data_af894_1 << 0x18 | data_af890_1 << 0x10 | data_af888_1 | data_af88c_1 << 8);
    system_reg_write(0x4308, 
        data_af8a4_1 << 0x18 | data_af8a0_1 << 0x10 | data_af898_1 | data_af89c_1 << 8);
    system_reg_write(0x430c, 
        data_af8b4_1 << 0x18 | data_af8b0_1 << 0x10 | data_af8a8_1 | data_af8ac_1 << 8);
    system_reg_write(0x4310, 
        data_af8c4_1 << 0x18 | data_af8c0_1 << 0x10 | data_af8b8_1 | data_af8bc_1 << 8);
    system_reg_write(0x4378, data_af750_1 << 0x10 | param_adr_min_kneepoint_array_def);
    system_reg_write(0x437c, data_af758_1 << 0x10 | data_af754_1);
    system_reg_write(0x4380, data_af760_1 << 0x10 | data_af75c_1);
    system_reg_write(0x4384, data_af768_1 << 0x10 | data_af764_1);
    system_reg_write(0x4388, data_af770_1 << 0x10 | data_af76c_1);
    system_reg_write(0x438c, data_af774_1);
    system_reg_write(0x43a8, 
        data_af784_1 << 0x18 | data_af780_1 << 0x10 | data_af778_1 | data_af77c_1 << 8);
    system_reg_write(0x43ac, 
        data_af794_1 << 0x18 | data_af790_1 << 0x10 | data_af788_1 | data_af78c_1 << 8);
    system_reg_write(0x43b0, 
        data_af7a4_1 << 0x18 | data_af7a0_1 << 0x10 | data_af798_1 | data_af79c_1 << 8);
    system_reg_write(0x43b4, data_af6c4_1 << 0x10 | param_adr_coc_kneepoint_y1_array);
    system_reg_write(0x43b8, data_af6cc_1 << 0x10 | data_af6c8_1);
    system_reg_write(0x43bc, data_af6d4_1 << 0x10 | data_af6d0_1);
    system_reg_write(0x43c0, data_af6dc_1 << 0x10 | data_af6d8_1);
    system_reg_write(0x43c4, data_af6e4_1 << 0x10 | data_af6e0_1);
    system_reg_write(0x43c8, data_af6ec_1 << 0x10 | data_af6e8_1);
    system_reg_write(0x43cc, data_af694_1 << 0x10 | param_adr_coc_kneepoint_y2_array);
    system_reg_write(0x43d0, data_af69c_1 << 0x10 | data_af698_1);
    system_reg_write(0x43d4, data_af6a4_1 << 0x10 | data_af6a0_1);
    system_reg_write(0x43d8, data_af6ac_1 << 0x10 | data_af6a8_1);
    system_reg_write(0x43dc, data_af6b4_1 << 0x10 | data_af6b0_1);
    system_reg_write(0x43e0, data_af6bc_1 << 0x10 | data_af6b8_1);
    system_reg_write(0x43e4, data_af664_1 << 0x10 | param_adr_coc_kneepoint_y3_array);
    system_reg_write(0x43e8, data_af66c_1 << 0x10 | data_af668_1);
    system_reg_write(0x43ec, data_af674_1 << 0x10 | data_af670_1);
    system_reg_write(0x43f0, data_af67c_1 << 0x10 | data_af678_1);
    system_reg_write(0x43f4, data_af684_1 << 0x10 | data_af680_1);
    system_reg_write(0x43f8, data_af68c_1 << 0x10 | data_af688_1);
    system_reg_write(0x43fc, data_af634_1 << 0x10 | param_adr_coc_kneepoint_y4_array);
    system_reg_write(0x4400, data_af63c_1 << 0x10 | data_af638_1);
    system_reg_write(0x4404, data_af644_1 << 0x10 | data_af640_1);
    system_reg_write(0x4408, data_af64c_1 << 0x10 | data_af648_1);
    system_reg_write(0x440c, data_af654_1 << 0x10 | data_af650_1);
    system_reg_write(0x4410, data_af65c_1 << 0x10 | data_af658_1);
    system_reg_write(0x4414, data_af604_1 << 0x10 | param_adr_coc_kneepoint_y5_array);
    system_reg_write(0x4418, data_af60c_1 << 0x10 | data_af608_1);
    system_reg_write(0x441c, data_af614_1 << 0x10 | data_af610_1);
    system_reg_write(0x4420, data_af61c_1 << 0x10 | data_af618_1);
    system_reg_write(0x4424, data_af624_1 << 0x10 | data_af620_1);
    system_reg_write(0x4428, data_af62c_1 << 0x10 | data_af628_1);
    system_reg_write(0x442c, data_af5cc_1 << 0x10 | param_adr_coc_adjust_array);
    system_reg_write(0x4430, data_af5d4_1 << 0x10 | data_af5d0_1);
    system_reg_write(0x4434, data_af5d8_1);
    system_reg_write(0x4438, data_af5e0_1 << 0x10 | data_af5dc_1);
    system_reg_write(0x443c, data_af5e8_1 << 0x10 | data_af5e4_1);
    system_reg_write(0x4440, data_af5ec_1);
    system_reg_write(0x4444, 
        data_af5f4_1 << 8 | data_af5f8_1 << 0x10 | data_af5f0_1 | data_af5fc_1 << 0x18);
    system_reg_write(0x4484, data_af540_1 << 0x10 | param_adr_stat_block_hist_diff_array);
    system_reg_write(0x4488, data_af548_1 << 0x10 | data_af544_1);
    uint32_t width_def_1 = width_def;
    uint32_t height_def_1 = height_def;
    data_af15c_1 = height_def_1;
    data_d022c_1 = height_def_1;
    data_af158_1 = width_def_1;
    data_d0228_1 = width_def_1;
    return 0;
}

