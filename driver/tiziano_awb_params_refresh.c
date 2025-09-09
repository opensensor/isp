#include "include/main.h"


  int32_t tiziano_awb_params_refresh()

{
    memcpy(&_awb_parameter, 0x95b30, 0xb4);
    memcpy(&_pixel_cnt_th, 0x95be4, 4);
    memcpy(&_awb_lowlight_rg_th, 0x95be8, 8);
    memcpy(&_AwbPointPos, 0x95bf0, 8);
    memcpy(&_awb_cof, 0x95bf8, 8);
    memcpy(&_awb_mode, 0x95c18, 0xc);
    memcpy(&_wb_static, 0x95c2c, 8);
    memcpy(&_light_src, 0x95c34, 0x50);
    memcpy(&_light_src_num, 0x95c84, 4);
    memcpy(&_rg_pos, 0x95c88, 0x3c);
    memcpy(&_bg_pos, 0x95cc4, 0x3c);
    memcpy(&_awb_ct_th_ot_luxhigh, 0x95d00, 0x10);
    memcpy(&_awb_ct_th_ot_luxlow, 0x95d10, 0x10);
    memcpy(&_awb_ct_th_in, 0x95d20, 0x10);
    memcpy(&_awb_ct_para_ot, 0x95d30, 8);
    memcpy(&_awb_ct_para_in, 0x95d38, 8);
    memcpy(&_awb_dis_tw, 0x95d40, 0xc);
    memcpy(&_rgbg_weight, 0x95d4c, 0x384);
    memcpy(&_color_temp_mesh, 0x960d0, 0x384);
    memcpy(&_awb_wght, 0x96454, 0x384);
    memcpy(&_rgbg_weight_ot, 0x967d8, 0x384);
    memcpy(&_ls_w_lut, 0x96b5c, 0x808);
    
    if (!awb_dn_refresh_flag)
    {
        memcpy(&_awb_mf_para, 0x95c00, 0x18);
        memcpy(&_awb_ct, 0x95c24, 4);
        memcpy(&_awb_ct_last, 0x95c28, 4);
    }
    
    awb_dn_refresh_flag = 0;
    return 0;
}

