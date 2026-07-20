# Binary Assembly Audit

- schema: `regtrace-binary-audit-v1`
- OEM: `/home/matteius/re-framework/tx-isp-t41.ko`
- recovered: `driver/t41/tx_isp_t41_recovered.ko`
- objdump counts exclude relocation records
- thresholds: min_oem_insns=24 stub_insns=8 collapse=0.50 similar=0.80..1.25 expansion=2.00

## Summary

| Metric | OEM | Recovered |
|---|---:|---:|
| Function symbols | 1314 | 1525 |
| Functions with disassembly | 1314 | 1525 |
| Executable section bytes | 479556 | 341360 |
| Initialized writable bytes | 25120 | 17088 |
| Uninitialized writable bytes | 20592 | 302432 |

- direct matches: 1273
- replacement matches: 0 (missing=0)
- unmatched: OEM-only=41 recovered-only=252
- matched instructions: OEM=116036 recovered=82513 ratio=0.711
- classes: stub=18 collapsed=86 shorter=391 same_count=264 similar=472 larger=24 expanded=18

## Allocated Section Delta

| Section | OEM bytes | Recovered bytes | Delta |
|---|---:|---:|---:|
| `.MIPS.abiflags` | 24 | 24 | +0 |
| `.bss` | 20592 | 302432 | +281840 |
| `.data` | 24768 | 16736 | -8032 |
| `.exit.text` | 1140 | 0 | -1140 |
| `.gnu.linkonce.this_module` | 352 | 352 | +0 |
| `.init.text` | 16 | 0 | -16 |
| `.modinfo` | 1012 | 1136 | +124 |
| `.note.gnu.build-id` | 36 | 36 | +0 |
| `.reginfo` | 24 | 24 | +0 |
| `.rodata` | 15760 | 4304 | -11456 |
| `.rodata.str1.4` | 25860 | 5356 | -20504 |
| `.text` | 478400 | 341360 | -137040 |
| `__ksymtab` | 248 | 280 | +32 |
| `__ksymtab_strings` | 637 | 737 | +100 |
| `__param` | 260 | 280 | +20 |

## Function Outliers

Showing 250 of 830 outliers. JSON and CSV contain every comparison row.

| Class | Match | OEM symbol | Recovered symbol(s) | OEM insns | Recovered insns | Ratio | Delta | Calls | Branches |
|---|---|---|---|---:|---:|---:|---:|---:|---:|
| stub | exact_name | `tisp_bcsh_BCS_adjust` | `tisp_bcsh_BCS_adjust` | 355 | 2 | 0.006 | -353 | 17->0 | 28->1 |
| stub | exact_name | `ispcore_core_ops_ioctl` | `ispcore_core_ops_ioctl` | 309 | 2 | 0.006 | -307 | 8->0 | 55->0 |
| stub | exact_name | `ispint_adr_64` | `ispint_adr_64` | 101 | 3 | 0.030 | -98 | 2->0 | 14->0 |
| stub | exact_name | `func_interp1_short` | `func_interp1_short` | 67 | 2 | 0.030 | -65 | 0->0 | 10->0 |
| stub | exact_name | `func_zone_ct_weight` | `func_zone_ct_weight` | 42 | 2 | 0.048 | -40 | 0->0 | 6->1 |
| stub | exact_name | `ISPAWBInterpolation2` | `ISPAWBInterpolation2` | 42 | 4 | 0.095 | -38 | 4->0 | 2->1 |
| stub | exact_name | `tisp_irsca_para_calc` | `tisp_irsca_para_calc` | 41 | 8 | 0.195 | -33 | 0->0 | 4->1 |
| stub | exact_name | `tisp_ae_max_exp_calc_fps` | `tisp_ae_max_exp_calc_fps` | 38 | 2 | 0.053 | -36 | 1->0 | 0->0 |
| stub | exact_name | `tisp_bcsh_dn_params_refresh` | `tisp_bcsh_dn_params_refresh` | 38 | 6 | 0.158 | -32 | 3->1 | 0->0 |
| stub | exact_name | `tisp_ccm_dn_params_refresh` | `tisp_ccm_dn_params_refresh` | 38 | 6 | 0.158 | -32 | 4->1 | 0->0 |
| stub | exact_name | `tisp_mdns_dn_params_refresh` | `tisp_mdns_dn_params_refresh` | 37 | 6 | 0.162 | -31 | 4->1 | 0->0 |
| stub | exact_name | `tisp_hldc_quadratic_func` | `tisp_hldc_quadratic_func` | 35 | 3 | 0.086 | -32 | 0->0 | 0->0 |
| stub | exact_name | `tisp_g_awb_attr` | `tisp_g_awb_attr` | 35 | 6 | 0.171 | -29 | 2->1 | 0->0 |
| stub | exact_name | `tisp_hldc_para_validity_judge` | `tisp_hldc_para_validity_judge` | 34 | 2 | 0.059 | -32 | 0->0 | 4->0 |
| stub | exact_name | `tisp_gamma_dn_params_refresh` | `tisp_gamma_dn_params_refresh` | 33 | 4 | 0.121 | -29 | 4->0 | 0->1 |
| stub | exact_name | `tisp_gib_calc_self_gain` | `tisp_gib_calc_self_gain` | 32 | 2 | 0.062 | -30 | 0->0 | 2->0 |
| stub | exact_name | `tisp_set_sensor_short_analog_gain` | `tisp_set_sensor_short_analog_gain` | 32 | 4 | 0.125 | -28 | 4->0 | 0->1 |
| stub | exact_name | `tisp_g_hv_flip` | `tisp_g_hv_flip` | 27 | 2 | 0.074 | -25 | 0->0 | 0->0 |
| collapsed | exact_name | `tisp_awb_ct_detect` | `tisp_awb_ct_detect` | 3100 | 1079 | 0.348 | -2021 | 41->32 | 362->144 |
| collapsed | exact_name | `tx_isp_core_ops_s_ctrl` | `tx_isp_core_ops_s_ctrl` | 1768 | 257 | 0.145 | -1511 | 150->23 | 226->40 |
| collapsed | exact_name | `tx_isp_core_ops_g_ctrl` | `tx_isp_core_ops_g_ctrl` | 1386 | 619 | 0.447 | -767 | 100->46 | 201->124 |
| collapsed | exact_name | `tisp_ae_ev_list_alloc_calc` | `tisp_ae_ev_list_alloc_calc` | 1332 | 401 | 0.301 | -931 | 67->18 | 201->65 |
| collapsed | exact_name | `tisp_mdns_reg_cfg` | `tisp_mdns_reg_cfg` | 1308 | 305 | 0.233 | -1003 | 102->43 | 4->0 |
| collapsed | exact_name | `Tiziano_adr_fpga` | `Tiziano_adr_fpga` | 1152 | 366 | 0.318 | -786 | 18->9 | 115->47 |
| collapsed | exact_name | `tisp_ae_tune` | `tisp_ae_tune` | 932 | 46 | 0.049 | -886 | 40->4 | 82->1 |
| collapsed | exact_name | `tisp_ae_ev_alloc_calc` | `tisp_ae_ev_alloc_calc` | 931 | 325 | 0.349 | -606 | 50->17 | 126->53 |
| collapsed | exact_name | `Tisp_lce_soft` | `Tisp_lce_soft` | 915 | 65 | 0.071 | -850 | 39->7 | 83->3 |
| collapsed | exact_name | `tisp_awb_long_alogrithm` | `tisp_awb_long_alogrithm` | 863 | 382 | 0.443 | -481 | 14->11 | 80->41 |
| collapsed | exact_name | `tisp_code_tuning_ioctl` | `tisp_code_tuning_ioctl` | 780 | 76 | 0.097 | -704 | 41->6 | 119->10 |
| collapsed | exact_name | `tisp_tmo_fpga` | `tisp_tmo_fpga` | 776 | 34 | 0.044 | -742 | 8->2 | 68->2 |
| collapsed | exact_name | `tisp_ae_short_ev_alloc_calc` | `tisp_ae_short_ev_alloc_calc` | 672 | 127 | 0.189 | -545 | 40->3 | 102->17 |
| collapsed | exact_name | `subdev_sensor_ops_ioctl` | `subdev_sensor_ops_ioctl` | 498 | 42 | 0.084 | -456 | 29->2 | 78->7 |
| collapsed | exact_name | `tiziano_adr_ev_func` | `tiziano_adr_ev_func` | 492 | 204 | 0.415 | -288 | 45->0 | 12->11 |
| collapsed | exact_name | `tisp_wdr_get_data` | `tisp_wdr_get_data` | 383 | 123 | 0.321 | -260 | 20->10 | 11->5 |
| collapsed | exact_name | `tisp_ae_fliker_detect` | `tisp_ae_fliker_detect` | 366 | 154 | 0.421 | -212 | 18->16 | 31->5 |
| collapsed | exact_name | `tisp_bcsh_H_adjust` | `tisp_bcsh_H_adjust` | 284 | 61 | 0.215 | -223 | 9->6 | 15->3 |
| collapsed | exact_name | `tisp_ccm_matrix_trans_by_sat` | `tisp_ccm_matrix_trans_by_sat` | 268 | 10 | 0.037 | -258 | 20->1 | 14->1 |
| collapsed | exact_name | `tisp_wdr_default_reg_refresh` | `tisp_wdr_default_reg_refresh` | 268 | 101 | 0.377 | -167 | 32->5 | 1->8 |
| collapsed | exact_name | `tisp_ae_short_expt` | `tisp_ae_short_expt` | 241 | 105 | 0.436 | -136 | 12->7 | 24->8 |
| collapsed | exact_name | `tisp_ae_par_calc` | `tisp_ae_par_calc` | 239 | 90 | 0.377 | -149 | 8->6 | 23->1 |
| collapsed | exact_name | `tisp_tmo_ev_interp` | `tisp_tmo_ev_interp` | 211 | 99 | 0.469 | -112 | 2->1 | 42->14 |
| collapsed | exact_name | `tisp_clm_write_csc_para` | `tisp_clm_write_csc_para` | 198 | 71 | 0.359 | -127 | 16->5 | 0->0 |
| collapsed | exact_name | `vic_core_ops_ioctl` | `vic_core_ops_ioctl` | 192 | 29 | 0.151 | -163 | 7->1 | 33->5 |
| collapsed | exact_name | `tisp_g_module_attr` | `tisp_g_module_attr` | 189 | 44 | 0.233 | -145 | 4->2 | 29->3 |
| collapsed | exact_name | `tisp_s_module_attr` | `tisp_s_module_attr` | 189 | 51 | 0.270 | -138 | 4->3 | 29->4 |
| collapsed | exact_name | `tisp_blc_ae_write_dgain` | `tisp_blc_ae_write_dgain` | 184 | 77 | 0.418 | -107 | 22->4 | 6->5 |
| collapsed | exact_name | `tisp_bcsh_interp_by_ev` | `tisp_bcsh_interp_by_ev` | 182 | 82 | 0.451 | -100 | 11->0 | 12->11 |
| collapsed | exact_name | `tisp_enable_tuning` | `tisp_enable_tuning` | 178 | 40 | 0.225 | -138 | 7->3 | 9->2 |
| collapsed | exact_name | `tisp_msca_curve_calc` | `tisp_msca_curve_calc` | 167 | 66 | 0.395 | -101 | 8->4 | 15->8 |
| collapsed | exact_name | `tisp_set_scaler_level_control_set` | `tisp_set_scaler_level_control_set` | 153 | 57 | 0.373 | -96 | 6->4 | 7->7 |
| collapsed | exact_name | `tisp_awb_face_pos` | `tisp_awb_face_pos` | 151 | 71 | 0.470 | -80 | 6->3 | 12->4 |
| collapsed | exact_name | `subsection_map` | `subsection_map` | 138 | 66 | 0.478 | -72 | 4->3 | 11->5 |
| collapsed | exact_name | `lce_wdr_light_lock` | `lce_wdr_light_lock` | 134 | 52 | 0.388 | -82 | 0->0 | 14->7 |
| collapsed | exact_name | `tisp_hldc_calc_para` | `tisp_hldc_calc_para` | 127 | 33 | 0.260 | -94 | 1->0 | 5->1 |
| collapsed | exact_name | `tisp_bcsh_aitp_to_hard` | `tisp_bcsh_aitp_to_hard` | 126 | 39 | 0.310 | -87 | 2->0 | 6->1 |
| collapsed | exact_name | `func_local_info` | `func_local_info` | 121 | 46 | 0.380 | -75 | 10->5 | 2->2 |
| collapsed | exact_name | `ispcore_irq_main_fd_work` | `ispcore_irq_main_fd_work` | 118 | 39 | 0.331 | -79 | 5->1 | 20->8 |
| collapsed | exact_name | `tisp_ae_calc_process` | `tisp_ae_calc_process` | 104 | 48 | 0.462 | -56 | 6->1 | 9->6 |
| collapsed | exact_name | `fs_core_ops_ioctl` | `fs_core_ops_ioctl` | 96 | 48 | 0.500 | -48 | 5->3 | 8->3 |
| collapsed | exact_name | `tisp_tmo_interrupt_static` | `tisp_tmo_interrupt_static` | 95 | 28 | 0.295 | -67 | 6->3 | 1->0 |
| collapsed | exact_name | `tisp_defog_img_filter25` | `tisp_defog_img_filter25` | 92 | 34 | 0.370 | -58 | 0->0 | 11->5 |
| collapsed | exact_name | `tisp_lce_interrupt_static` | `tisp_lce_interrupt_static` | 91 | 26 | 0.286 | -65 | 11->2 | 8->1 |
| collapsed | exact_name | `tisp_msca_chx_cfg_load` | `tisp_msca_chx_cfg_load` | 91 | 37 | 0.407 | -54 | 9->5 | 3->0 |
| collapsed | exact_name | `ispcore_irq_main_fs_work` | `ispcore_irq_main_fs_work` | 88 | 17 | 0.193 | -71 | 2->0 | 15->3 |
| collapsed | exact_name | `tisp_wdr_process_init` | `tisp_wdr_process_init` | 88 | 40 | 0.455 | -48 | 8->2 | 8->4 |
| collapsed | exact_name | `tisp_ae_roi_point` | `tisp_ae_roi_point` | 85 | 14 | 0.165 | -71 | 6->1 | 7->0 |
| collapsed | exact_name | `tisp_msca_scaling_algorithm` | `tisp_msca_scaling_algorithm` | 83 | 32 | 0.386 | -51 | 3->3 | 0->0 |
| collapsed | exact_name | `tisp_defog_img_filter9` | `tisp_defog_img_filter9` | 76 | 14 | 0.184 | -62 | 0->0 | 5->1 |
| collapsed | exact_name | `isp_core_debug_show` | `isp_core_debug_show` | 76 | 34 | 0.447 | -42 | 2->2 | 7->3 |
| collapsed | exact_name | `tisp_tmo_api_set_curve` | `tisp_tmo_api_set_curve` | 76 | 34 | 0.447 | -42 | 5->5 | 10->1 |
| collapsed | exact_name | `func_subsection_light` | `func_subsection_light` | 75 | 20 | 0.267 | -55 | 1->0 | 5->1 |
| collapsed | exact_name | `tisp_defog_max_filter` | `tisp_defog_max_filter` | 74 | 35 | 0.473 | -39 | 0->0 | 14->5 |
| collapsed | exact_name | `tisp_ae_calc_convergence_speed` | `tisp_ae_calc_convergence_speed` | 70 | 30 | 0.429 | -40 | 4->2 | 2->0 |
| collapsed | exact_name | `tisp_awb_process` | `tisp_awb_process` | 69 | 10 | 0.145 | -59 | 6->2 | 3->0 |
| collapsed | exact_name | `tisp_s_raw_row_control` | `tisp_s_raw_row_control` | 68 | 30 | 0.441 | -38 | 5->3 | 4->0 |
| collapsed | exact_name | `isp_frame_done_wait` | `isp_frame_done_wait` | 67 | 30 | 0.448 | -37 | 5->1 | 3->2 |
| collapsed | exact_name | `tisp_dpc_pm_suspend` | `tisp_dpc_pm_suspend` | 62 | 17 | 0.274 | -45 | 3->1 | 3->1 |
| collapsed | exact_name | `tisp_ae_lib_bilinear_intp` | `tisp_ae_lib_bilinear_intp` | 62 | 20 | 0.323 | -42 | 1->1 | 10->4 |
| collapsed | exact_name | `tisp_msca_crop_api` | `tisp_msca_crop_api` | 60 | 20 | 0.333 | -40 | 4->3 | 1->0 |
| collapsed | exact_name | `tisp_s_module_control` | `tisp_s_module_control` | 59 | 27 | 0.458 | -32 | 4->2 | 3->1 |
| collapsed | exact_name | `tisp_s_ccm_attr` | `tisp_s_ccm_attr` | 57 | 18 | 0.316 | -39 | 5->1 | 6->3 |
| collapsed | exact_name | `tisp_csc_api_get` | `tisp_csc_api_get` | 55 | 24 | 0.436 | -31 | 2->1 | 7->2 |
| collapsed | exact_name | `tisp_ae_algorithm` | `tisp_ae_algorithm` | 51 | 16 | 0.314 | -35 | 2->2 | 2->0 |
| collapsed | exact_name | `tisp_tmo_api_get_curve` | `tisp_tmo_api_get_curve` | 51 | 24 | 0.471 | -27 | 0->2 | 10->1 |
| collapsed | exact_name | `vic_sensor_ops_ioctl` | `vic_sensor_ops_ioctl` | 50 | 14 | 0.280 | -36 | 0->0 | 8->3 |
| collapsed | exact_name | `tisp_ae_fps_calc_max_exp` | `tisp_ae_fps_calc_max_exp` | 49 | 17 | 0.347 | -32 | 2->1 | 1->0 |
| collapsed | exact_name | `tisp_defog_pm_suspend` | `tisp_defog_pm_suspend` | 47 | 17 | 0.362 | -30 | 2->1 | 2->1 |
| collapsed | exact_name | `tisp_dmsc_pm_suspend` | `tisp_dmsc_pm_suspend` | 47 | 17 | 0.362 | -30 | 2->1 | 2->1 |
| collapsed | exact_name | `ISPAWBInterpolation1` | `ISPAWBInterpolation1` | 45 | 12 | 0.267 | -33 | 4->1 | 2->0 |
| collapsed | exact_name | `tisp_bcsh_refresh_by_csc` | `tisp_bcsh_refresh_by_csc` | 44 | 19 | 0.432 | -25 | 4->4 | 0->0 |
| collapsed | exact_name | `tisp_mdns_param_array_set` | `tisp_mdns_param_array_set` | 39 | 18 | 0.462 | -21 | 5->2 | 0->0 |
| collapsed | exact_name | `tisp_bcsh_wdr_en` | `tisp_bcsh_wdr_en` | 38 | 15 | 0.395 | -23 | 3->3 | 0->0 |
| collapsed | exact_name | `tisp_ccm_wdr_en` | `tisp_ccm_wdr_en` | 38 | 15 | 0.395 | -23 | 4->3 | 0->0 |
| collapsed | exact_name | `tisp_defog_strength_itp` | `tisp_defog_strength_itp` | 36 | 11 | 0.306 | -25 | 0->0 | 4->1 |
| collapsed | exact_name | `tisp_bcsh_ev_update` | `tisp_bcsh_ev_update` | 33 | 15 | 0.455 | -18 | 2->2 | 1->1 |
| collapsed | exact_name | `tiziano_adr_5x5_out` | `tiziano_adr_5x5_out` | 31 | 14 | 0.452 | -17 | 0->0 | 6->4 |
| collapsed | exact_name | `tisp_awb_api_set_ct_trend_offset` | `tisp_awb_api_set_ct_trend_offset` | 29 | 10 | 0.345 | -19 | 0->0 | 1->1 |
| collapsed | exact_name | `tisp_ccm_refresh_by_csc` | `tisp_ccm_refresh_by_csc` | 26 | 10 | 0.385 | -16 | 3->2 | 0->0 |
| collapsed | exact_name | `fix_point_mult3` | `fix_point_mult3` | 25 | 10 | 0.400 | -15 | 1->1 | 0->0 |
| collapsed | exact_name | `fix_point_mult3_64` | `fix_point_mult3_64` | 25 | 10 | 0.400 | -15 | 1->1 | 0->0 |
| collapsed | exact_name | `tisp_awb_api_get_ct_trend_offset` | `tisp_awb_api_get_ct_trend_offset` | 24 | 10 | 0.417 | -14 | 0->0 | 1->1 |
| collapsed | exact_name | `tisp_process_deinit` | `tisp_process_deinit` | 24 | 10 | 0.417 | -14 | 2->1 | 1->0 |
| collapsed | exact_name | `tisp_set_csc_attr` | `tisp_set_csc_attr` | 24 | 10 | 0.417 | -14 | 4->2 | 0->0 |
| collapsed | exact_name | `system_reg_write_af` | `system_reg_write_af` | 24 | 11 | 0.458 | -13 | 1->0 | 1->1 |
| oem_only | oem_only | `isp_info_show.isra.6` |  | 960 | 0 | 0.000 | -960 | 115->0 | 111->0 |
| oem_only | oem_only | `mipi_phy_stream_on.constprop.3` |  | 263 | 0 | 0.000 | -263 | 15->0 | 17->0 |
| oem_only | oem_only | `isp_mbus_to_bayer.isra.1` |  | 261 | 0 | 0.000 | -261 | 2->0 | 98->0 |
| oem_only | oem_only | `tx_isp_set_ae_algo_open.isra.23` |  | 156 | 0 | 0.000 | -156 | 14->0 | 7->0 |
| oem_only | oem_only | `ispcore_set_clk_parent.isra.4` |  | 133 | 0 | 0.000 | -133 | 4->0 | 15->0 |
| oem_only | oem_only | `tx_isp_switch_bin.isra.78` |  | 111 | 0 | 0.000 | -111 | 7->0 | 7->0 |
| oem_only | oem_only | `tx_isp_get_ae_algo_handle.isra.21` |  | 103 | 0 | 0.000 | -103 | 6->0 | 1->0 |
| oem_only | oem_only | `tx_isp_get_default_bin_path.isra.25` |  | 75 | 0 | 0.000 | -75 | 5->0 | 5->0 |
| oem_only | oem_only | `isp_core_tunning_default_ioctl.isra.80` |  | 74 | 0 | 0.000 | -74 | 5->0 | 9->0 |
| oem_only | oem_only | `tx_isp_video_link_destroy.isra.3` |  | 73 | 0 | 0.000 | -73 | 4->0 | 8->0 |
| oem_only | oem_only | `ispcore_frame_channel_s_fmt.isra.2` |  | 60 | 0 | 0.000 | -60 | 1->0 | 3->0 |
| oem_only | oem_only | `tx_isp_set_default_bin_path.isra.16` |  | 57 | 0 | 0.000 | -57 | 4->0 | 4->0 |
| oem_only | oem_only | `tx_isp_ae_expoinfo_g_attr.isra.55` |  | 48 | 0 | 0.000 | -48 | 4->0 | 3->0 |
| oem_only | oem_only | `tx_isp_ae_weight_g_attr.isra.58` |  | 48 | 0 | 0.000 | -48 | 4->0 | 3->0 |
| oem_only | oem_only | `tx_isp_af_weight_g_attr.isra.49` |  | 48 | 0 | 0.000 | -48 | 4->0 | 3->0 |
| oem_only | oem_only | `tx_isp_awb_attr_g_attr.isra.51` |  | 48 | 0 | 0.000 | -48 | 4->0 | 3->0 |
| oem_only | oem_only | `tx_isp_awb_weight_g_attr.isra.50` |  | 48 | 0 | 0.000 | -48 | 4->0 | 3->0 |
| oem_only | oem_only | `tx_isp_gamma_g_attr.isra.42` |  | 48 | 0 | 0.000 | -48 | 4->0 | 3->0 |
| oem_only | oem_only | `tx_isp_gamma_s_attr.isra.9` |  | 48 | 0 | 0.000 | -48 | 4->0 | 3->0 |
| oem_only | oem_only | `tx_isp_module_ratio_s_attr.isra.10` |  | 48 | 0 | 0.000 | -48 | 4->0 | 3->0 |
| oem_only | oem_only | `tx_isp_tmo_s_curve.isra.34` |  | 48 | 0 | 0.000 | -48 | 4->0 | 3->0 |
| oem_only | oem_only | `tx_isp_ae_g_at_list.isra.70` |  | 43 | 0 | 0.000 | -43 | 4->0 | 2->0 |
| oem_only | oem_only | `tx_isp_ae_g_ev_list.isra.69` |  | 43 | 0 | 0.000 | -43 | 4->0 | 2->0 |
| oem_only | oem_only | `tx_isp_ae_g_explist.isra.63` |  | 43 | 0 | 0.000 | -43 | 4->0 | 2->0 |
| oem_only | oem_only | `tx_isp_tmo_g_curve.isra.72` |  | 43 | 0 | 0.000 | -43 | 4->0 | 2->0 |
| oem_only | oem_only | `tx_isp_ae_expoinfo_s_attr.isra.20` |  | 41 | 0 | 0.000 | -41 | 4->0 | 2->0 |
| oem_only | oem_only | `tx_isp_ae_s_explist.isra.24` |  | 37 | 0 | 0.000 | -37 | 3->0 | 2->0 |
| oem_only | oem_only | `tx_isp_ae_weight_s_attr.isra.21` |  | 37 | 0 | 0.000 | -37 | 3->0 | 2->0 |
| oem_only | oem_only | `tx_isp_af_weight_s_attr.isra.16` |  | 37 | 0 | 0.000 | -37 | 3->0 | 2->0 |
| oem_only | oem_only | `tx_isp_awb_weight_s_attr.isra.17` |  | 37 | 0 | 0.000 | -37 | 3->0 | 2->0 |
| oem_only | oem_only | `tx_isp_driver_version.isra.12` |  | 37 | 0 | 0.000 | -37 | 3->0 | 1->0 |
| oem_only | oem_only | `tx_isp_ai_g_ratio_table.isra.75` |  | 33 | 0 | 0.000 | -33 | 3->0 | 1->0 |
| oem_only | oem_only | `tx_isp_module_ratio_g_attr.isra.60` |  | 33 | 0 | 0.000 | -33 | 3->0 | 1->0 |
| oem_only | oem_only | `tisp_lce_clr_ram.part.0` |  | 30 | 0 | 0.000 | -30 | 2->0 | 1->0 |
| oem_only | oem_only | `tisp_wdr_paixu.part.1` |  | 26 | 0 | 0.000 | -26 | 0->0 | 5->0 |
| oem_only | oem_only | `tisp_dpc_write_reg.part.0` |  | 18 | 0 | 0.000 | -18 | 2->0 | 0->0 |
| oem_only | oem_only | `csi_sensor_ops_sync_sensor_attr.part.2` |  | 15 | 0 | 0.000 | -15 | 1->0 | 0->0 |
| oem_only | oem_only | `vic_sensor_ops_sync_sensor_attr.part.4` |  | 15 | 0 | 0.000 | -15 | 1->0 | 0->0 |
| oem_only | oem_only | `lce_compress_data.part.1` |  | 9 | 0 | 0.000 | -9 | 0->0 | 1->0 |
| oem_only | oem_only | `lsc_exchange_data.constprop.1` |  | 9 | 0 | 0.000 | -9 | 0->0 | 0->0 |
| oem_only | oem_only | `tx_isp_module_exit` |  | 4 | 0 | 0.000 | -4 | 0->0 | 1->0 |
| shorter | exact_name | `Tiziano_wdr_fusion_fpga` | `Tiziano_wdr_fusion_fpga` | 2197 | 1373 | 0.625 | -824 | 48->36 | 246->163 |
| shorter | exact_name | `tx_isp_unlocked_ioctl` | `tx_isp_unlocked_ioctl` | 1845 | 1100 | 0.596 | -745 | 102->71 | 288->193 |
| shorter | exact_name | `frame_channel_unlocked_ioctl` | `frame_channel_unlocked_ioctl` | 1076 | 637 | 0.592 | -439 | 61->45 | 169->102 |
| shorter | exact_name | `Tiziano_defog_soft` | `Tiziano_defog_soft` | 755 | 449 | 0.595 | -306 | 9->3 | 55->35 |
| shorter | exact_name | `tisp_init` | `tisp_init` | 680 | 360 | 0.529 | -320 | 87->36 | 39->38 |
| shorter | exact_name | `Tiziano_wdr_deghost_fpga` | `Tiziano_wdr_deghost_fpga` | 679 | 419 | 0.617 | -260 | 40->34 | 49->26 |
| shorter | exact_name | `tisp_lsc_ct_interp` | `tisp_lsc_ct_interp` | 666 | 419 | 0.629 | -247 | 19->6 | 32->33 |
| shorter | exact_name | `isp_vic_cmd_set` | `isp_vic_cmd_set` | 609 | 447 | 0.734 | -162 | 54->46 | 74->52 |
| shorter | exact_name | `isp_vic_interrupt_service_routine` | `isp_vic_interrupt_service_routine` | 555 | 369 | 0.665 | -186 | 33->30 | 49->29 |
| shorter | exact_name | `proc_ivdc_writel` | `proc_ivdc_writel` | 543 | 294 | 0.541 | -249 | 42->25 | 68->31 |
| shorter | exact_name | `subsection_up` | `subsection_up` | 530 | 298 | 0.562 | -232 | 31->31 | 20->17 |
| shorter | exact_name | `tisp_ae_algo_handle` | `tisp_ae_algo_handle` | 504 | 384 | 0.762 | -120 | 49->43 | 12->13 |
| shorter | exact_name | `ispcore_core_ops_init` | `ispcore_core_ops_init` | 487 | 344 | 0.706 | -143 | 25->19 | 43->38 |
| shorter | exact_name | `tisp_af_set_hardware_param` | `tisp_af_set_hardware_param` | 483 | 385 | 0.797 | -98 | 50->48 | 7->7 |
| shorter | exact_name | `isp_save_cmd_set` | `isp_save_cmd_set` | 431 | 253 | 0.587 | -178 | 35->22 | 42->18 |
| shorter | exact_name | `tx_isp_subdev_init` | `tx_isp_subdev_init` | 420 | 306 | 0.729 | -114 | 27->19 | 43->40 |
| shorter | exact_name | `tisp_lce_init` | `tisp_lce_init` | 412 | 246 | 0.597 | -166 | 19->13 | 9->5 |
| shorter | exact_name | `tiziano_adr_5x5_init` | `tiziano_adr_5x5_init` | 394 | 252 | 0.640 | -142 | 37->30 | 24->22 |
| shorter | exact_name | `tisp_defog_init` | `tisp_defog_init` | 391 | 277 | 0.708 | -114 | 34->14 | 11->13 |
| shorter | exact_name | `tisp_lsc_mirror_flip` | `tisp_lsc_mirror_flip` | 386 | 263 | 0.681 | -123 | 41->22 | 26->13 |
| shorter | exact_name | `tisp_ae_weight_mean` | `tisp_ae_weight_mean` | 369 | 247 | 0.669 | -122 | 11->6 | 10->8 |
| shorter | exact_name | `tisp_wdr_direct_reg_refresh` | `tisp_wdr_direct_reg_refresh` | 340 | 260 | 0.765 | -80 | 38->26 | 2->8 |
| shorter | exact_name | `tisp_ae_init` | `tisp_ae_init` | 325 | 185 | 0.569 | -140 | 27->17 | 9->6 |
| shorter | exact_name | `tisp_s_wdr_en` | `tisp_s_wdr_en` | 323 | 177 | 0.548 | -146 | 38->14 | 17->10 |
| shorter | exact_name | `tisp_awb_spec_calculate` | `tisp_awb_spec_calculate` | 320 | 251 | 0.784 | -69 | 33->25 | 30->24 |
| shorter | exact_name | `tisp_msca_set_line` | `tisp_msca_set_line` | 307 | 215 | 0.700 | -92 | 28->21 | 25->15 |
| shorter | exact_name | `tisp_msca_init_chx_cfg` | `tisp_msca_init_chx_cfg` | 295 | 216 | 0.732 | -79 | 17->8 | 8->8 |
| shorter | exact_name | `tisp_day_or_night_event` | `tisp_day_or_night_event` | 294 | 172 | 0.585 | -122 | 28->16 | 15->12 |
| shorter | exact_name | `lce_self_light_correct` | `lce_self_light_correct` | 286 | 184 | 0.643 | -102 | 6->5 | 29->19 |
| shorter | exact_name | `tisp_tstp_reg_cfg` | `tisp_tstp_reg_cfg` | 276 | 171 | 0.620 | -105 | 30->29 | 0->0 |
| shorter | exact_name | `tisp_vic_ctrl_ioctl` | `tisp_vic_ctrl_ioctl` | 268 | 165 | 0.616 | -103 | 15->12 | 39->24 |
| shorter | exact_name | `tisp_af_alogrithm` | `tisp_af_alogrithm` | 256 | 197 | 0.770 | -59 | 8->8 | 15->13 |
| shorter | exact_name | `isp_framesource_show` | `isp_framesource_show` | 249 | 180 | 0.723 | -69 | 24->23 | 21->13 |
| shorter | exact_name | `func_adr_map_curve1` | `func_adr_map_curve1` | 235 | 144 | 0.613 | -91 | 1->1 | 7->7 |
| shorter | exact_name | `tisp_defog_update_reg_para2` | `tisp_defog_update_reg_para2` | 233 | 137 | 0.588 | -96 | 3->0 | 19->18 |
| shorter | exact_name | `lce_hist_method` | `lce_hist_method` | 233 | 168 | 0.721 | -65 | 4->4 | 25->15 |
| shorter | exact_name | `tisp_defog_dn_params_refresh` | `tisp_defog_dn_params_refresh` | 227 | 144 | 0.634 | -83 | 31->25 | 3->3 |
| shorter | exact_name | `tisp_awb_set_gain` | `tisp_awb_set_gain` | 222 | 151 | 0.680 | -71 | 13->11 | 12->10 |
| shorter | exact_name | `tisp_bcsh_interp_by_ct` | `tisp_bcsh_interp_by_ct` | 216 | 143 | 0.662 | -73 | 5->1 | 43->37 |
| shorter | exact_name | `func_gauss_local` | `func_gauss_local` | 215 | 144 | 0.670 | -71 | 10->9 | 13->7 |
| shorter | exact_name | `tisp_gib_ae_write_dgain` | `tisp_gib_ae_write_dgain` | 212 | 114 | 0.538 | -98 | 28->1 | 7->6 |
| shorter | exact_name | `tisp_msca_normalized` | `tisp_msca_normalized` | 210 | 164 | 0.781 | -46 | 5->5 | 20->10 |
| shorter | exact_name | `Tiziano_Awb_Ct_Cal` | `Tiziano_Awb_Ct_Cal` | 206 | 111 | 0.539 | -95 | 4->4 | 22->17 |
| shorter | exact_name | `func_map_y_filter_sp` | `func_map_y_filter_sp` | 205 | 127 | 0.620 | -78 | 4->2 | 8->6 |
| shorter | exact_name | `func_map_y_filter` | `func_map_y_filter` | 201 | 133 | 0.662 | -68 | 1->1 | 9->11 |
| shorter | exact_name | `tisp_ae_set_hardware_param` | `tisp_ae_set_hardware_param` | 200 | 141 | 0.705 | -59 | 9->7 | 5->4 |
| shorter | exact_name | `tisp_ae_get_bv` | `tisp_ae_get_bv` | 189 | 130 | 0.688 | -59 | 8->8 | 9->2 |
| shorter | exact_name | `tiziano_adr_read_data` | `tiziano_adr_read_data` | 188 | 111 | 0.590 | -77 | 0->0 | 3->1 |
| shorter | exact_name | `tisp_awb_ev_update_Ywgt` | `tisp_awb_ev_update_Ywgt` | 186 | 116 | 0.624 | -70 | 0->0 | 28->23 |
| shorter | exact_name | `tx_isp_ivdc_show` | `tx_isp_ivdc_show` | 185 | 131 | 0.708 | -54 | 25->25 | 4->1 |
| shorter | exact_name | `tisp_mdns_reg_cfg_equation_dif` | `tisp_mdns_reg_cfg_equation_dif` | 184 | 135 | 0.734 | -49 | 0->0 | 19->13 |
| shorter | exact_name | `ispcore_frame_channel_set_fmt` | `ispcore_frame_channel_set_fmt` | 180 | 92 | 0.511 | -88 | 7->5 | 26->15 |
| shorter | exact_name | `tisp_tmo_detailen_ev_interp` | `tisp_tmo_detailen_ev_interp` | 180 | 96 | 0.533 | -84 | 0->0 | 9->14 |
| shorter | exact_name | `tisp_ccm_interp_by_ct` | `tisp_ccm_interp_by_ct` | 179 | 124 | 0.693 | -55 | 2->1 | 34->26 |
| shorter | exact_name | `tisp_sdns_ref_reg_cfg` | `tisp_sdns_ref_reg_cfg` | 172 | 134 | 0.779 | -38 | 26->26 | 0->0 |
| shorter | exact_name | `tisp_deinit` | `tisp_deinit` | 169 | 103 | 0.609 | -66 | 31->25 | 5->0 |
| shorter | exact_name | `tisp_clm_write_lut` | `tisp_clm_write_lut` | 167 | 106 | 0.635 | -61 | 18->11 | 2->1 |
| shorter | exact_name | `tisp_lsc_init` | `tisp_lsc_init` | 161 | 123 | 0.764 | -38 | 11->8 | 5->3 |
| shorter | exact_name | `tiziano_adr_stat_calc` | `tiziano_adr_stat_calc` | 159 | 98 | 0.616 | -61 | 0->0 | 3->3 |
| shorter | exact_name | `tisp_tmo_ram_reg_refresh` | `tisp_tmo_ram_reg_refresh` | 157 | 103 | 0.656 | -54 | 16->10 | 4->4 |
| shorter | exact_name | `tisp_cdns_reg_cfg` | `tisp_cdns_reg_cfg` | 156 | 113 | 0.724 | -43 | 11->11 | 3->1 |
| shorter | exact_name | `tisp_tstp_mark1_func` | `tisp_tstp_mark1_func` | 154 | 95 | 0.617 | -59 | 18->17 | 9->7 |
| shorter | exact_name | `tisp_clm_ct_interp` | `tisp_clm_ct_interp` | 154 | 121 | 0.786 | -33 | 6->4 | 19->19 |
| shorter | exact_name | `tisp_suspend_all` | `tisp_suspend_all` | 137 | 85 | 0.620 | -52 | 9->7 | 9->6 |
| shorter | exact_name | `tiziano_adr_base_pars` | `tiziano_adr_base_pars` | 136 | 73 | 0.537 | -63 | 4->2 | 5->4 |
| shorter | exact_name | `tisp_lsc_lut_valid_judge` | `tisp_lsc_lut_valid_judge` | 134 | 105 | 0.784 | -29 | 2->2 | 19->16 |
| shorter | exact_name | `func_adr_reg_write_every` | `func_adr_reg_write_every` | 131 | 98 | 0.748 | -33 | 10->9 | 7->5 |
| shorter | exact_name | `tisp_csccr_api_set` | `tisp_csccr_api_set` | 130 | 97 | 0.746 | -33 | 8->8 | 10->8 |
| shorter | exact_name | `tisp_msca_para_calc` | `tisp_msca_para_calc` | 129 | 102 | 0.791 | -27 | 0->0 | 19->20 |
| shorter | exact_name | `tisp_log2_int_to_fixed_64` | `tisp_log2_int_to_fixed_64` | 128 | 83 | 0.648 | -45 | 3->1 | 15->12 |
| shorter | exact_name | `tisp_s_statis_config_attr` | `tisp_s_statis_config_attr` | 128 | 101 | 0.789 | -27 | 10->10 | 6->6 |
| shorter | exact_name | `tisp_hldc_set_attr` | `tisp_hldc_set_attr` | 127 | 79 | 0.622 | -48 | 6->5 | 1->0 |
| shorter | exact_name | `tisp_bcsh_init` | `tisp_bcsh_init` | 125 | 80 | 0.640 | -45 | 10->7 | 0->0 |
| shorter | exact_name | `frame_channel_vidioc_set_fmt` | `frame_channel_vidioc_set_fmt` | 125 | 97 | 0.776 | -28 | 8->7 | 16->13 |
| shorter | exact_name | `tisp_wdr_deinit` | `tisp_wdr_deinit` | 122 | 82 | 0.672 | -40 | 11->5 | 14->6 |
| shorter | exact_name | `tisp_defog_param_array_set` | `tisp_defog_param_array_set` | 121 | 84 | 0.694 | -37 | 11->10 | 3->3 |
| shorter | exact_name | `tisp_mdns_addr_alloc` | `tisp_mdns_addr_alloc` | 114 | 77 | 0.675 | -37 | 9->7 | 4->2 |
| shorter | exact_name | `tisp_tmo_deinit` | `tisp_tmo_deinit` | 114 | 78 | 0.684 | -36 | 10->4 | 13->6 |
| shorter | exact_name | `tisp_ae_clac_deflicker_cfg` | `tisp_ae_clac_deflicker_cfg` | 114 | 85 | 0.746 | -29 | 3->3 | 7->4 |
| shorter | exact_name | `isp_core_tuning_event` | `isp_core_tuning_event` | 113 | 62 | 0.549 | -51 | 7->5 | 10->5 |
| shorter | exact_name | `tisp_top_write_bypass` | `tisp_top_write_bypass` | 113 | 78 | 0.690 | -35 | 7->5 | 11->10 |
| shorter | exact_name | `tisp_lce_get_data` | `tisp_lce_get_data` | 112 | 68 | 0.607 | -44 | 8->6 | 5->6 |
| shorter | exact_name | `ivdc_misc_unlocked_ioctl` | `ivdc_misc_unlocked_ioctl` | 112 | 81 | 0.723 | -31 | 6->4 | 17->14 |
| shorter | exact_name | `vic_mdma_enable` | `vic_mdma_enable` | 111 | 82 | 0.739 | -29 | 0->0 | 7->3 |
| shorter | exact_name | `sensor_init` | `sensor_init` | 111 | 88 | 0.793 | -23 | 0->0 | 0->0 |
| shorter | exact_name | `ispcore_frame_channel_qbuf` | `ispcore_frame_channel_qbuf` | 110 | 82 | 0.745 | -28 | 5->3 | 9->9 |
| shorter | exact_name | `tisp_msca_set_mask` | `tisp_msca_set_mask` | 109 | 77 | 0.706 | -32 | 8->8 | 4->2 |
| shorter | exact_name | `tisp_ae_par_update_trig` | `tisp_ae_par_update_trig` | 107 | 80 | 0.748 | -27 | 7->6 | 5->3 |
| shorter | exact_name | `tisp_ccm_init` | `tisp_ccm_init` | 107 | 81 | 0.757 | -26 | 9->7 | 0->0 |
| shorter | exact_name | `ispcore_sync_sensor_attr` | `ispcore_sync_sensor_attr` | 106 | 61 | 0.575 | -45 | 5->5 | 7->6 |
| shorter | exact_name | `isp_malloc_buffer` | `isp_malloc_buffer` | 106 | 72 | 0.679 | -34 | 6->3 | 12->10 |
| shorter | exact_name | `tisp_ae_api_get_scene_luma` | `tisp_ae_api_get_scene_luma` | 105 | 60 | 0.571 | -45 | 4->3 | 5->1 |
| shorter | exact_name | `tisp_lsc_param_array_set` | `tisp_lsc_param_array_set` | 103 | 53 | 0.515 | -50 | 8->5 | 5->2 |
| shorter | exact_name | `isp_vic_frd_show` | `isp_vic_frd_show` | 103 | 61 | 0.592 | -42 | 4->4 | 6->6 |
| shorter | exact_name | `tisp_tmo_detailen_gain_interp` | `tisp_tmo_detailen_gain_interp` | 97 | 55 | 0.567 | -42 | 14->2 | 0->6 |
| shorter | exact_name | `frame_channel_vidioc_get_fmt` | `frame_channel_vidioc_get_fmt` | 97 | 59 | 0.608 | -38 | 7->5 | 9->7 |
| shorter | exact_name | `tisp_gib_interp_by_again` | `tisp_gib_interp_by_again` | 93 | 59 | 0.634 | -34 | 6->1 | 4->9 |
| shorter | exact_name | `tisp_s_module_ratio_attr` | `tisp_s_module_ratio_attr` | 84 | 44 | 0.524 | -40 | 3->2 | 15->8 |
| shorter | exact_name | `tisp_s_raw_rw_control` | `tisp_s_raw_rw_control` | 84 | 53 | 0.631 | -31 | 5->5 | 8->1 |
| shorter | exact_name | `tisp_bypass_update` | `tisp_bypass_update` | 83 | 56 | 0.675 | -27 | 5->5 | 0->0 |
| shorter | exact_name | `tisp_clm_init` | `tisp_clm_init` | 83 | 59 | 0.711 | -24 | 7->6 | 0->0 |
| shorter | exact_name | `tisp_mdns_func_en` | `tisp_mdns_func_en` | 82 | 46 | 0.561 | -36 | 2->1 | 3->3 |
| shorter | exact_name | `ispcore_frame_channel_ir_qbuf` | `ispcore_frame_channel_ir_qbuf` | 79 | 61 | 0.772 | -18 | 4->3 | 12->11 |
| shorter | exact_name | `tisp_ae_sepc_area` | `tisp_ae_sepc_area` | 78 | 46 | 0.590 | -32 | 1->1 | 4->4 |
| shorter | exact_name | `tisp_lce_process` | `tisp_lce_process` | 76 | 54 | 0.711 | -22 | 6->5 | 2->2 |
