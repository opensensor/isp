/*
 * TX-ISP Missing Functions Header
 * Function declarations for all implemented missing functions
 */

#ifndef __TX_ISP_MISSING_FUNCS_H__
#define __TX_ISP_MISSING_FUNCS_H__

#include <linux/types.h>
#include <linux/workqueue.h>

/* ===== SYSTEM/UTILITY FUNCTIONS ===== */

extern struct workqueue_struct *system_wq;

int system_yvu_or_yuv(int format_flag, u32 reg_addr, u32 color_data);
int table_intp(int arg1, int *table, int table_size, int value);
int32_t tisp_log2_fixed_to_fixed_64(uint64_t val, int32_t in_fix_point, uint8_t out_fix_point);
int64_t tisp_log2_int_to_fixed_64(uint64_t val, int32_t arg2, uint8_t arg3, uint8_t arg4);
int tisp_simple_intp(int index, int factor, int *table);
uint32_t tisp_top_read(void);

/* ===== AE (AUTO EXPOSURE) FUNCTIONS ===== */

uint8_t tisp_ae_g_luma(uint8_t *luma_out);
int tisp_ae_param_array_set(int param_id, void *data, int *size_out);
int tisp_ae_target(u32 target_val, int *input_table, int *output_table, int shift_bits);
int tisp_ae_tune(int *param1, int *param2, int *param3, int step_size, int precision, int max_val);
int tisp_ae_trig(void);

/* Placeholder declarations for remaining AE functions */
int tisp_adr_ev_update(int ev);
int tisp_adr_param_array_set(int id, void *data, int *size);
int tisp_ae_algo_handle(void *data);
int tisp_ae_g_at_list(void *list);
int tisp_ae_g_comp(void *comp);
int tisp_ae_g_min(void *min_val);
int tisp_ae_g_scene_luma(void *luma);
int tisp_ae_get_antiflicker_step(void);
int tisp_ae_manual_get(void *manual);
int tisp_ae_manual_set(void *manual);
int tisp_ae_mean_update(void *mean);
int tisp_ae_min_max_set(void *min_max);
int tisp_ae_s_at_list(void *list);
int tisp_ae_s_comp(void *comp);
int tisp_ae_s_min(void *min_val);
int tisp_ae_set_hist_custome(void *hist);
int tisp_ae_state_get(void *state);

/* ===== AWB (AUTO WHITE BALANCE) FUNCTIONS ===== */

int tisp_awb_algo_handle(void *awb_data);
int tisp_awb_param_array_set(int param_id, void *data, int *size_out);
uint32_t tisp_awb_get_ct(uint32_t *ct_out);
int tisp_awb_set_ct(uint32_t ct_value);
int tisp_awb_ev_update(int ev_value);
int tisp_awb_get_frz(int *freeze_out);
int tisp_awb_set_frz(int freeze_val);
int tisp_awb_get_zone(void *zone_data);
int tisp_awb_get_ct_trend(int *trend_out);
int tisp_awb_set_ct_trend(int trend_val);

/* Placeholder declarations for remaining AWB functions */
int tisp_awb_get_cluster_awb_params(void *params);
int tisp_awb_param_array_get(int id, void *data, int *size);
int tisp_awb_set_cluster_awb_params(void *params);

/* ===== BCSH (BRIGHTNESS/CONTRAST/SATURATION/HUE) FUNCTIONS ===== */

uint32_t tisp_bcsh_g_brightness(void);
uint32_t tisp_bcsh_g_contrast(void);
uint32_t tisp_bcsh_g_saturation(void);
uint32_t tisp_bcsh_g_hue(void);
uint32_t tisp_get_brightness(void);
uint32_t tisp_get_contrast(void);
uint32_t tisp_get_saturation(void);
uint32_t tisp_get_bcsh_hue(void);
int tisp_bcsh_set_attr(uint8_t brightness, uint8_t contrast, uint8_t saturation, uint8_t hue);
int tisp_bcsh_get_attr(uint8_t *brightness, uint8_t *contrast, uint8_t *saturation, uint8_t *hue);
int tisp_bcsh_param_array_set(int param_id, void *data, int *size_out);
int tisp_bcsh_ev_update(int ev_value);
int tisp_bcsh_ct_update(int ct_value);
int tisp_bcsh_g_rgb_coefft(void *coefft_out);
int tisp_bcsh_s_rgb_coefft(void *coefft_in);
int tisp_bcsh_set_mjpeg_contrast(int contrast);

/* ===== DMSC (DEMOSAIC) FUNCTIONS ===== */

int tisp_dmsc_param_array_set(int param_id, void *data, int *size_out);
int tisp_dmsc_intp(int ev_gain);
int tisp_dmsc_refresh(int param);

/* Placeholder declarations for remaining DMSC functions */
int tisp_dmsc_alias_par_cfg(void *cfg);
int tisp_dmsc_all_reg_refresh(int param);
int tisp_dmsc_awb_gain_par_cfg(void *cfg);
int tisp_dmsc_d_ud_ns_par_cfg(void *cfg);
int tisp_dmsc_deir_par_cfg(void *cfg);
int tisp_dmsc_deir_rgb_par_cfg(void *cfg);
int tisp_dmsc_dir_par_cfg(void *cfg);
int tisp_dmsc_fc_par_cfg(void *cfg);
int tisp_dmsc_get_par_cfg(void *cfg);
int tisp_dmsc_intp_reg_refresh(void);
int tisp_dmsc_nor_par_cfg(void *cfg);
int tisp_dmsc_out_opt_cfg(void *cfg);
int tisp_dmsc_param_array_get(int id, void *data, int *size);
int tisp_dmsc_rgb_alias_par_cfg(void *cfg);
int tisp_dmsc_sharpness_get(void);
int tisp_dmsc_sharpness_set(int sharpness);
int tisp_dmsc_sp_alias_par_cfg(void *cfg);
int tisp_dmsc_sp_d_b_wei_np_cfg(void *cfg);
int tisp_dmsc_sp_d_par_cfg(void *cfg);
int tisp_dmsc_sp_d_sigma_3_np_cfg(void *cfg);
int tisp_dmsc_sp_d_w_wei_np_cfg(void *cfg);
int tisp_dmsc_sp_ud_b_wei_np_cfg(void *cfg);
int tisp_dmsc_sp_ud_par_cfg(void *cfg);
int tisp_dmsc_sp_ud_w_wei_np_cfg(void *cfg);
int tisp_dmsc_uu_np_cfg(void *cfg);
int tisp_dmsc_uu_par_cfg(void *cfg);

/* ===== CONTROL AND CONFIGURATION FUNCTIONS ===== */

int tisp_flip_enable(int enable);
int tisp_hv_flip_enable(int flip_mode);
int tisp_mirror_enable(int enable);
int tisp_hv_flip_get(int *flip_status);

/* Frame control functions */
int tisp_set_fps(uint32_t fps_packed);
int tisp_set_frame_drop(int channel, int *drop_config, int param3);
int tisp_get_frame_drop(int channel, int *drop_config);

/* ===== AF (AUTO FOCUS) FUNCTION STUBS ===== */

int tisp_af_get_attr(void *attr);
int tisp_af_set_attr(void *attr);
int tisp_af_set_attr_refresh(void);
int tisp_af_get_metric(void *metric);
int tisp_af_get_statistics(void *stats);
int tisp_af_param_array_set(int id, void *data, int *size);
int tisp_af_process_impl(void *data);

/* ===== CCM (COLOR CORRECTION MATRIX) FUNCTION STUBS ===== */

int tisp_ccm_get_attr(void *attr);
int tisp_ccm_set_attr(void *attr);
int tisp_ccm_param_array_set(int id, void *data, int *size);

/* ===== DPC (DEAD PIXEL CORRECTION) FUNCTION STUBS ===== */

int tisp_dpc_cor_par_cfg(void *cfg);
int tisp_dpc_d_m1_par_cfg(void *cfg);
int tisp_dpc_d_m2_par_cfg(void *cfg);
int tisp_dpc_d_m3_par_cfg(void *cfg);
int tisp_dpc_intp(int param);
int tisp_dpc_intp_reg_refresh(void);
int tisp_dpc_param_array_set(int id, void *data, int *size);
int tisp_dpc_refresh(void);
int tisp_dpc_s_par_cfg(void *cfg);

/* ===== NOISE REDUCTION FUNCTION STUBS ===== */

/* MDNS Functions */
int tisp_mdns_bypass(int enable);
int tisp_mdns_c_2d_param_cfg(void *cfg);
int tisp_mdns_c_3d_param_cfg(void *cfg);
int tisp_mdns_intp(int param);
int tisp_mdns_intp_reg_refresh(void);
int tisp_mdns_param_array_set(int id, void *data, int *size);
int tisp_mdns_refresh(void);
int tisp_mdns_top_func_cfg(void *cfg);
int tisp_mdns_top_func_refresh(void);
int tisp_mdns_y_2d_param_cfg(void *cfg);
int tisp_mdns_y_3d_param_cfg(void *cfg);

/* RDNS Functions */
int tisp_rdns_all_reg_refresh(void);
int tisp_rdns_awb_gain_par_cfg(void *cfg);
int tisp_rdns_awb_gain_updata(void);
int tisp_rdns_gain_update(int gain);
int tisp_rdns_gray_np_par_cfg(void *cfg);
int tisp_rdns_intp(int param);
int tisp_rdns_intp_reg_refresh(void);
int tisp_rdns_lum_np_par_cfg(void *cfg);
int tisp_rdns_opt_cfg(void *cfg);
int tisp_rdns_param_array_set(int id, void *data, int *size);
int tisp_rdns_refresh(void);
int tisp_rdns_sl_par_cfg(void *cfg);
int tisp_rdns_slope_cfg(void *cfg);
int tisp_rdns_std_np_par_cfg(void *cfg);
int tisp_rdns_text_np_par_cfg(void *cfg);
int tisp_rdns_thres_par_cfg(void *cfg);

/* ===== LSC (LENS SHADING CORRECTION) FUNCTION STUBS ===== */

int tisp_lsc_ct_update(int ct);
int tisp_lsc_gain_update(void *gain);
int tisp_lsc_hvflip(int flip);
int tisp_lsc_lut_mirror_exchange(void);
int tisp_lsc_mirror_flip(int flip);
int tisp_lsc_param_array_set(int id, void *data, int *size);
int tisp_lsc_upside_down_lut(void);

/* ===== ADDITIONAL FUNCTION STUBS ===== */

int tisp_gamma_param_array_set(int id, void *data, int *size);
int tisp_sharpen_intp(int param);
int tisp_sharpen_intp_reg_refresh(void);
int tisp_sharpen_param_array_set(int id, void *data, int *size);
int tisp_sharpen_refresh(void);
int tisp_set_sharpness(int sharpness);
int tisp_get_sharpness(void);
int tisp_set_sensor_digital_gain(int gain);
int tisp_set_wdr_output_mode(int mode);
int tisp_get_wdr_output_mode(void);
int tisp_set_user_csc(void *csc);
int tisp_get_current_csc(void *csc);
int tisp_get_csc_attr(void *attr);
int tisp_set_csc_attr(void *attr);
int tisp_get_blc_attr(void *attr);

#endif /* __TX_ISP_MISSING_FUNCS_H__ */
