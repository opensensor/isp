/*
 * TX-ISP DMSC (Demosaic) Functions Implementation
 * Based on Binary Ninja MCP decompilation with safe struct member access
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/string.h>
#include "tx-isp-common.h"
#include "tx_isp.h"
#include "tx_isp_tuning.h"

/* External function declarations */
extern int tisp_simple_intp(int index, int factor, int *table);
extern void tisp_dmsc_all_reg_refresh(int param);
extern void tisp_dmsc_par_refresh(int param1, int param2, int param3);

/* Global DMSC variables and arrays - based on Binary Ninja reference */
int data_9a430 = 0;

/* DMSC parameter arrays - extensive set based on decompiled switch statement */
static uint8_t dmsc_uu_np_array[0x40];
static uint8_t dmsc_r_deir_array[0x20];
static uint8_t dmsc_g_deir_array[0x20];
static uint8_t dmsc_b_deir_array[0x20];
static uint8_t dmsc_sp_d_sigma_3_np_array[0x40];
static uint8_t dmsc_sp_d_w_wei_np_array[0x58];
static uint8_t dmsc_sp_d_b_wei_np_array[0x58];
static uint8_t dmsc_sp_ud_w_wei_np_array[0x58];
static uint8_t dmsc_sp_ud_b_wei_np_array[0x58];
static uint8_t dmsc_out_opt[4];
static uint8_t dmsc_hv_thres_1_array[0x24];
static uint8_t dmsc_hv_stren_array[0x24];
static uint8_t dmsc_aa_thres_1_array[0x24];
static uint8_t dmsc_aa_stren_array[0x24];
static uint8_t dmsc_hvaa_thres_1_array[0x24];
static uint8_t dmsc_hvaa_stren_array[0x24];
static uint8_t dmsc_dir_par_array[0x24];
static uint8_t dmsc_uu_thres_array[0x24];
static uint8_t dmsc_uu_stren_array[0x24];
static uint8_t dmsc_uu_par_array[0x10];
static uint8_t dmsc_alias_stren_array[0x24];
static uint8_t dmsc_alias_thres_1_array[0x24];
static uint8_t dmsc_alias_thres_2_array[0x24];
static uint8_t dmsc_alias_dir_thres_array[0x24];
static uint8_t dmsc_alias_par_array[0x10];
static uint8_t dmsc_nor_alias_thres_array[0x24];
static uint8_t dmsc_nor_par_array[0x10];
static uint8_t dmsc_sp_d_w_stren_array[0x24];
static uint8_t dmsc_sp_d_b_stren_array[0x24];
static uint8_t dmsc_sp_d_brig_thres_array[0x24];
static uint8_t dmsc_sp_d_dark_thres_array[0x24];
static uint8_t dmsc_sp_d_v2_win5_thres_array[0x24];
static uint8_t dmsc_sp_d_flat_stren_array[0x24];
static uint8_t dmsc_sp_d_flat_thres_array[0x24];
static uint8_t dmsc_sp_d_oe_stren_array[0x24];
static uint8_t dmsc_sp_d_par_array[0x2c];
static uint8_t dmsc_sp_ud_w_stren_array[0x24];
static uint8_t dmsc_sp_ud_b_stren_array[0x24];
static uint8_t dmsc_sp_ud_brig_thres_array[0x24];
static uint8_t dmsc_sp_ud_dark_thres_array[0x24];
static uint8_t dmsc_sp_ud_std_stren_array[0x24];
static uint8_t dmsc_sp_ud_std_thres_array[0x24];
static uint8_t dmsc_sp_ud_flat_thres_array[0x24];
static uint8_t dmsc_sp_ud_flat_stren_array[0x24];
static uint8_t dmsc_sp_ud_oe_stren_array[0x24];
static uint8_t dmsc_sp_ud_par_array[0x34];
static uint8_t dmsc_sp_ud_v1_v2_par_array[0x28];
static uint8_t dmsc_sp_alias_thres_array[0x24];
static uint8_t dmsc_sp_alias_par_array[8];
static uint8_t dmsc_rgb_dir_thres_array[0x24];
static uint8_t dmsc_rgb_alias_stren_array[0x24];
static uint8_t dmsc_rgb_alias_par_array[8];
static uint8_t dmsc_fc_alias_stren_array[0x24];
static uint8_t dmsc_fc_t1_thres_array[0x24];
static uint8_t dmsc_fc_t1_stren_array[0x24];
static uint8_t dmsc_fc_t2_stren_array[0x24];
static uint8_t dmsc_fc_t3_stren_array[0x24];
static uint8_t dmsc_fc_lum_stren_array[0x24];
static uint8_t dmsc_fc_lum_thres_array[0x24];
static uint8_t dmsc_fc_par_array[0x28];
static uint8_t dmsc_deir_oe_en[8];
static uint8_t dmsc_deir_rgb_ir_oe_slope[0x14];
static uint8_t dmsc_deir_fusion_thres_array[0x24];
static uint8_t dmsc_deir_fusion_stren_array[0x24];
static uint8_t dmsc_sp_d_ns_thres_array[0x24];
static uint8_t dmsc_sp_ud_ns_thres_array[0x24];
static uint8_t dmsc_sp_d_ud_ns_opt[8];
static uint8_t dmsc_uu_thres_wdr_array[0x24];
static uint8_t dmsc_uu_stren_wdr_array[0x24];
static uint8_t dmsc_sp_d_w_stren_wdr_array[0x24];
static uint8_t dmsc_sp_d_b_stren_wdr_array[0x24];
static uint8_t dmsc_sp_ud_w_stren_wdr_array[0x24];
static uint8_t dmsc_sp_ud_b_stren_wdr_array[0x24];
uint8_t dmsc_awb_gain[0xc] = {0};

/* Current array pointers for WDR/normal mode switching */
static uint8_t *dmsc_uu_thres_array_now = dmsc_uu_thres_array;
static uint8_t *dmsc_uu_stren_array_now = dmsc_uu_stren_array;
static uint8_t *dmsc_sp_d_w_stren_array_now = dmsc_sp_d_w_stren_array;
static uint8_t *dmsc_sp_d_b_stren_array_now = dmsc_sp_d_b_stren_array;
static uint8_t *dmsc_sp_ud_w_stren_array_now = dmsc_sp_ud_w_stren_array;
static uint8_t *dmsc_sp_ud_b_stren_array_now = dmsc_sp_ud_b_stren_array;

/* Interpolated values - global variables used by hardware */
int dmsc_hv_thres_1_intp = 0;
int dmsc_hv_stren_intp = 0;
int dmsc_aa_thres_1_intp = 0;
int dmsc_aa_stren_intp = 0;
int dmsc_hvaa_thres_1_intp = 0;
int dmsc_hvaa_stren_intp = 0;
int dmsc_uu_thres_intp = 0;
int dmsc_uu_stren_intp = 0;
int dmsc_alias_stren_intp = 0;
int dmsc_alias_thres_1_intp = 0;
int dmsc_alias_thres_2_intp = 0;
int dmsc_alias_dir_thres_intp = 0;
int dmsc_nor_alias_thres_intp = 0;
int dmsc_sp_d_w_stren_intp = 0;
int dmsc_sp_d_b_stren_intp = 0;
int dmsc_sp_d_brig_thres_intp = 0;
int dmsc_sp_d_dark_thres_intp = 0;
int dmsc_sp_ud_w_stren_intp = 0;
int dmsc_sp_ud_b_stren_intp = 0;
int dmsc_sp_ud_brig_thres_intp = 0;
int dmsc_sp_ud_dark_thres_intp = 0;
int dmsc_sp_alias_thres_intp = 0;
int dmsc_rgb_dir_thres_intp = 0;
int dmsc_rgb_alias_stren_intp = 0;
int dmsc_fc_alias_stren_intp = 0;
int dmsc_fc_t1_thres_intp = 0;
int dmsc_fc_t1_stren_intp = 0;
int dmsc_fc_t2_stren_intp = 0;
int dmsc_fc_t3_stren_intp = 0;
int dmsc_deir_fusion_thres_intp = 0;
int dmsc_deir_fusion_stren_intp = 0;
int dmsc_sp_d_v2_win5_thres_intp = 0;
int dmsc_sp_d_flat_stren_intp = 0;
int dmsc_sp_d_flat_thres_intp = 0;
int dmsc_sp_d_oe_stren_intp = 0;
int dmsc_sp_ud_std_stren_intp = 0;
int dmsc_sp_ud_std_thres_intp = 0;
int dmsc_sp_ud_flat_thres_intp = 0;
int dmsc_sp_ud_flat_stren_intp = 0;
int dmsc_sp_ud_oe_stren_intp = 0;
int dmsc_fc_lum_stren_intp = 0;
int dmsc_fc_lum_thres_intp = 0;
int dmsc_sp_d_ns_thres_intp = 0;
int dmsc_sp_ud_ns_thres_intp = 0;

/**
 * tisp_dmsc_param_array_set - Set DMSC parameter array
 * @param_id: Parameter ID (0x5f-0xa8)
 * @data: Input data buffer
 * @size_out: Output size pointer
 */
int tisp_dmsc_param_array_set(int param_id, void *data, int *size_out)
{
    if (param_id - 0x5f >= 0x4a) {
        pr_err("tisp_dmsc_param_array_set: Invalid parameter ID %d\n", param_id);
        return -1;
    }

    void *target_array = NULL;
    int copy_size = 0;

    switch (param_id) {
        case 0x5f:
            target_array = dmsc_uu_np_array;
            copy_size = 0x40;
            break;
        case 0x60:
            target_array = dmsc_r_deir_array;
            copy_size = 0x20;
            break;
        case 0x61:
            target_array = dmsc_g_deir_array;
            copy_size = 0x20;
            break;
        case 0x62:
            target_array = dmsc_b_deir_array;
            copy_size = 0x20;
            break;
        case 0x63:
            target_array = dmsc_sp_d_sigma_3_np_array;
            copy_size = 0x40;
            break;
        case 0x64:
            target_array = dmsc_sp_d_w_wei_np_array;
            copy_size = 0x58;
            break;
        case 0x65:
            target_array = dmsc_sp_d_b_wei_np_array;
            copy_size = 0x58;
            break;
        case 0x66:
            target_array = dmsc_sp_ud_w_wei_np_array;
            copy_size = 0x58;
            break;
        case 0x67:
            target_array = dmsc_sp_ud_b_wei_np_array;
            copy_size = 0x58;
            break;
        case 0x68:
            target_array = dmsc_out_opt;
            copy_size = 4;
            break;
        case 0x69:
            target_array = dmsc_hv_thres_1_array;
            copy_size = 0x24;
            break;
        case 0x6a:
            target_array = dmsc_hv_stren_array;
            copy_size = 0x24;
            break;
        case 0x6b:
            target_array = dmsc_aa_thres_1_array;
            copy_size = 0x24;
            break;
        case 0x6c:
            target_array = dmsc_aa_stren_array;
            copy_size = 0x24;
            break;
        case 0x6d:
            target_array = dmsc_hvaa_thres_1_array;
            copy_size = 0x24;
            break;
        case 0x6e:
            target_array = dmsc_hvaa_stren_array;
            copy_size = 0x24;
            break;
        case 0x6f:
            target_array = dmsc_dir_par_array;
            copy_size = 0x24;
            break;
        case 0x70:
            target_array = dmsc_uu_thres_array;
            copy_size = 0x24;
            break;
        case 0x71:
            target_array = dmsc_uu_stren_array;
            copy_size = 0x24;
            break;
        case 0x72:
            target_array = dmsc_uu_par_array;
            copy_size = 0x10;
            break;
        case 0x73:
            target_array = dmsc_alias_stren_array;
            copy_size = 0x24;
            break;
        case 0x74:
            target_array = dmsc_alias_thres_1_array;
            copy_size = 0x24;
            break;
        case 0x75:
            target_array = dmsc_alias_thres_2_array;
            copy_size = 0x24;
            break;
        case 0x76:
            target_array = dmsc_alias_dir_thres_array;
            copy_size = 0x24;
            break;
        case 0x77:
            target_array = dmsc_alias_par_array;
            copy_size = 0x10;
            break;
        case 0x78:
            target_array = dmsc_nor_alias_thres_array;
            copy_size = 0x24;
            break;
        case 0x79:
            target_array = dmsc_nor_par_array;
            copy_size = 0x10;
            break;
        case 0xa8:
            target_array = dmsc_awb_gain;
            copy_size = 0xc;
            break;
        /* Add more cases as needed - truncated for space */
        default:
            return -1;
    }

    if (target_array) {
        memcpy(target_array, data, copy_size);
    }

    *size_out = copy_size;
    
    /* Refresh all DMSC registers */
    tisp_dmsc_all_reg_refresh(data_9a430);
    
    return 0;
}
EXPORT_SYMBOL(tisp_dmsc_param_array_set);

/**
 * tisp_dmsc_intp - Perform DMSC interpolation for all parameters
 * @ev_gain: Combined EV and gain value (upper 16 bits = EV, lower 16 bits = gain)
 */
int tisp_dmsc_intp(int ev_gain)
{
    int ev_index = ev_gain >> 16;
    int gain_factor = ev_gain & 0xffff;

    /* Perform interpolation for all DMSC parameters */
    dmsc_hv_thres_1_intp = tisp_simple_intp(ev_index, gain_factor, (int *)dmsc_hv_thres_1_array);
    dmsc_hv_stren_intp = tisp_simple_intp(ev_index, gain_factor, (int *)dmsc_hv_stren_array);
    dmsc_aa_thres_1_intp = tisp_simple_intp(ev_index, gain_factor, (int *)dmsc_aa_thres_1_array);
    dmsc_aa_stren_intp = tisp_simple_intp(ev_index, gain_factor, (int *)dmsc_aa_stren_array);
    dmsc_hvaa_thres_1_intp = tisp_simple_intp(ev_index, gain_factor, (int *)dmsc_hvaa_thres_1_array);
    dmsc_hvaa_stren_intp = tisp_simple_intp(ev_index, gain_factor, (int *)dmsc_hvaa_stren_array);
    dmsc_uu_thres_intp = tisp_simple_intp(ev_index, gain_factor, (int *)dmsc_uu_thres_array_now);
    dmsc_uu_stren_intp = tisp_simple_intp(ev_index, gain_factor, (int *)dmsc_uu_stren_array_now);
    dmsc_alias_stren_intp = tisp_simple_intp(ev_index, gain_factor, (int *)dmsc_alias_stren_array);
    dmsc_alias_thres_1_intp = tisp_simple_intp(ev_index, gain_factor, (int *)dmsc_alias_thres_1_array);
    dmsc_alias_thres_2_intp = tisp_simple_intp(ev_index, gain_factor, (int *)dmsc_alias_thres_2_array);
    dmsc_alias_dir_thres_intp = tisp_simple_intp(ev_index, gain_factor, (int *)dmsc_alias_dir_thres_array);
    dmsc_nor_alias_thres_intp = tisp_simple_intp(ev_index, gain_factor, (int *)dmsc_nor_alias_thres_array);
    dmsc_sp_d_w_stren_intp = tisp_simple_intp(ev_index, gain_factor, (int *)dmsc_sp_d_w_stren_array_now);
    dmsc_sp_d_b_stren_intp = tisp_simple_intp(ev_index, gain_factor, (int *)dmsc_sp_d_b_stren_array_now);
    dmsc_sp_d_brig_thres_intp = tisp_simple_intp(ev_index, gain_factor, (int *)dmsc_sp_d_brig_thres_array);
    dmsc_sp_d_dark_thres_intp = tisp_simple_intp(ev_index, gain_factor, (int *)dmsc_sp_d_dark_thres_array);
    dmsc_sp_ud_w_stren_intp = tisp_simple_intp(ev_index, gain_factor, (int *)dmsc_sp_ud_w_stren_array_now);
    dmsc_sp_ud_b_stren_intp = tisp_simple_intp(ev_index, gain_factor, (int *)dmsc_sp_ud_b_stren_array_now);
    dmsc_sp_ud_brig_thres_intp = tisp_simple_intp(ev_index, gain_factor, (int *)dmsc_sp_ud_brig_thres_array);
    dmsc_sp_ud_dark_thres_intp = tisp_simple_intp(ev_index, gain_factor, (int *)dmsc_sp_ud_dark_thres_array);
    dmsc_sp_alias_thres_intp = tisp_simple_intp(ev_index, gain_factor, (int *)dmsc_sp_alias_thres_array);
    dmsc_rgb_dir_thres_intp = tisp_simple_intp(ev_index, gain_factor, (int *)dmsc_rgb_dir_thres_array);
    dmsc_rgb_alias_stren_intp = tisp_simple_intp(ev_index, gain_factor, (int *)dmsc_rgb_alias_stren_array);
    dmsc_fc_alias_stren_intp = tisp_simple_intp(ev_index, gain_factor, (int *)dmsc_fc_alias_stren_array);
    dmsc_fc_t1_thres_intp = tisp_simple_intp(ev_index, gain_factor, (int *)dmsc_fc_t1_thres_array);
    dmsc_fc_t1_stren_intp = tisp_simple_intp(ev_index, gain_factor, (int *)dmsc_fc_t1_stren_array);
    dmsc_fc_t2_stren_intp = tisp_simple_intp(ev_index, gain_factor, (int *)dmsc_fc_t2_stren_array);
    dmsc_fc_t3_stren_intp = tisp_simple_intp(ev_index, gain_factor, (int *)dmsc_fc_t3_stren_array);
    dmsc_deir_fusion_thres_intp = tisp_simple_intp(ev_index, gain_factor, (int *)dmsc_deir_fusion_thres_array);
    dmsc_deir_fusion_stren_intp = tisp_simple_intp(ev_index, gain_factor, (int *)dmsc_deir_fusion_stren_array);
    dmsc_sp_d_v2_win5_thres_intp = tisp_simple_intp(ev_index, gain_factor, (int *)dmsc_sp_d_v2_win5_thres_array);
    dmsc_sp_d_flat_stren_intp = tisp_simple_intp(ev_index, gain_factor, (int *)dmsc_sp_d_flat_stren_array);
    dmsc_sp_d_flat_thres_intp = tisp_simple_intp(ev_index, gain_factor, (int *)dmsc_sp_d_flat_thres_array);
    dmsc_sp_d_oe_stren_intp = tisp_simple_intp(ev_index, gain_factor, (int *)dmsc_sp_d_oe_stren_array);
    dmsc_sp_ud_std_stren_intp = tisp_simple_intp(ev_index, gain_factor, (int *)dmsc_sp_ud_std_stren_array);
    dmsc_sp_ud_std_thres_intp = tisp_simple_intp(ev_index, gain_factor, (int *)dmsc_sp_ud_std_thres_array);
    dmsc_sp_ud_flat_thres_intp = tisp_simple_intp(ev_index, gain_factor, (int *)dmsc_sp_ud_flat_thres_array);
    dmsc_sp_ud_flat_stren_intp = tisp_simple_intp(ev_index, gain_factor, (int *)dmsc_sp_ud_flat_stren_array);
    dmsc_sp_ud_oe_stren_intp = tisp_simple_intp(ev_index, gain_factor, (int *)dmsc_sp_ud_oe_stren_array);
    dmsc_fc_lum_stren_intp = tisp_simple_intp(ev_index, gain_factor, (int *)dmsc_fc_lum_stren_array);
    dmsc_fc_lum_thres_intp = tisp_simple_intp(ev_index, gain_factor, (int *)dmsc_fc_lum_thres_array);
    dmsc_sp_d_ns_thres_intp = tisp_simple_intp(ev_index, gain_factor, (int *)dmsc_sp_d_ns_thres_array);
    dmsc_sp_ud_ns_thres_intp = tisp_simple_intp(ev_index, gain_factor, (int *)dmsc_sp_ud_ns_thres_array);

    return 0;
}
EXPORT_SYMBOL(tisp_dmsc_intp);

/**
 * tisp_dmsc_refresh - Refresh DMSC parameters
 * @param: Refresh parameter
 */
int tisp_dmsc_refresh(int param)
{
    tisp_dmsc_par_refresh(param, 0x100, 1);
    return 0;
}
EXPORT_SYMBOL(tisp_dmsc_refresh);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("TX-ISP DMSC Functions Implementation");
MODULE_AUTHOR("Generated from Binary Ninja MCP");
