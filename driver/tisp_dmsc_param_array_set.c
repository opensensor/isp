#include "include/main.h"


  int32_t tisp_dmsc_param_array_set(int32_t arg1, int32_t arg2, int32_t* arg3)

{
    if (arg1 - 0x5f >= 0x4a)
    {
        int32_t var_10_1_5 = arg1;
        isp_printf(2, &$LC0, "tisp_dmsc_param_array_set");
        return 0xffffffff;
    }
    
    int32_t* $a0_1;
    int32_t $a2_1;
    
    switch (arg1)
    {
        case 0x5f:
        {
            $a0_1 = &dmsc_uu_np_array;
            $a2_1 = 0x40;
            break;
        }
        case 0x60:
        {
            $a0_1 = &dmsc_r_deir_array;
            $a2_1 = 0x20;
            break;
        }
        case 0x61:
        {
            $a0_1 = &dmsc_g_deir_array;
            $a2_1 = 0x20;
            break;
        }
        case 0x62:
        {
            $a0_1 = &dmsc_b_deir_array;
            $a2_1 = 0x20;
            break;
        }
        case 0x63:
        {
            $a0_1 = &dmsc_sp_d_sigma_3_np_array;
            $a2_1 = 0x40;
            break;
        }
        case 0x64:
        {
            $a0_1 = &dmsc_sp_d_w_wei_np_array;
            $a2_1 = 0x58;
            break;
        }
        case 0x65:
        {
            $a0_1 = &dmsc_sp_d_b_wei_np_array;
            $a2_1 = 0x58;
            break;
        }
        case 0x66:
        {
            $a0_1 = &dmsc_sp_ud_w_wei_np_array;
            $a2_1 = 0x58;
            break;
        }
        case 0x67:
        {
            $a0_1 = &dmsc_sp_ud_b_wei_np_array;
            $a2_1 = 0x58;
            break;
        }
        case 0x68:
        {
            $a0_1 = &dmsc_out_opt;
            $a2_1 = 4;
            break;
        }
        case 0x69:
        {
            $a0_1 = &dmsc_hv_thres_1_array;
            $a2_1 = 0x24;
            break;
        }
        case 0x6a:
        {
            $a0_1 = &dmsc_hv_stren_array;
            $a2_1 = 0x24;
            break;
        }
        case 0x6b:
        {
            $a0_1 = &dmsc_aa_thres_1_array;
            $a2_1 = 0x24;
            break;
        }
        case 0x6c:
        {
            $a0_1 = &dmsc_aa_stren_array;
            $a2_1 = 0x24;
            break;
        }
        case 0x6d:
        {
            $a0_1 = &dmsc_hvaa_thres_1_array;
            $a2_1 = 0x24;
            break;
        }
        case 0x6e:
        {
            $a0_1 = &dmsc_hvaa_stren_array;
            $a2_1 = 0x24;
            break;
        }
        case 0x6f:
        {
            $a0_1 = &dmsc_dir_par_array;
            $a2_1 = 0x24;
            break;
        }
        case 0x70:
        {
            $a0_1 = &dmsc_uu_thres_array;
            $a2_1 = 0x24;
            break;
        }
        case 0x71:
        {
            $a0_1 = &dmsc_uu_stren_array;
            $a2_1 = 0x24;
            break;
        }
        case 0x72:
        {
            $a0_1 = &dmsc_uu_par_array;
            $a2_1 = 0x10;
            break;
        }
        case 0x73:
        {
            $a0_1 = &dmsc_alias_stren_array;
            $a2_1 = 0x24;
            break;
        }
        case 0x74:
        {
            $a0_1 = &dmsc_alias_thres_1_array;
            $a2_1 = 0x24;
            break;
        }
        case 0x75:
        {
            $a0_1 = &dmsc_alias_thres_2_array;
            $a2_1 = 0x24;
            break;
        }
        case 0x76:
        {
            $a0_1 = &dmsc_alias_dir_thres_array;
            $a2_1 = 0x24;
            break;
        }
        case 0x77:
        {
            $a0_1 = &dmsc_alias_par_array;
            $a2_1 = 0x10;
            break;
        }
        case 0x78:
        {
            $a0_1 = &dmsc_nor_alias_thres_array;
            $a2_1 = 0x24;
            break;
        }
        case 0x79:
        {
            $a0_1 = &dmsc_nor_par_array;
            $a2_1 = 0x10;
            break;
        }
        case 0x7a:
        {
            $a0_1 = &dmsc_sp_d_w_stren_array;
            $a2_1 = 0x24;
            break;
        }
        case 0x7b:
        {
            $a0_1 = &dmsc_sp_d_b_stren_array;
            $a2_1 = 0x24;
            break;
        }
        case 0x7c:
        {
            $a0_1 = &dmsc_sp_d_brig_thres_array;
            $a2_1 = 0x24;
            break;
        }
        case 0x7d:
        {
            $a0_1 = &dmsc_sp_d_dark_thres_array;
            $a2_1 = 0x24;
            break;
        }
        case 0x7e:
        {
            $a0_1 = &dmsc_sp_d_v2_win5_thres_array;
            $a2_1 = 0x24;
            break;
        }
        case 0x7f:
        {
            $a0_1 = &dmsc_sp_d_flat_stren_array;
            $a2_1 = 0x24;
            break;
        }
        case 0x80:
        {
            $a0_1 = &dmsc_sp_d_flat_thres_array;
            $a2_1 = 0x24;
            break;
        }
        case 0x81:
        {
            $a0_1 = &dmsc_sp_d_oe_stren_array;
            $a2_1 = 0x24;
            break;
        }
        case 0x82:
        {
            $a0_1 = &dmsc_sp_d_par_array;
            $a2_1 = 0x2c;
            break;
        }
        case 0x83:
        {
            $a0_1 = &dmsc_sp_ud_w_stren_array;
            $a2_1 = 0x24;
            break;
        }
        case 0x84:
        {
            $a0_1 = &dmsc_sp_ud_b_stren_array;
            $a2_1 = 0x24;
            break;
        }
        case 0x85:
        {
            $a0_1 = &dmsc_sp_ud_brig_thres_array;
            $a2_1 = 0x24;
            break;
        }
        case 0x86:
        {
            $a0_1 = &dmsc_sp_ud_dark_thres_array;
            $a2_1 = 0x24;
            break;
        }
        case 0x87:
        {
            $a0_1 = &dmsc_sp_ud_std_stren_array;
            $a2_1 = 0x24;
            break;
        }
        case 0x88:
        {
            $a0_1 = &dmsc_sp_ud_std_thres_array;
            $a2_1 = 0x24;
            break;
        }
        case 0x89:
        {
            $a0_1 = &dmsc_sp_ud_flat_thres_array;
            $a2_1 = 0x24;
            break;
        }
        case 0x8a:
        {
            $a0_1 = &dmsc_sp_ud_flat_stren_array;
            $a2_1 = 0x24;
            break;
        }
        case 0x8b:
        {
            $a0_1 = &dmsc_sp_ud_oe_stren_array;
            $a2_1 = 0x24;
            break;
        }
        case 0x8c:
        {
            $a0_1 = &dmsc_sp_ud_par_array;
            $a2_1 = 0x34;
            break;
        }
        case 0x8d:
        {
            $a0_1 = &dmsc_sp_ud_v1_v2_par_array;
            $a2_1 = 0x28;
            break;
        }
        case 0x8e:
        {
            $a0_1 = &dmsc_sp_alias_thres_array;
            $a2_1 = 0x24;
            break;
        }
        case 0x8f:
        {
            $a0_1 = &dmsc_sp_alias_par_array;
            $a2_1 = 8;
            break;
        }
        case 0x90:
        {
            $a0_1 = &dmsc_rgb_dir_thres_array;
            $a2_1 = 0x24;
            break;
        }
        case 0x91:
        {
            $a0_1 = &dmsc_rgb_alias_stren_array;
            $a2_1 = 0x24;
            break;
        }
        case 0x92:
        {
            $a0_1 = &dmsc_rgb_alias_par_array;
            $a2_1 = 8;
            break;
        }
        case 0x93:
        {
            $a0_1 = &dmsc_fc_alias_stren_array;
            $a2_1 = 0x24;
            break;
        }
        case 0x94:
        {
            $a0_1 = &dmsc_fc_t1_thres_array;
            $a2_1 = 0x24;
            break;
        }
        case 0x95:
        {
            $a0_1 = &dmsc_fc_t1_stren_array;
            $a2_1 = 0x24;
            break;
        }
        case 0x96:
        {
            $a0_1 = &dmsc_fc_t2_stren_array;
            $a2_1 = 0x24;
            break;
        }
        case 0x97:
        {
            $a0_1 = &dmsc_fc_t3_stren_array;
            $a2_1 = 0x24;
            break;
        }
        case 0x98:
        {
            $a0_1 = &dmsc_fc_lum_stren_array;
            $a2_1 = 0x24;
            break;
        }
        case 0x99:
        {
            $a0_1 = &dmsc_fc_lum_thres_array;
            $a2_1 = 0x24;
            break;
        }
        case 0x9a:
        {
            $a0_1 = &dmsc_fc_par_array;
            $a2_1 = 0x28;
            break;
        }
        case 0x9b:
        {
            $a0_1 = &dmsc_deir_oe_en;
            $a2_1 = 8;
            break;
        }
        case 0x9c:
        {
            $a0_1 = &dmsc_deir_rgb_ir_oe_slope;
            $a2_1 = 0x14;
            break;
        }
        case 0x9d:
        {
            $a0_1 = &dmsc_deir_fusion_thres_array;
            $a2_1 = 0x24;
            break;
        }
        case 0x9e:
        {
            $a0_1 = &dmsc_deir_fusion_stren_array;
            $a2_1 = 0x24;
            break;
        }
        case 0x9f:
        {
            $a0_1 = &dmsc_sp_d_ns_thres_array;
            $a2_1 = 0x24;
            break;
        }
        case 0xa0:
        {
            $a0_1 = &dmsc_sp_ud_ns_thres_array;
            $a2_1 = 0x24;
            break;
        }
        case 0xa1:
        {
            $a0_1 = &dmsc_sp_d_ud_ns_opt;
            $a2_1 = 8;
            break;
        }
        case 0xa2:
        {
            $a0_1 = &dmsc_uu_thres_wdr_array;
            $a2_1 = 0x24;
            break;
        }
        case 0xa3:
        {
            $a0_1 = &dmsc_uu_stren_wdr_array;
            $a2_1 = 0x24;
            break;
        }
        case 0xa4:
        {
            $a0_1 = &dmsc_sp_d_w_stren_wdr_array;
            $a2_1 = 0x24;
            break;
        }
        case 0xa5:
        {
            $a0_1 = &dmsc_sp_d_b_stren_wdr_array;
            $a2_1 = 0x24;
            break;
        }
        case 0xa6:
        {
            $a0_1 = &dmsc_sp_ud_w_stren_wdr_array;
            $a2_1 = 0x24;
            break;
        }
        case 0xa7:
        {
            $a0_1 = &dmsc_sp_ud_b_stren_wdr_array;
            $a2_1 = 0x24;
            break;
        }
        case 0xa8:
        {
            $a0_1 = &dmsc_awb_gain;
            $a2_1 = 0xc;
            break;
        }
    }
    
    *arg3 = $a2_1;
    memcpy($a0_1);
    tisp_dmsc_all_reg_refresh(data_9a430_4);
    return 0;
}

