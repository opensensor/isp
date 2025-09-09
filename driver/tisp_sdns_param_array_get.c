#include "include/main.h"


  int32_t tisp_sdns_param_array_get(int32_t arg1, int32_t arg2, int32_t* arg3)

{
    if (arg1 - 0x105 >= 0x7b)
    {
        int32_t var_18_1_13 = arg1;
        isp_printf(2, &$LC0, "tisp_sdns_param_array_get");
        return 0xffffffff;
    }
    
    void* $a1_1;
    int32_t $s1_1;
    
    switch (arg1)
    {
        case 0x105:
        {
            $a1_1 = &sdns_aa_mv_det_opt;
            $s1_1 = 0x1c;
            break;
        }
        case 0x106:
        {
            $a1_1 = &sdns_grad_zx_thres_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x107:
        {
            $a1_1 = &sdns_grad_zy_thres_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x108:
        {
            $a1_1 = &sdns_std_thr1_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x109:
        {
            $a1_1 = &sdns_std_thr2_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x10a:
        {
            $a1_1 = &sdns_h_mv_wei;
            $s1_1 = 0x10;
            break;
        }
        case 0x10b:
        {
            $a1_1 = &sdns_mv_num_thr_5x5_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x10c:
        {
            $a1_1 = &sdns_mv_num_thr_7x7_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x10d:
        {
            $a1_1 = &sdns_mv_num_thr_9x9_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x10e:
        {
            $a1_1 = &sdns_mv_num_thr_11x11_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x10f:
        {
            $a1_1 = &sdns_r_s;
            $s1_1 = 0x3c;
            break;
        }
        case 0x110:
        {
            $a1_1 = &sdns_r_mv;
            $s1_1 = 0x3c;
            break;
        }
        case 0x111:
        {
            $a1_1 = &sdns_h_s_1_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x112:
        {
            $a1_1 = &sdns_h_s_2_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x113:
        {
            $a1_1 = &sdns_h_s_3_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x114:
        {
            $a1_1 = &sdns_h_s_4_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x115:
        {
            $a1_1 = &sdns_h_s_5_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x116:
        {
            $a1_1 = &sdns_h_s_6_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x117:
        {
            $a1_1 = &sdns_h_s_7_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x118:
        {
            $a1_1 = &sdns_h_s_8_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x119:
        {
            $a1_1 = &sdns_h_s_9_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x11a:
        {
            $a1_1 = &sdns_h_s_10_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x11b:
        {
            $a1_1 = &sdns_h_s_11_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x11c:
        {
            $a1_1 = &sdns_h_s_12_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x11d:
        {
            $a1_1 = &sdns_h_s_13_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x11e:
        {
            $a1_1 = &sdns_h_s_14_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x11f:
        {
            $a1_1 = &sdns_h_s_15_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x120:
        {
            $a1_1 = &sdns_h_s_16_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x121:
        {
            $a1_1 = &sdns_h_mv_1_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x122:
        {
            $a1_1 = &sdns_h_mv_2_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x123:
        {
            $a1_1 = &sdns_h_mv_3_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x124:
        {
            $a1_1 = &sdns_h_mv_4_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x125:
        {
            $a1_1 = &sdns_h_mv_5_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x126:
        {
            $a1_1 = &sdns_h_mv_6_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x127:
        {
            $a1_1 = &sdns_h_mv_7_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x128:
        {
            $a1_1 = &sdns_h_mv_8_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x129:
        {
            $a1_1 = &sdns_h_mv_9_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x12a:
        {
            $a1_1 = &sdns_h_mv_10_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x12b:
        {
            $a1_1 = &sdns_h_mv_11_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x12c:
        {
            $a1_1 = &sdns_h_mv_12_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x12d:
        {
            $a1_1 = &sdns_h_mv_13_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x12e:
        {
            $a1_1 = &sdns_h_mv_14_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x12f:
        {
            $a1_1 = &sdns_h_mv_15_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x130:
        {
            $a1_1 = &sdns_h_mv_16_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x131:
        {
            $a1_1 = &sdns_dark_thres_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x132:
        {
            $a1_1 = &sdns_light_thres_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x133:
        {
            $a1_1 = &sdns_h_val_max;
            $s1_1 = 4;
            break;
        }
        case 0x134:
        {
            $a1_1 = &sdns_sharpen_tt_opt_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x135:
        {
            $a1_1 = &sdns_d_s1_thr;
            $s1_1 = 0x3c;
            break;
        }
        case 0x136:
        {
            $a1_1 = &sdns_w_thr;
            $s1_1 = 0x40;
            break;
        }
        case 0x137:
        {
            $a1_1 = &sdns_ave_fliter;
            $s1_1 = 0xc;
            break;
        }
        case 0x138:
        {
            $a1_1 = &sdns_y;
            $s1_1 = 0xc;
            break;
        }
        case 0x139:
        {
            $a1_1 = &sdns_x_1xg_1x4;
            $s1_1 = 0x100;
            break;
        }
        case 0x13a:
        {
            $a1_1 = &sdns_k_1xg_1x4;
            $s1_1 = 0x40;
            break;
        }
        case 0x13b:
        {
            $a1_1 = &sdns_h_val;
            $s1_1 = 0x40;
            break;
        }
        case 0x13c:
        {
            $a1_1 = &sdns_sharpen_g_std;
            $s1_1 = 8;
            break;
        }
        case 0x13d:
        {
            $a1_1 = &sdns_sp_uu_par;
            $s1_1 = 0xc;
            break;
        }
        case 0x13e:
        {
            $a1_1 = &sdns_sp_uu_thres_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x13f:
        {
            $a1_1 = &sdns_sp_uu_stren_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x140:
        {
            $a1_1 = &sdns_sp_mv_uu_thres_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x141:
        {
            $a1_1 = &sdns_sp_mv_uu_stren_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x142:
        {
            $a1_1 = &sdns_sp_mv_wei_uu_value;
            $s1_1 = 0x10;
            break;
        }
        case 0x143:
        {
            $a1_1 = &sdns_sp_d_v2_sigma_win5_slope;
            $s1_1 = 8;
            break;
        }
        case 0x144:
        {
            $a1_1 = &sdns_sp_d_v2_win5_thres_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x145:
        {
            $a1_1 = &sdns_sp_d_wbhl_flat;
            $s1_1 = 0x24;
            break;
        }
        case 0x146:
        {
            $a1_1 = &sdns_sp_d_w_sp_stren_0_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x147:
        {
            $a1_1 = &sdns_sp_d_w_sp_stren_1_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x148:
        {
            $a1_1 = &sdns_sp_d_w_sp_stren_2_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x149:
        {
            $a1_1 = &sdns_sp_d_w_sp_stren_3_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x14a:
        {
            $a1_1 = &sdns_sp_d_b_sp_stren_0_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x14b:
        {
            $a1_1 = &sdns_sp_d_b_sp_stren_1_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x14c:
        {
            $a1_1 = &sdns_sp_d_b_sp_stren_2_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x14d:
        {
            $a1_1 = &sdns_sp_d_b_sp_stren_3_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x14e:
        {
            $a1_1 = &sdns_sp_d_flat_thres_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x14f:
        {
            $a1_1 = &sdns_sp_d_flat_stren_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x150:
        {
            $a1_1 = &sdns_sp_ud_v2_1_coef;
            $s1_1 = 0x20;
            break;
        }
        case 0x151:
        {
            $a1_1 = &sdns_sp_ud_w_sp_stren_0_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x152:
        {
            $a1_1 = &sdns_sp_ud_w_sp_stren_1_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x153:
        {
            $a1_1 = &sdns_sp_ud_w_sp_stren_2_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x154:
        {
            $a1_1 = &sdns_sp_ud_w_sp_stren_3_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x155:
        {
            $a1_1 = &sdns_sp_ud_b_sp_stren_0_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x156:
        {
            $a1_1 = &sdns_sp_ud_b_sp_stren_1_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x157:
        {
            $a1_1 = &sdns_sp_ud_b_sp_stren_2_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x158:
        {
            $a1_1 = &sdns_sp_ud_b_sp_stren_3_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x159:
        {
            $a1_1 = &sdns_sp_ud_std_thres_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x15a:
        {
            $a1_1 = &sdns_sp_ud_std_stren_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x15b:
        {
            $a1_1 = &sdns_sp_ud_flat_thres_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x15c:
        {
            $a1_1 = &sdns_sp_ud_flat_stren_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x15d:
        {
            $a1_1 = &sdns_sp_ud_wbhl_flat;
            $s1_1 = 0x2c;
            break;
        }
        case 0x15e:
        {
            $a1_1 = &sdns_sp_uu_np_array;
            $s1_1 = 0x40;
            break;
        }
        case 0x15f:
        {
            $a1_1 = &sdns_sp_d_w_wei_np_array;
            $s1_1 = 0x58;
            break;
        }
        case 0x160:
        {
            $a1_1 = &sdns_sp_d_b_wei_np_array;
            $s1_1 = 0x58;
            break;
        }
        case 0x161:
        {
            $a1_1 = &sdns_sp_ud_w_wei_np_array;
            $s1_1 = 0x58;
            break;
        }
        case 0x162:
        {
            $a1_1 = &rgbg_dis;
            $s1_1 = 0x24;
            break;
        }
        case 0x163:
        {
            $a1_1 = &sdns_grad_zx_thres_wdr_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x164:
        {
            $a1_1 = &sdns_grad_zy_thres_wdr_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x165:
        {
            $a1_1 = &sdns_std_thr1_wdr_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x166:
        {
            $a1_1 = &sdns_std_thr2_wdr_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x167:
        {
            $a1_1 = &sdns_h_mv_wei_wdr;
            $s1_1 = 0x10;
            break;
        }
        case 0x168:
        {
            $a1_1 = &sdns_h_s_1_wdr_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x169:
        {
            $a1_1 = &sdns_h_s_2_wdr_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x16a:
        {
            $a1_1 = &sdns_h_s_3_wdr_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x16b:
        {
            $a1_1 = &sdns_h_s_4_wdr_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x16c:
        {
            $a1_1 = &sdns_h_s_5_wdr_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x16d:
        {
            $a1_1 = &sdns_h_s_6_wdr_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x16e:
        {
            $a1_1 = &sdns_h_s_7_wdr_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x16f:
        {
            $a1_1 = &sdns_h_s_8_wdr_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x170:
        {
            $a1_1 = &sdns_h_s_9_wdr_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x171:
        {
            $a1_1 = &sdns_h_s_10_wdr_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x172:
        {
            $a1_1 = &sdns_h_s_11_wdr_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x173:
        {
            $a1_1 = &sdns_h_s_12_wdr_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x174:
        {
            $a1_1 = &sdns_h_s_13_wdr_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x175:
        {
            $a1_1 = &sdns_h_s_14_wdr_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x176:
        {
            $a1_1 = &sdns_h_s_15_wdr_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x177:
        {
            $a1_1 = &sdns_h_s_16_wdr_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x178:
        {
            $a1_1 = &sdns_sharpen_tt_opt_wdr_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x179:
        {
            $a1_1 = &sdns_ave_fliter_wdr;
            $s1_1 = 0xc;
            break;
        }
        case 0x17a:
        {
            $a1_1 = &sdns_ave_thres_wdr_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x17b:
        {
            $a1_1 = &sdns_sp_uu_thres_wdr_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x17c:
        {
            $a1_1 = &sdns_sp_uu_stren_wdr_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x17d:
        {
            $a1_1 = &sdns_sp_mv_uu_thres_wdr_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x17e:
        {
            $a1_1 = &sdns_sp_mv_uu_stren_wdr_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x17f:
        {
            $a1_1 = &sdns_sp_ud_b_wei_np_array;
            $s1_1 = 0x58;
            break;
        }
    }
    
    memcpy(arg2, $a1_1, $s1_1);
    *arg3 = $s1_1;
    return 0;
}

