# Binary Assembly Audit

- schema: `regtrace-binary-audit-v1`
- OEM: `/home/matteius/re-framework/tx-isp-t41.ko`
- recovered: `driver/t41/tx_isp_t41_recovered.ko`
- objdump counts exclude relocation records
- thresholds: min_oem_insns=24 stub_insns=8 collapse=0.50 similar=0.80..1.25 expansion=2.00

## Summary

| Metric | OEM | Recovered |
|---|---:|---:|
| Function symbols | 1314 | 1517 |
| Functions with disassembly | 1314 | 1517 |
| Executable section bytes | 479556 | 333296 |
| Initialized writable bytes | 25120 | 15808 |
| Uninitialized writable bytes | 20592 | 383104 |

- direct matches: 1273
- replacement matches: 0 (missing=0)
- unmatched: OEM-only=41 recovered-only=244
- matched instructions: OEM=116036 recovered=80879 ratio=0.697
- classes: stub=31 collapsed=91 shorter=398 same_count=245 similar=468 larger=22 expanded=18

## Allocated Section Delta

| Section | OEM bytes | Recovered bytes | Delta |
|---|---:|---:|---:|
| `.MIPS.abiflags` | 24 | 24 | +0 |
| `.bss` | 20592 | 383104 | +362512 |
| `.data` | 24768 | 15456 | -9312 |
| `.exit.text` | 1140 | 0 | -1140 |
| `.gnu.linkonce.this_module` | 352 | 352 | +0 |
| `.init.text` | 16 | 0 | -16 |
| `.modinfo` | 1012 | 1136 | +124 |
| `.note.gnu.build-id` | 36 | 36 | +0 |
| `.reginfo` | 24 | 24 | +0 |
| `.rodata` | 15760 | 3488 | -12272 |
| `.rodata.str1.4` | 25860 | 4516 | -21344 |
| `.text` | 478400 | 333296 | -145104 |
| `__ksymtab` | 248 | 248 | +0 |
| `__ksymtab_strings` | 637 | 637 | +0 |
| `__param` | 260 | 280 | +20 |

## Function Outliers

Showing 500 of 845 outliers. JSON and CSV contain every comparison row.

| Class | Match | OEM symbol | Recovered symbol(s) | OEM insns | Recovered insns | Ratio | Delta | Calls | Branches |
|---|---|---|---|---:|---:|---:|---:|---:|---:|
| stub | exact_name | `tisp_bcsh_BCS_adjust` | `tisp_bcsh_BCS_adjust` | 355 | 2 | 0.006 | -353 | 17->0 | 28->1 |
| stub | exact_name | `ispcore_core_ops_ioctl` | `ispcore_core_ops_ioctl` | 309 | 2 | 0.006 | -307 | 8->0 | 55->0 |
| stub | exact_name | `ispint_adr_64` | `ispint_adr_64` | 101 | 3 | 0.030 | -98 | 2->0 | 14->0 |
| stub | exact_name | `func_interp1_short` | `func_interp1_short` | 67 | 2 | 0.030 | -65 | 0->0 | 10->0 |
| stub | exact_name | `isp_frame_done_wait` | `isp_frame_done_wait` | 67 | 7 | 0.104 | -60 | 5->0 | 3->0 |
| stub | exact_name | `private_wait_event_interruptible_timeout` | `private_wait_event_interruptible_timeout` | 49 | 2 | 0.041 | -47 | 3->0 | 6->0 |
| stub | exact_name | `private_reset_tx_isp_module` | `private_reset_tx_isp_module` | 49 | 8 | 0.163 | -41 | 1->0 | 4->2 |
| stub | exact_name | `ispcore_pad_event_handle` | `ispcore_pad_event_handle` | 48 | 2 | 0.042 | -46 | 0->0 | 9->0 |
| stub | exact_name | `mbus_to_bayer_write` | `mbus_to_bayer_write` | 46 | 4 | 0.087 | -42 | 3->0 | 4->0 |
| stub | exact_name | `func_zone_ct_weight` | `func_zone_ct_weight` | 42 | 2 | 0.048 | -40 | 0->0 | 6->1 |
| stub | exact_name | `ISPAWBInterpolation2` | `ISPAWBInterpolation2` | 42 | 4 | 0.095 | -38 | 4->0 | 2->1 |
| stub | exact_name | `private_log2_int_to_fixed` | `private_log2_int_to_fixed` | 42 | 6 | 0.143 | -36 | 1->0 | 6->2 |
| stub | exact_name | `tisp_irsca_para_calc` | `tisp_irsca_para_calc` | 41 | 8 | 0.195 | -33 | 0->0 | 4->1 |
| stub | exact_name | `private_leading_one_position_64` | `private_leading_one_position_64` | 38 | 2 | 0.053 | -36 | 0->0 | 7->0 |
| stub | exact_name | `tisp_ae_max_exp_calc_fps` | `tisp_ae_max_exp_calc_fps` | 38 | 2 | 0.053 | -36 | 1->0 | 0->0 |
| stub | exact_name | `tisp_bcsh_dn_params_refresh` | `tisp_bcsh_dn_params_refresh` | 38 | 6 | 0.158 | -32 | 3->1 | 0->0 |
| stub | exact_name | `tisp_ccm_dn_params_refresh` | `tisp_ccm_dn_params_refresh` | 38 | 6 | 0.158 | -32 | 4->1 | 0->0 |
| stub | exact_name | `fix_point_sub` | `fix_point_sub` | 37 | 2 | 0.054 | -35 | 1->0 | 3->0 |
| stub | exact_name | `fix_point_sub_64` | `fix_point_sub_64` | 37 | 2 | 0.054 | -35 | 1->0 | 3->0 |
| stub | exact_name | `tisp_mdns_dn_params_refresh` | `tisp_mdns_dn_params_refresh` | 37 | 6 | 0.162 | -31 | 4->1 | 0->0 |
| stub | exact_name | `tisp_hldc_quadratic_func` | `tisp_hldc_quadratic_func` | 35 | 3 | 0.086 | -32 | 0->0 | 0->0 |
| stub | exact_name | `tisp_g_awb_attr` | `tisp_g_awb_attr` | 35 | 6 | 0.171 | -29 | 2->1 | 0->0 |
| stub | exact_name | `tisp_hldc_para_validity_judge` | `tisp_hldc_para_validity_judge` | 34 | 2 | 0.059 | -32 | 0->0 | 4->0 |
| stub | exact_name | `tisp_gamma_dn_params_refresh` | `tisp_gamma_dn_params_refresh` | 33 | 4 | 0.121 | -29 | 4->0 | 0->1 |
| stub | exact_name | `tisp_gib_calc_self_gain` | `tisp_gib_calc_self_gain` | 32 | 2 | 0.062 | -30 | 0->0 | 2->0 |
| stub | exact_name | `tx_isp_enable_irq` | `tx_isp_enable_irq` | 32 | 2 | 0.062 | -30 | 3->0 | 2->0 |
| stub | exact_name | `tisp_set_sensor_short_analog_gain` | `tisp_set_sensor_short_analog_gain` | 32 | 4 | 0.125 | -28 | 4->0 | 0->1 |
| stub | exact_name | `private_leading_one_position` | `private_leading_one_position` | 31 | 2 | 0.065 | -29 | 0->0 | 5->0 |
| stub | exact_name | `tx_isp_disable_irq` | `tx_isp_disable_irq` | 31 | 2 | 0.065 | -29 | 3->0 | 2->0 |
| stub | exact_name | `tisp_g_hv_flip` | `tisp_g_hv_flip` | 27 | 2 | 0.074 | -25 | 0->0 | 0->0 |
| stub | exact_name | `private_copy_from_user` | `private_copy_from_user` | 27 | 4 | 0.148 | -23 | 2->0 | 2->1 |
| collapsed | exact_name | `tisp_awb_ct_detect` | `tisp_awb_ct_detect` | 3100 | 789 | 0.255 | -2311 | 41->16 | 362->119 |
| collapsed | exact_name | `tx_isp_core_ops_s_ctrl` | `tx_isp_core_ops_s_ctrl` | 1768 | 261 | 0.148 | -1507 | 150->23 | 226->40 |
| collapsed | exact_name | `tx_isp_core_ops_g_ctrl` | `tx_isp_core_ops_g_ctrl` | 1386 | 624 | 0.450 | -762 | 100->47 | 201->125 |
| collapsed | exact_name | `tisp_ae_ev_list_alloc_calc` | `tisp_ae_ev_list_alloc_calc` | 1332 | 401 | 0.301 | -931 | 67->18 | 201->65 |
| collapsed | exact_name | `tisp_mdns_reg_cfg` | `tisp_mdns_reg_cfg` | 1308 | 305 | 0.233 | -1003 | 102->43 | 4->0 |
| collapsed | exact_name | `Tiziano_adr_fpga` | `Tiziano_adr_fpga` | 1152 | 366 | 0.318 | -786 | 18->9 | 115->47 |
| collapsed | exact_name | `tisp_ae_tune` | `tisp_ae_tune` | 932 | 46 | 0.049 | -886 | 40->4 | 82->1 |
| collapsed | exact_name | `tisp_ae_ev_alloc_calc` | `tisp_ae_ev_alloc_calc` | 931 | 325 | 0.349 | -606 | 50->17 | 126->53 |
| collapsed | exact_name | `Tisp_lce_soft` | `Tisp_lce_soft` | 915 | 65 | 0.071 | -850 | 39->7 | 83->3 |
| collapsed | exact_name | `tisp_awb_long_alogrithm` | `tisp_awb_long_alogrithm` | 863 | 382 | 0.443 | -481 | 14->11 | 80->41 |
| collapsed | exact_name | `tisp_code_tuning_ioctl` | `tisp_code_tuning_ioctl` | 780 | 76 | 0.097 | -704 | 41->6 | 119->10 |
| collapsed | exact_name | `tisp_tmo_fpga` | `tisp_tmo_fpga` | 776 | 34 | 0.044 | -742 | 8->2 | 68->2 |
| collapsed | exact_name | `tisp_init` | `tisp_init` | 680 | 259 | 0.381 | -421 | 87->24 | 39->17 |
| collapsed | exact_name | `tisp_ae_short_ev_alloc_calc` | `tisp_ae_short_ev_alloc_calc` | 672 | 127 | 0.189 | -545 | 40->3 | 102->17 |
| collapsed | exact_name | `subdev_sensor_ops_ioctl` | `subdev_sensor_ops_ioctl` | 498 | 42 | 0.084 | -456 | 29->2 | 78->7 |
| collapsed | exact_name | `tiziano_adr_ev_func` | `tiziano_adr_ev_func` | 492 | 204 | 0.415 | -288 | 45->0 | 12->11 |
| collapsed | exact_name | `Tiziano_Awb_Ct_Detect_GrayWorld_mode` | `Tiziano_Awb_Ct_Detect_GrayWorld_mode` | 429 | 110 | 0.256 | -319 | 12->1 | 35->9 |
| collapsed | exact_name | `tx_isp_subdev_init` | `tx_isp_subdev_init` | 420 | 65 | 0.155 | -355 | 27->4 | 43->6 |
| collapsed | exact_name | `tisp_wdr_get_data` | `tisp_wdr_get_data` | 383 | 123 | 0.321 | -260 | 20->10 | 11->5 |
| collapsed | exact_name | `tisp_ae_fliker_detect` | `tisp_ae_fliker_detect` | 366 | 154 | 0.421 | -212 | 18->16 | 31->5 |
| collapsed | exact_name | `tisp_awb_init` | `tisp_awb_init` | 304 | 106 | 0.349 | -198 | 25->12 | 6->3 |
| collapsed | exact_name | `tisp_day_or_night_event` | `tisp_day_or_night_event` | 294 | 143 | 0.486 | -151 | 28->10 | 15->12 |
| collapsed | exact_name | `tisp_bcsh_H_adjust` | `tisp_bcsh_H_adjust` | 284 | 61 | 0.215 | -223 | 9->6 | 15->3 |
| collapsed | exact_name | `tisp_tmo_init` | `tisp_tmo_init` | 268 | 120 | 0.448 | -148 | 36->13 | 3->2 |
| collapsed | exact_name | `tisp_ae_short_expt` | `tisp_ae_short_expt` | 241 | 105 | 0.436 | -136 | 12->7 | 24->8 |
| collapsed | exact_name | `tisp_ae_par_calc` | `tisp_ae_par_calc` | 239 | 90 | 0.377 | -149 | 8->6 | 23->1 |
| collapsed | exact_name | `vic_core_ops_ioctl` | `vic_core_ops_ioctl` | 192 | 29 | 0.151 | -163 | 7->1 | 33->5 |
| collapsed | exact_name | `tisp_g_module_attr` | `tisp_g_module_attr` | 189 | 44 | 0.233 | -145 | 4->2 | 29->3 |
| collapsed | exact_name | `tisp_s_module_attr` | `tisp_s_module_attr` | 189 | 51 | 0.270 | -138 | 4->3 | 29->4 |
| collapsed | exact_name | `tisp_ae_get_bv` | `tisp_ae_get_bv` | 189 | 91 | 0.481 | -98 | 8->5 | 9->2 |
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
| collapsed | exact_name | `tisp_dpc_pm_suspend` | `tisp_dpc_pm_suspend` | 62 | 17 | 0.274 | -45 | 3->1 | 3->1 |
| collapsed | exact_name | `tisp_ae_lib_bilinear_intp` | `tisp_ae_lib_bilinear_intp` | 62 | 20 | 0.323 | -42 | 1->1 | 10->4 |
| collapsed | exact_name | `tisp_msca_crop_api` | `tisp_msca_crop_api` | 60 | 20 | 0.333 | -40 | 4->3 | 1->0 |
| collapsed | exact_name | `tisp_tmo_params_fristframe_reg_refresh` | `tisp_tmo_params_fristframe_reg_refresh` | 60 | 30 | 0.500 | -30 | 7->3 | 1->1 |
| collapsed | exact_name | `tisp_s_module_control` | `tisp_s_module_control` | 59 | 27 | 0.458 | -32 | 4->2 | 3->1 |
| collapsed | exact_name | `tisp_awb_params_refresh` | `tisp_awb_params_refresh` | 58 | 25 | 0.431 | -33 | 0->0 | 6->5 |
| collapsed | exact_name | `tisp_s_ccm_attr` | `tisp_s_ccm_attr` | 57 | 18 | 0.316 | -39 | 5->1 | 6->3 |
| collapsed | exact_name | `tisp_csc_api_get` | `tisp_csc_api_get` | 55 | 24 | 0.436 | -31 | 2->1 | 7->2 |
| collapsed | exact_name | `tisp_awb_param_array_set` | `tisp_awb_param_array_set` | 54 | 25 | 0.463 | -29 | 5->2 | 1->0 |
| collapsed | exact_name | `tisp_ae_algorithm` | `tisp_ae_algorithm` | 51 | 16 | 0.314 | -35 | 2->2 | 2->0 |
| collapsed | exact_name | `tisp_tmo_api_get_curve` | `tisp_tmo_api_get_curve` | 51 | 24 | 0.471 | -27 | 0->2 | 10->1 |
| collapsed | exact_name | `vic_sensor_ops_ioctl` | `vic_sensor_ops_ioctl` | 50 | 14 | 0.280 | -36 | 0->0 | 8->3 |
| collapsed | exact_name | `tisp_ae_fps_calc_max_exp` | `tisp_ae_fps_calc_max_exp` | 49 | 17 | 0.347 | -32 | 2->1 | 1->0 |
| collapsed | exact_name | `tisp_defog_pm_suspend` | `tisp_defog_pm_suspend` | 47 | 17 | 0.362 | -30 | 2->1 | 2->1 |
| collapsed | exact_name | `tisp_dmsc_pm_suspend` | `tisp_dmsc_pm_suspend` | 47 | 17 | 0.362 | -30 | 2->1 | 2->1 |
| collapsed | exact_name | `ISPAWBInterpolation1` | `ISPAWBInterpolation1` | 45 | 12 | 0.267 | -33 | 4->1 | 2->0 |
| collapsed | exact_name | `tisp_bcsh_refresh_by_csc` | `tisp_bcsh_refresh_by_csc` | 44 | 19 | 0.432 | -25 | 4->4 | 0->0 |
| collapsed | exact_name | `tisp_mdns_param_array_set` | `tisp_mdns_param_array_set` | 39 | 18 | 0.462 | -21 | 5->2 | 0->0 |
| collapsed | exact_name | `isp_subdev_release_clks` | `isp_subdev_release_clks` | 38 | 14 | 0.368 | -24 | 2->1 | 3->2 |
| collapsed | exact_name | `tisp_bcsh_wdr_en` | `tisp_bcsh_wdr_en` | 38 | 15 | 0.395 | -23 | 3->3 | 0->0 |
| collapsed | exact_name | `tisp_ccm_wdr_en` | `tisp_ccm_wdr_en` | 38 | 15 | 0.395 | -23 | 4->3 | 0->0 |
| collapsed | exact_name | `tisp_defog_strength_itp` | `tisp_defog_strength_itp` | 36 | 11 | 0.306 | -25 | 0->0 | 4->1 |
| collapsed | exact_name | `tisp_bcsh_ev_update` | `tisp_bcsh_ev_update` | 33 | 15 | 0.455 | -18 | 2->2 | 1->1 |
| collapsed | exact_name | `tiziano_adr_5x5_out` | `tiziano_adr_5x5_out` | 31 | 14 | 0.452 | -17 | 0->0 | 6->4 |
| collapsed | exact_name | `tisp_awb_api_set_ct_trend_offset` | `tisp_awb_api_set_ct_trend_offset` | 29 | 10 | 0.345 | -19 | 0->0 | 1->1 |
| collapsed | exact_name | `tisp_ccm_refresh_by_csc` | `tisp_ccm_refresh_by_csc` | 26 | 10 | 0.385 | -16 | 3->2 | 0->0 |
| collapsed | exact_name | `tisp_wdr_frame_out` | `tisp_wdr_frame_out` | 25 | 9 | 0.360 | -16 | 1->0 | 0->0 |
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
| shorter | exact_name | `Tiziano_wdr_fusion_fpga` | `Tiziano_wdr_fusion_fpga` | 2197 | 1355 | 0.617 | -842 | 48->33 | 246->163 |
| shorter | exact_name | `tx_isp_unlocked_ioctl` | `tx_isp_unlocked_ioctl` | 1845 | 1124 | 0.609 | -721 | 102->74 | 288->193 |
| shorter | exact_name | `frame_channel_unlocked_ioctl` | `frame_channel_unlocked_ioctl` | 1076 | 637 | 0.592 | -439 | 61->46 | 169->102 |
| shorter | exact_name | `Tiziano_defog_soft` | `Tiziano_defog_soft` | 755 | 449 | 0.595 | -306 | 9->3 | 55->35 |
| shorter | exact_name | `Tiziano_wdr_deghost_fpga` | `Tiziano_wdr_deghost_fpga` | 679 | 419 | 0.617 | -260 | 40->34 | 49->26 |
| shorter | exact_name | `tisp_lsc_ct_interp` | `tisp_lsc_ct_interp` | 666 | 362 | 0.544 | -304 | 19->5 | 32->33 |
| shorter | exact_name | `isp_vic_cmd_set` | `isp_vic_cmd_set` | 609 | 446 | 0.732 | -163 | 54->46 | 74->52 |
| shorter | exact_name | `isp_vic_interrupt_service_routine` | `isp_vic_interrupt_service_routine` | 555 | 370 | 0.667 | -185 | 33->30 | 49->29 |
| shorter | exact_name | `proc_ivdc_writel` | `proc_ivdc_writel` | 543 | 293 | 0.540 | -250 | 42->25 | 68->31 |
| shorter | exact_name | `subsection_up` | `subsection_up` | 530 | 298 | 0.562 | -232 | 31->31 | 20->17 |
| shorter | exact_name | `tisp_ae_algo_handle` | `tisp_ae_algo_handle` | 504 | 284 | 0.563 | -220 | 49->29 | 12->11 |
| shorter | exact_name | `ispcore_core_ops_init` | `ispcore_core_ops_init` | 487 | 293 | 0.602 | -194 | 25->18 | 43->30 |
| shorter | exact_name | `tisp_af_set_hardware_param` | `tisp_af_set_hardware_param` | 483 | 385 | 0.797 | -98 | 50->48 | 7->7 |
| shorter | exact_name | `tiziano_load_parameters` | `tiziano_load_parameters` | 440 | 301 | 0.684 | -139 | 40->33 | 27->21 |
| shorter | exact_name | `isp_save_cmd_set` | `isp_save_cmd_set` | 431 | 252 | 0.585 | -179 | 35->22 | 42->18 |
| shorter | exact_name | `tisp_lce_init` | `tisp_lce_init` | 412 | 246 | 0.597 | -166 | 19->13 | 9->5 |
| shorter | exact_name | `tiziano_adr_5x5_init` | `tiziano_adr_5x5_init` | 394 | 251 | 0.637 | -143 | 37->30 | 24->22 |
| shorter | exact_name | `tisp_defog_init` | `tisp_defog_init` | 391 | 276 | 0.706 | -115 | 34->14 | 11->13 |
| shorter | exact_name | `tisp_lsc_mirror_flip` | `tisp_lsc_mirror_flip` | 386 | 263 | 0.681 | -123 | 41->22 | 26->13 |
| shorter | exact_name | `tisp_ae_weight_mean` | `tisp_ae_weight_mean` | 369 | 247 | 0.669 | -122 | 11->6 | 10->8 |
| shorter | exact_name | `tisp_ae_init` | `tisp_ae_init` | 325 | 185 | 0.569 | -140 | 27->17 | 9->6 |
| shorter | exact_name | `tisp_s_wdr_en` | `tisp_s_wdr_en` | 323 | 177 | 0.548 | -146 | 38->14 | 17->10 |
| shorter | exact_name | `tisp_awb_spec_calculate` | `tisp_awb_spec_calculate` | 320 | 251 | 0.784 | -69 | 33->25 | 30->24 |
| shorter | exact_name | `tisp_msca_set_line` | `tisp_msca_set_line` | 307 | 215 | 0.700 | -92 | 28->21 | 25->15 |
| shorter | exact_name | `tisp_msca_init_chx_cfg` | `tisp_msca_init_chx_cfg` | 295 | 216 | 0.732 | -79 | 17->8 | 8->8 |
| shorter | exact_name | `lce_self_light_correct` | `lce_self_light_correct` | 286 | 184 | 0.643 | -102 | 6->5 | 29->19 |
| shorter | exact_name | `tisp_tstp_reg_cfg` | `tisp_tstp_reg_cfg` | 276 | 171 | 0.620 | -105 | 30->29 | 0->0 |
| shorter | exact_name | `tisp_vic_ctrl_ioctl` | `tisp_vic_ctrl_ioctl` | 268 | 162 | 0.604 | -106 | 15->12 | 39->24 |
| shorter | exact_name | `tisp_ccm_matrix_trans_by_sat` | `tisp_ccm_matrix_trans_by_sat` | 268 | 184 | 0.687 | -84 | 20->19 | 14->7 |
| shorter | exact_name | `tisp_af_alogrithm` | `tisp_af_alogrithm` | 256 | 197 | 0.770 | -59 | 8->8 | 15->13 |
| shorter | exact_name | `isp_framesource_show` | `isp_framesource_show` | 249 | 180 | 0.723 | -69 | 24->23 | 21->13 |
| shorter | exact_name | `func_adr_map_curve1` | `func_adr_map_curve1` | 235 | 144 | 0.613 | -91 | 1->1 | 7->7 |
| shorter | exact_name | `tisp_defog_update_reg_para2` | `tisp_defog_update_reg_para2` | 233 | 137 | 0.588 | -96 | 3->0 | 19->18 |
| shorter | exact_name | `lce_hist_method` | `lce_hist_method` | 233 | 168 | 0.721 | -65 | 4->4 | 25->15 |
| shorter | exact_name | `tisp_defog_dn_params_refresh` | `tisp_defog_dn_params_refresh` | 227 | 144 | 0.634 | -83 | 31->25 | 3->3 |
| shorter | exact_name | `tisp_awb_set_gain` | `tisp_awb_set_gain` | 222 | 151 | 0.680 | -71 | 13->11 | 12->10 |
| shorter | exact_name | `tisp_bcsh_interp_by_ct` | `tisp_bcsh_interp_by_ct` | 216 | 154 | 0.713 | -62 | 5->2 | 43->35 |
| shorter | exact_name | `func_gauss_local` | `func_gauss_local` | 215 | 144 | 0.670 | -71 | 10->9 | 13->7 |
| shorter | exact_name | `tisp_ae_long_ev_alloc` | `tisp_ae_long_ev_alloc` | 214 | 164 | 0.766 | -50 | 12->9 | 8->7 |
| shorter | exact_name | `tisp_gib_ae_write_dgain` | `tisp_gib_ae_write_dgain` | 212 | 118 | 0.557 | -94 | 28->12 | 7->5 |
| shorter | exact_name | `tisp_tmo_ev_interp` | `tisp_tmo_ev_interp` | 211 | 106 | 0.502 | -105 | 2->2 | 42->18 |
| shorter | exact_name | `tisp_msca_normalized` | `tisp_msca_normalized` | 210 | 165 | 0.786 | -45 | 5->5 | 20->10 |
| shorter | exact_name | `Tiziano_Awb_Ct_Cal` | `Tiziano_Awb_Ct_Cal` | 206 | 111 | 0.539 | -95 | 4->4 | 22->17 |
| shorter | exact_name | `func_map_y_filter_sp` | `func_map_y_filter_sp` | 205 | 127 | 0.620 | -78 | 4->2 | 8->6 |
| shorter | exact_name | `func_map_y_filter` | `func_map_y_filter` | 201 | 133 | 0.662 | -68 | 1->1 | 9->11 |
| shorter | exact_name | `tisp_ae_set_hardware_param` | `tisp_ae_set_hardware_param` | 200 | 141 | 0.705 | -59 | 9->7 | 5->4 |
| shorter | exact_name | `tisp_clm_write_csc_para` | `tisp_clm_write_csc_para` | 198 | 107 | 0.540 | -91 | 16->15 | 0->0 |
| shorter | exact_name | `tiziano_adr_read_data` | `tiziano_adr_read_data` | 188 | 111 | 0.590 | -77 | 0->0 | 3->1 |
| shorter | exact_name | `tisp_awb_ev_update_Ywgt` | `tisp_awb_ev_update_Ywgt` | 186 | 116 | 0.624 | -70 | 0->0 | 28->23 |
| shorter | exact_name | `tx_isp_ivdc_show` | `tx_isp_ivdc_show` | 185 | 131 | 0.708 | -54 | 25->25 | 4->1 |
| shorter | exact_name | `tisp_blc_ae_write_dgain` | `tisp_blc_ae_write_dgain` | 184 | 105 | 0.571 | -79 | 22->12 | 6->5 |
| shorter | exact_name | `tisp_mdns_reg_cfg_equation_dif` | `tisp_mdns_reg_cfg_equation_dif` | 184 | 135 | 0.734 | -49 | 0->0 | 19->13 |
| shorter | exact_name | `ispcore_frame_channel_set_fmt` | `ispcore_frame_channel_set_fmt` | 180 | 92 | 0.511 | -88 | 7->5 | 26->15 |
| shorter | exact_name | `tisp_ae_short_ev_alloc` | `tisp_ae_short_ev_alloc` | 180 | 139 | 0.772 | -41 | 10->8 | 8->6 |
| shorter | exact_name | `tisp_ccm_interp_by_ct` | `tisp_ccm_interp_by_ct` | 179 | 143 | 0.799 | -36 | 2->2 | 34->28 |
| shorter | exact_name | `tisp_sdns_ref_reg_cfg` | `tisp_sdns_ref_reg_cfg` | 172 | 134 | 0.779 | -38 | 26->26 | 0->0 |
| shorter | exact_name | `tisp_deinit` | `tisp_deinit` | 169 | 102 | 0.604 | -67 | 31->25 | 5->0 |
| shorter | exact_name | `tisp_clm_write_lut` | `tisp_clm_write_lut` | 167 | 106 | 0.635 | -61 | 18->11 | 2->1 |
| shorter | exact_name | `tisp_lsc_init` | `tisp_lsc_init` | 161 | 123 | 0.764 | -38 | 11->8 | 5->3 |
| shorter | exact_name | `tiziano_adr_stat_calc` | `tiziano_adr_stat_calc` | 159 | 98 | 0.616 | -61 | 0->0 | 3->3 |
| shorter | exact_name | `tisp_tmo_ram_reg_refresh` | `tisp_tmo_ram_reg_refresh` | 157 | 103 | 0.656 | -54 | 16->10 | 4->4 |
| shorter | exact_name | `tisp_cdns_reg_cfg` | `tisp_cdns_reg_cfg` | 156 | 113 | 0.724 | -43 | 11->11 | 3->1 |
| shorter | exact_name | `tisp_tstp_mark1_func` | `tisp_tstp_mark1_func` | 154 | 95 | 0.617 | -59 | 18->17 | 9->7 |
| shorter | exact_name | `tisp_suspend_all` | `tisp_suspend_all` | 137 | 84 | 0.613 | -53 | 9->7 | 9->6 |
| shorter | exact_name | `tiziano_adr_base_pars` | `tiziano_adr_base_pars` | 136 | 73 | 0.537 | -63 | 4->2 | 5->4 |
| shorter | exact_name | `tisp_lsc_lut_valid_judge` | `tisp_lsc_lut_valid_judge` | 134 | 105 | 0.784 | -29 | 2->2 | 19->16 |
| shorter | exact_name | `func_adr_reg_write_every` | `func_adr_reg_write_every` | 131 | 98 | 0.748 | -33 | 10->9 | 7->5 |
| shorter | exact_name | `tisp_csccr_api_set` | `tisp_csccr_api_set` | 130 | 97 | 0.746 | -33 | 8->8 | 10->8 |
| shorter | exact_name | `tisp_msca_para_calc` | `tisp_msca_para_calc` | 129 | 102 | 0.791 | -27 | 0->0 | 19->20 |
| shorter | exact_name | `tisp_s_statis_config_attr` | `tisp_s_statis_config_attr` | 128 | 101 | 0.789 | -27 | 10->10 | 6->6 |
| shorter | exact_name | `tisp_hldc_set_attr` | `tisp_hldc_set_attr` | 127 | 79 | 0.622 | -48 | 6->5 | 1->0 |
| shorter | exact_name | `tisp_bcsh_init` | `tisp_bcsh_init` | 125 | 80 | 0.640 | -45 | 10->7 | 0->0 |
| shorter | exact_name | `frame_channel_vidioc_set_fmt` | `frame_channel_vidioc_set_fmt` | 125 | 97 | 0.776 | -28 | 8->7 | 16->13 |
| shorter | exact_name | `tisp_defog_param_array_set` | `tisp_defog_param_array_set` | 121 | 84 | 0.694 | -37 | 11->10 | 3->3 |
| shorter | exact_name | `tisp_ae_clac_deflicker_cfg` | `tisp_ae_clac_deflicker_cfg` | 114 | 65 | 0.570 | -49 | 3->2 | 7->4 |
| shorter | exact_name | `tisp_mdns_addr_alloc` | `tisp_mdns_addr_alloc` | 114 | 77 | 0.675 | -37 | 9->7 | 4->2 |
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
| shorter | exact_name | `isp_malloc_buffer` | `isp_malloc_buffer` | 106 | 66 | 0.623 | -40 | 6->5 | 12->8 |
| shorter | exact_name | `tisp_ae_api_get_scene_luma` | `tisp_ae_api_get_scene_luma` | 105 | 60 | 0.571 | -45 | 4->3 | 5->1 |
| shorter | exact_name | `tx_isp_request_irq` | `tx_isp_request_irq` | 105 | 81 | 0.771 | -24 | 6->5 | 7->5 |
| shorter | exact_name | `tisp_lsc_param_array_set` | `tisp_lsc_param_array_set` | 103 | 53 | 0.515 | -50 | 8->5 | 5->2 |
| shorter | exact_name | `isp_vic_frd_show` | `isp_vic_frd_show` | 103 | 61 | 0.592 | -42 | 4->4 | 6->6 |
| shorter | exact_name | `frame_channel_vidioc_get_fmt` | `frame_channel_vidioc_get_fmt` | 97 | 59 | 0.608 | -38 | 7->5 | 9->7 |
| shorter | exact_name | `tisp_gib_interp_by_again` | `tisp_gib_interp_by_again` | 93 | 69 | 0.742 | -24 | 6->5 | 4->4 |
| shorter | exact_name | `tisp_s_module_ratio_attr` | `tisp_s_module_ratio_attr` | 84 | 44 | 0.524 | -40 | 3->2 | 15->8 |
| shorter | exact_name | `tisp_s_raw_rw_control` | `tisp_s_raw_rw_control` | 84 | 53 | 0.631 | -31 | 5->5 | 8->1 |
| shorter | exact_name | `tisp_awb_deinit` | `tisp_awb_deinit` | 83 | 52 | 0.627 | -31 | 6->6 | 9->6 |
| shorter | exact_name | `tisp_bypass_update` | `tisp_bypass_update` | 83 | 56 | 0.675 | -27 | 5->5 | 0->0 |
| shorter | exact_name | `tisp_clm_init` | `tisp_clm_init` | 83 | 59 | 0.711 | -24 | 7->6 | 0->0 |
| shorter | exact_name | `tisp_mdns_func_en` | `tisp_mdns_func_en` | 82 | 46 | 0.561 | -36 | 2->1 | 3->3 |
| shorter | exact_name | `ispcore_frame_channel_ir_qbuf` | `ispcore_frame_channel_ir_qbuf` | 79 | 61 | 0.772 | -18 | 4->3 | 12->11 |
| shorter | exact_name | `tisp_ae_sepc_area` | `tisp_ae_sepc_area` | 78 | 46 | 0.590 | -32 | 1->1 | 4->4 |
| shorter | exact_name | `tisp_lce_process` | `tisp_lce_process` | 76 | 51 | 0.671 | -25 | 6->5 | 2->2 |
| shorter | exact_name | `tisp_wdr_degweight_ev` | `tisp_wdr_degweight_ev` | 76 | 59 | 0.776 | -17 | 0->0 | 13->11 |
| shorter | exact_name | `tisp_set_ae_long_ag` | `tisp_set_ae_long_ag` | 75 | 54 | 0.720 | -21 | 3->2 | 5->6 |
| shorter | exact_name | `tisp_set_ae_short_ag` | `tisp_set_ae_short_ag` | 75 | 54 | 0.720 | -21 | 3->2 | 5->6 |
| shorter | exact_name | `tisp_g_ae_statis_attr` | `tisp_g_ae_statis_attr` | 75 | 57 | 0.760 | -18 | 5->5 | 2->0 |
| shorter | exact_name | `tisp_ccm_interp_by_ev` | `tisp_ccm_interp_by_ev` | 74 | 47 | 0.635 | -27 | 0->0 | 14->8 |
| shorter | exact_name | `tisp_wdr_get_lum_s` | `tisp_wdr_get_lum_s` | 73 | 52 | 0.712 | -21 | 0->0 | 9->7 |
| shorter | exact_name | `tisp_gsm_init` | `tisp_gsm_init` | 71 | 55 | 0.775 | -16 | 6->2 | 0->0 |
| shorter | exact_name | `tisp_ydns_reg_cfg` | `tisp_ydns_reg_cfg` | 71 | 56 | 0.789 | -15 | 7->7 | 0->0 |
| shorter | exact_name | `tisp_blc_ag_interp` | `tisp_blc_ag_interp` | 70 | 50 | 0.714 | -20 | 5->4 | 1->1 |
| shorter | exact_name | `tisp_code_create_tuning_node` | `tisp_code_create_tuning_node` | 65 | 42 | 0.646 | -23 | 6->4 | 2->0 |
| shorter | exact_name | `ispcore_frame_channel_streamon` | `ispcore_frame_channel_streamon` | 64 | 37 | 0.578 | -27 | 4->2 | 7->4 |
| shorter | exact_name | `tisp_irsca_write_reg` | `tisp_irsca_write_reg` | 64 | 47 | 0.734 | -17 | 5->3 | 0->0 |
| shorter | exact_name | `tisp_awb_pm_suspend` | `tisp_awb_pm_suspend` | 62 | 45 | 0.726 | -17 | 3->3 | 3->1 |
| shorter | exact_name | `tiziano_adr_algorithm` | `tiziano_adr_algorithm` | 62 | 46 | 0.742 | -16 | 3->2 | 1->1 |
| shorter | exact_name | `tisp_blc_write_reg` | `tisp_blc_write_reg` | 58 | 37 | 0.638 | -21 | 4->3 | 0->0 |
| shorter | exact_name | `private_wait_event_interruptible` | `private_wait_event_interruptible` | 58 | 46 | 0.793 | -12 | 5->4 | 4->3 |
| shorter | exact_name | `tisp_set_fps` | `tisp_set_fps` | 57 | 32 | 0.561 | -25 | 3->2 | 2->0 |
| shorter | exact_name | `tisp_tmo_faceae_refresh` | `tisp_tmo_faceae_refresh` | 57 | 40 | 0.702 | -17 | 5->5 | 0->0 |
| shorter | exact_name | `tisp_ae_interrupt_hist` | `tisp_ae_interrupt_hist` | 56 | 43 | 0.768 | -13 | 5->4 | 3->0 |
| shorter | exact_name | `tisp_msca_deinit` | `tisp_msca_deinit` | 54 | 31 | 0.574 | -23 | 2->2 | 6->3 |
| shorter | exact_name | `tisp_tstp_mark2_func` | `tisp_tstp_mark2_func` | 53 | 40 | 0.755 | -13 | 4->4 | 1->1 |
| shorter | exact_name | `tisp_ccm_api_set` | `tisp_ccm_api_set` | 52 | 35 | 0.673 | -17 | 5->4 | 0->0 |
| shorter | exact_name | `tisp_s_raw_rw_control_internal` | `tisp_s_raw_rw_control_internal` | 51 | 30 | 0.588 | -21 | 3->3 | 2->0 |
| shorter | exact_name | `tx_isp_csi_remove` | `tx_isp_csi_remove` | 51 | 35 | 0.686 | -16 | 6->4 | 1->1 |
| shorter | exact_name | `tisp_sdns_pm_suspend` | `tisp_sdns_pm_suspend` | 51 | 37 | 0.725 | -14 | 2->2 | 2->2 |
| shorter | exact_name | `tisp_ysp_pm_suspend` | `tisp_ysp_pm_suspend` | 51 | 37 | 0.725 | -14 | 2->2 | 2->2 |
| shorter | exact_name | `ae_process_kthread` | `ae_process_kthread` | 50 | 29 | 0.580 | -21 | 5->4 | 4->1 |
| shorter | exact_name | `tisp_bcsh_api_set_ccm` | `tisp_bcsh_api_set_ccm` | 50 | 30 | 0.600 | -20 | 4->4 | 0->0 |
| shorter | exact_name | `tisp_s_hv_flip` | `tisp_s_hv_flip` | 50 | 31 | 0.620 | -19 | 1->1 | 0->0 |
| shorter | exact_name | `tisp_mdns_par_refresh` | `tisp_mdns_par_refresh` | 50 | 34 | 0.680 | -16 | 4->2 | 4->3 |
| shorter | exact_name | `tisp_ae_api_set_exp_list` | `tisp_ae_api_set_exp_list` | 50 | 39 | 0.780 | -11 | 0->0 | 2->2 |
| shorter | exact_name | `tisp_tmo_process` | `tisp_tmo_process` | 50 | 39 | 0.780 | -11 | 5->5 | 0->0 |
| shorter | exact_name | `tisp_adr_faceae_curve_adjust` | `tisp_adr_faceae_curve_adjust` | 48 | 31 | 0.646 | -17 | 0->0 | 4->3 |
| shorter | exact_name | `tx_isp_core_remove` | `tx_isp_core_remove` | 48 | 35 | 0.729 | -13 | 7->5 | 2->2 |
| shorter | exact_name | `tisp_mdns_api_ai_par_push` | `tisp_mdns_api_ai_par_push` | 48 | 36 | 0.750 | -12 | 5->5 | 0->0 |
| shorter | exact_name | `tisp_tmo_pm_suspend` | `tisp_tmo_pm_suspend` | 48 | 37 | 0.771 | -11 | 2->2 | 2->2 |
| shorter | exact_name | `tisp_set_frame_drop` | `tisp_set_frame_drop` | 46 | 31 | 0.674 | -15 | 4->3 | 4->3 |
| shorter | exact_name | `tisp_bcsh_param_array_set` | `tisp_bcsh_param_array_set` | 45 | 26 | 0.578 | -19 | 4->4 | 0->0 |
| shorter | exact_name | `tisp_ccm_param_array_set` | `tisp_ccm_param_array_set` | 45 | 30 | 0.667 | -15 | 5->4 | 0->0 |
| shorter | exact_name | `tisp_cdns_pm_suspend` | `tisp_cdns_pm_suspend` | 45 | 34 | 0.756 | -11 | 2->2 | 1->1 |
| shorter | exact_name | `isp_frame_done_wakeup` | `isp_frame_done_wakeup` | 44 | 27 | 0.614 | -17 | 4->2 | 2->0 |
| shorter | exact_name | `tisp_ccm_ev_update` | `tisp_ccm_ev_update` | 44 | 30 | 0.682 | -14 | 3->2 | 3->3 |
| shorter | exact_name | `ispcore_frame_channel_streamoff` | `ispcore_frame_channel_streamoff` | 44 | 32 | 0.727 | -12 | 4->2 | 3->3 |
| shorter | exact_name | `ispcore_sensor_ops_ioctl` | `ispcore_sensor_ops_ioctl` | 44 | 34 | 0.773 | -10 | 1->1 | 8->7 |
| shorter | exact_name | `tisp_adr_process_func` | `tisp_adr_process_func` | 43 | 24 | 0.558 | -19 | 7->5 | 0->0 |
| shorter | exact_name | `tisp_wdr_interrupt_static` | `tisp_wdr_interrupt_static` | 43 | 27 | 0.628 | -16 | 4->3 | 0->0 |
| shorter | exact_name | `tisp_defog_process` | `tisp_defog_process` | 43 | 29 | 0.674 | -14 | 3->2 | 2->1 |
| shorter | exact_name | `vic_core_ops_init` | `vic_core_ops_init` | 43 | 33 | 0.767 | -10 | 1->1 | 5->4 |
| shorter | exact_name | `tisp_wdr_pm_suspend` | `tisp_wdr_pm_suspend` | 42 | 27 | 0.643 | -15 | 1->1 | 2->2 |
| shorter | exact_name | `adr_remove` | `adr_remove` | 41 | 23 | 0.561 | -18 | 0->0 | 0->0 |
| shorter | exact_name | `tisp_ysp_par_refresh` | `tisp_ysp_par_refresh` | 41 | 24 | 0.585 | -17 | 1->1 | 5->3 |
| shorter | exact_name | `tisp_sdns_par_refresh` | `tisp_sdns_par_refresh` | 41 | 31 | 0.756 | -10 | 1->1 | 5->3 |
| shorter | exact_name | `table_intp` | `table_intp` | 40 | 23 | 0.575 | -17 | 1->0 | 3->5 |
| shorter | exact_name | `isp_mem_init` | `isp_mem_init` | 40 | 31 | 0.775 | -9 | 3->2 | 0->0 |
| shorter | exact_name | `tisp_tmo_wdr_en` | `tisp_tmo_wdr_en` | 40 | 31 | 0.775 | -9 | 6->5 | 0->0 |
| shorter | exact_name | `tx_isp_vic_remove` | `tx_isp_vic_remove` | 39 | 27 | 0.692 | -12 | 4->2 | 3->3 |
| shorter | exact_name | `api_ae_set_hist_bin` | `api_ae_set_hist_bin` | 39 | 30 | 0.769 | -9 | 3->2 | 0->1 |
| shorter | exact_name | `tisp_tmo_param_array_set` | `tisp_tmo_param_array_set` | 39 | 30 | 0.769 | -9 | 6->5 | 0->0 |
| shorter | exact_name | `api_ae_get_weight` | `api_ae_get_weight` | 38 | 27 | 0.711 | -11 | 1->1 | 0->0 |
| shorter | exact_name | `api_ae_get_hist_bin` | `api_ae_get_hist_bin` | 38 | 28 | 0.737 | -10 | 3->2 | 0->0 |
| shorter | exact_name | `api_ae_get_antiflicker_step` | `api_ae_get_antiflicker_step` | 38 | 30 | 0.789 | -8 | 2->2 | 1->1 |
| shorter | exact_name | `tisp_awb_dn_params_refresh` | `tisp_awb_dn_params_refresh` | 37 | 19 | 0.514 | -18 | 2->1 | 1->0 |
| shorter | exact_name | `tisp_lce_clr_ram` | `tisp_lce_clr_ram` | 37 | 19 | 0.514 | -18 | 2->2 | 2->1 |
| shorter | exact_name | `tisp_lsc_param_array_get` | `tisp_lsc_param_array_get` | 37 | 21 | 0.568 | -16 | 2->1 | 0->0 |
| shorter | exact_name | `tisp_code_destroy_tuning_node` | `tisp_code_destroy_tuning_node` | 37 | 27 | 0.730 | -10 | 3->3 | 1->1 |
| shorter | exact_name | `tx_isp_remove` | `tx_isp_remove` | 37 | 29 | 0.784 | -8 | 6->4 | 0->0 |
| shorter | exact_name | `tisp_cdns_par_refresh` | `tisp_cdns_par_refresh` | 35 | 24 | 0.686 | -11 | 1->1 | 4->3 |
| shorter | exact_name | `tisp_clm_dn_params_refresh` | `tisp_clm_dn_params_refresh` | 35 | 27 | 0.771 | -8 | 4->4 | 0->0 |
| shorter | exact_name | `fix_point_div_32` | `fix_point_div_32` | 34 | 18 | 0.529 | -16 | 1->1 | 4->2 |
| shorter | exact_name | `tisp_msca_addr_fifo_read` | `tisp_msca_addr_fifo_read` | 34 | 22 | 0.647 | -12 | 2->2 | 0->0 |
| shorter | exact_name | `tisp_ae_pm_suspend` | `tisp_ae_pm_suspend` | 34 | 23 | 0.676 | -11 | 1->1 | 1->1 |
| shorter | exact_name | `tisp_af_pm_suspend` | `tisp_af_pm_suspend` | 34 | 23 | 0.676 | -11 | 1->1 | 1->1 |
| shorter | exact_name | `tisp_bcsh_pm_suspend` | `tisp_bcsh_pm_suspend` | 34 | 23 | 0.676 | -11 | 1->1 | 1->1 |
| shorter | exact_name | `tisp_clm_pm_suspend` | `tisp_clm_pm_suspend` | 34 | 23 | 0.676 | -11 | 1->1 | 1->1 |
| shorter | exact_name | `tisp_gsm_pm_suspend` | `tisp_gsm_pm_suspend` | 34 | 23 | 0.676 | -11 | 1->1 | 1->1 |
| shorter | exact_name | `tisp_hldc_pm_suspend` | `tisp_hldc_pm_suspend` | 34 | 23 | 0.676 | -11 | 1->1 | 1->1 |
| shorter | exact_name | `func_gam_x2y` | `func_gam_x2y` | 34 | 26 | 0.765 | -8 | 0->0 | 3->3 |
| shorter | exact_name | `tisp_ccm_ct_update` | `tisp_ccm_ct_update` | 34 | 27 | 0.794 | -7 | 3->2 | 2->2 |
| shorter | exact_name | `tisp_adr_pm_suspend` | `tisp_adr_pm_suspend` | 33 | 17 | 0.515 | -16 | 1->1 | 1->1 |
| shorter | exact_name | `tisp_blc_pm_suspend` | `tisp_blc_pm_suspend` | 33 | 17 | 0.515 | -16 | 1->1 | 1->1 |
| shorter | exact_name | `tisp_csc_pm_suspend` | `tisp_csc_pm_suspend` | 33 | 17 | 0.515 | -16 | 1->1 | 1->1 |
| shorter | exact_name | `tisp_lce_pm_suspend` | `tisp_lce_pm_suspend` | 33 | 17 | 0.515 | -16 | 1->1 | 1->1 |
| shorter | exact_name | `tisp_mdns_pm_suspend` | `tisp_mdns_pm_suspend` | 33 | 17 | 0.515 | -16 | 1->1 | 1->1 |
| shorter | exact_name | `isp_printf` | `isp_printf` | 33 | 20 | 0.606 | -13 | 2->1 | 2->2 |
| shorter | exact_name | `tisp_gib_pm_suspend` | `tisp_gib_pm_suspend` | 33 | 22 | 0.667 | -11 | 1->1 | 1->1 |
| shorter | exact_name | `tisp_lsc_pm_suspend` | `tisp_lsc_pm_suspend` | 33 | 22 | 0.667 | -11 | 1->1 | 1->1 |
| shorter | exact_name | `tisp_tstp_pm_suspend` | `tisp_tstp_pm_suspend` | 33 | 22 | 0.667 | -11 | 1->1 | 1->1 |
| shorter | exact_name | `tisp_ydns_pm_suspend` | `tisp_ydns_pm_suspend` | 33 | 22 | 0.667 | -11 | 1->1 | 1->1 |
| shorter | exact_name | `tisp_gamma_wdr_en` | `tisp_gamma_wdr_en` | 33 | 24 | 0.727 | -9 | 4->4 | 0->0 |
| shorter | exact_name | `tisp_blc_dn_params_refresh` | `tisp_blc_dn_params_refresh` | 33 | 26 | 0.788 | -7 | 5->5 | 0->0 |
| shorter | exact_name | `tisp_channel_main_fifo_clear` | `tisp_channel_main_fifo_clear` | 32 | 23 | 0.719 | -9 | 4->4 | 0->0 |
| shorter | exact_name | `ivdc_disable_irq` | `ivdc_disable_irq` | 32 | 24 | 0.750 | -8 | 3->1 | 2->3 |
| shorter | exact_name | `tisp_g_ccm_attr` | `tisp_g_ccm_attr` | 32 | 24 | 0.750 | -8 | 2->2 | 5->4 |
| shorter | exact_name | `tx_isp_vin_remove` | `tx_isp_vin_remove` | 31 | 20 | 0.645 | -11 | 4->2 | 1->0 |
| shorter | exact_name | `tisp_gsm_api_set` | `tisp_gsm_api_set` | 30 | 23 | 0.767 | -7 | 3->2 | 0->1 |
| shorter | exact_name | `ispcore_s_wdr_en` | `ispcore_s_wdr_en` | 29 | 19 | 0.655 | -10 | 1->1 | 2->1 |
| shorter | exact_name | `tx_isp_ivdc_remove` | `tx_isp_ivdc_remove` | 29 | 19 | 0.655 | -10 | 4->3 | 0->0 |
| shorter | exact_name | `tisp_get_frame_drop` | `tisp_get_frame_drop` | 29 | 21 | 0.724 | -8 | 2->2 | 0->0 |
| shorter | exact_name | `tisp_tmo_dn_params_refresh` | `tisp_tmo_dn_params_refresh` | 28 | 19 | 0.679 | -9 | 5->4 | 0->0 |
| shorter | exact_name | `tisp_ydns_par_refresh` | `tisp_ydns_par_refresh` | 28 | 19 | 0.679 | -9 | 0->0 | 4->3 |
| shorter | exact_name | `tisp_simple_intp_int8` | `tisp_simple_intp_int8` | 28 | 22 | 0.786 | -6 | 0->0 | 6->3 |
| shorter | exact_name | `tisp_cdns_param_array_set` | `tisp_cdns_param_array_set` | 27 | 21 | 0.778 | -6 | 2->2 | 0->0 |
| shorter | exact_name | `tisp_ysp_param_array_set` | `tisp_ysp_param_array_set` | 27 | 21 | 0.778 | -6 | 2->2 | 0->0 |
| shorter | exact_name | `func_matrix_check_short` | `func_matrix_check_short` | 26 | 17 | 0.654 | -9 | 1->1 | 3->1 |
| shorter | exact_name | `tisp_ydns_refresh` | `tisp_ydns_refresh` | 26 | 18 | 0.692 | -8 | 2->3 | 0->0 |
| shorter | exact_name | `fs_suspend_module` | `fs_suspend_module` | 26 | 20 | 0.769 | -6 | 1->0 | 3->4 |
| shorter | exact_name | `tisp_defog_get_data` | `tisp_defog_get_data` | 26 | 20 | 0.769 | -6 | 0->0 | 1->1 |
| shorter | exact_name | `tisp_adr_faceae_refresh` | `tisp_adr_faceae_refresh` | 25 | 13 | 0.520 | -12 | 1->1 | 0->0 |
| shorter | exact_name | `tisp_msca_addr_fifo_write` | `tisp_msca_addr_fifo_write` | 25 | 14 | 0.560 | -11 | 1->1 | 0->0 |
| shorter | exact_name | `tisp_s_sdns_ratio` | `tisp_s_sdns_ratio` | 25 | 16 | 0.640 | -9 | 1->0 | 1->1 |
| shorter | exact_name | `sensor_alloc_digital_gain_short` | `sensor_alloc_digital_gain_short` | 25 | 18 | 0.720 | -7 | 1->1 | 2->1 |
| shorter | exact_name | `u3bw` | `u3bw` | 25 | 19 | 0.760 | -6 | 0->0 | 2->2 |
| shorter | exact_name | `tisp_msca_param_array_set` | `tisp_msca_param_array_set` | 24 | 15 | 0.625 | -9 | 2->2 | 0->0 |
| shorter | exact_name | `tisp_ae_lib_div_64` | `tisp_ae_lib_div_64` | 24 | 17 | 0.708 | -7 | 1->1 | 3->2 |
| shorter | exact_name | `private_gpio_direction_output` | `private_gpio_direction_output` | 23 | 13 | 0.565 | -10 | 0->1 | 2->1 |
| shorter | exact_name | `tisp_mdns_refresh` | `tisp_mdns_refresh` | 23 | 14 | 0.609 | -9 | 2->2 | 0->0 |
| shorter | exact_name | `tisp_lsc_gain_update` | `tisp_lsc_gain_update` | 23 | 17 | 0.739 | -6 | 2->2 | 1->1 |
| shorter | exact_name | `tisp_tmo_pm_resume` | `tisp_tmo_pm_resume` | 22 | 6 | 0.273 | -16 | 3->1 | 0->0 |
| shorter | exact_name | `tisp_msca_ir_init` | `tisp_msca_ir_init` | 22 | 10 | 0.455 | -12 | 1->0 | 0->0 |
| shorter | exact_name | `sensor_alloc_analog_gain` | `sensor_alloc_analog_gain` | 22 | 12 | 0.545 | -10 | 1->1 | 0->0 |
| shorter | exact_name | `sensor_alloc_analog_gain_short` | `sensor_alloc_analog_gain_short` | 22 | 12 | 0.545 | -10 | 1->1 | 0->0 |
| shorter | exact_name | `sensor_alloc_digital_gain` | `sensor_alloc_digital_gain` | 22 | 12 | 0.545 | -10 | 1->1 | 0->0 |
| shorter | exact_name | `tisp_bcsh_api_set_brightness` | `tisp_bcsh_api_set_brightness` | 22 | 13 | 0.591 | -9 | 1->1 | 0->0 |
| shorter | exact_name | `tisp_bcsh_api_set_contrast` | `tisp_bcsh_api_set_contrast` | 22 | 13 | 0.591 | -9 | 1->1 | 0->0 |
| shorter | exact_name | `tisp_bcsh_api_set_hue` | `tisp_bcsh_api_set_hue` | 22 | 13 | 0.591 | -9 | 1->1 | 0->0 |
| shorter | exact_name | `tisp_bcsh_api_set_saturation` | `tisp_bcsh_api_set_saturation` | 22 | 13 | 0.591 | -9 | 1->1 | 0->0 |
| shorter | exact_name | `tisp_clm_refresh_by_csc` | `tisp_clm_refresh_by_csc` | 22 | 14 | 0.636 | -8 | 2->2 | 0->0 |
| shorter | exact_name | `tisp_ysp_all_reg_refresh` | `tisp_ysp_all_reg_refresh` | 22 | 14 | 0.636 | -8 | 3->3 | 0->0 |
| shorter | exact_name | `tisp_clm_ct_update` | `tisp_clm_ct_update` | 22 | 15 | 0.682 | -7 | 2->2 | 1->1 |
| shorter | exact_name | `tisp_gib_api_get_blc` | `tisp_gib_api_get_blc` | 22 | 17 | 0.773 | -5 | 0->0 | 1->1 |
| shorter | exact_name | `system_reg_set_awb_trig` | `system_reg_set_awb_trig` | 21 | 2 | 0.095 | -19 | 0->0 | 4->0 |
| shorter | exact_name | `tisp_bcsh_itp` | `tisp_bcsh_itp` | 21 | 2 | 0.095 | -19 | 0->0 | 4->1 |
| shorter | exact_name | `lib_tisp_debug_info` | `lib_tisp_debug_info` | 21 | 10 | 0.476 | -11 | 1->1 | 0->0 |
| shorter | exact_name | `tisp_blc_calc_self_gain` | `tisp_blc_calc_self_gain` | 20 | 2 | 0.100 | -18 | 0->0 | 1->0 |
| shorter | exact_name | `tisp_ae_get_ev_list` | `tisp_ae_get_ev_list` | 20 | 11 | 0.550 | -9 | 0->0 | 1->1 |
| shorter | exact_name | `tisp_msca_ram_write` | `tisp_msca_ram_write` | 20 | 11 | 0.550 | -9 | 1->1 | 0->0 |
| shorter | exact_name | `tisp_blc_api_get` | `tisp_blc_api_get` | 20 | 15 | 0.750 | -5 | 0->0 | 1->1 |
| shorter | exact_name | `tisp_wdr_log2` | `tisp_wdr_log2` | 19 | 2 | 0.105 | -17 | 0->0 | 4->0 |
| shorter | exact_name | `tisp_awb_set_weight` | `tisp_awb_set_weight` | 19 | 9 | 0.474 | -10 | 0->0 | 1->1 |
| shorter | exact_name | `tisp_ae_get_at_list` | `tisp_ae_get_at_list` | 19 | 10 | 0.526 | -9 | 0->0 | 1->1 |
| shorter | exact_name | `tisp_s_tmo_faceae` | `tisp_s_tmo_faceae` | 19 | 10 | 0.526 | -9 | 1->1 | 0->0 |
| shorter | exact_name | `sensor_get_normal_fps` | `sensor_get_normal_fps` | 19 | 11 | 0.579 | -8 | 0->0 | 0->0 |
| shorter | exact_name | `private_request_module` | `private_request_module` | 19 | 13 | 0.684 | -6 | 1->1 | 0->0 |
| shorter | exact_name | `tisp_ae_get_converge_step` | `tisp_ae_get_converge_step` | 19 | 15 | 0.789 | -4 | 0->0 | 0->0 |
| shorter | exact_name | `tisp_ae_set_converge_step` | `tisp_ae_set_converge_step` | 19 | 15 | 0.789 | -4 | 0->0 | 0->0 |
| shorter | exact_name | `tisp_wdr_pm_resume` | `tisp_wdr_pm_resume` | 19 | 15 | 0.789 | -4 | 1->1 | 1->1 |
| shorter | exact_name | `get_distance_1dim` | `get_distance_1dim` | 18 | 2 | 0.111 | -16 | 0->0 | 3->1 |
| shorter | exact_name | `tisp_ae_process` | `tisp_ae_process` | 18 | 6 | 0.333 | -12 | 1->1 | 0->0 |
| shorter | exact_name | `tiziano_adr_gamma_refresh` | `tiziano_adr_gamma_refresh` | 18 | 9 | 0.500 | -9 | 0->0 | 0->1 |
| shorter | exact_name | `dump_aisp_info_open` | `dump_aisp_info_open` | 18 | 10 | 0.556 | -8 | 1->1 | 0->0 |
| shorter | exact_name | `dump_isp_csi_open` | `dump_isp_csi_open` | 18 | 10 | 0.556 | -8 | 1->1 | 0->0 |
| shorter | exact_name | `dump_isp_framesource_open` | `dump_isp_framesource_open` | 18 | 10 | 0.556 | -8 | 1->1 | 0->0 |
| shorter | exact_name | `dump_isp_info_open` | `dump_isp_info_open` | 18 | 10 | 0.556 | -8 | 1->1 | 0->0 |
| shorter | exact_name | `dump_isp_vic_frd_open` | `dump_isp_vic_frd_open` | 18 | 10 | 0.556 | -8 | 1->1 | 0->0 |
| shorter | exact_name | `proc_ivdc_open` | `proc_ivdc_open` | 18 | 10 | 0.556 | -8 | 1->1 | 0->0 |
| shorter | exact_name | `tisp_cdns_all_reg_refresh` | `tisp_cdns_all_reg_refresh` | 18 | 10 | 0.556 | -8 | 2->2 | 0->0 |
| shorter | exact_name | `tisp_log2_fixed_to_fixed_64` | `tisp_log2_fixed_to_fixed_64` | 18 | 10 | 0.556 | -8 | 1->1 | 0->0 |
| shorter | exact_name | `tisp_ysp_intp_reg_refresh` | `tisp_ysp_intp_reg_refresh` | 18 | 10 | 0.556 | -8 | 2->2 | 0->0 |
| shorter | exact_name | `video_input_cmd_open` | `video_input_cmd_open` | 18 | 10 | 0.556 | -8 | 1->1 | 0->0 |
| shorter | exact_name | `private_seq_printf` | `private_seq_printf` | 18 | 13 | 0.722 | -5 | 1->1 | 0->0 |
| shorter | exact_name | `tisp_dmsc_intp_reg_refresh` | `tisp_dmsc_intp_reg_refresh` | 18 | 14 | 0.778 | -4 | 2->2 | 0->0 |
| shorter | exact_name | `tisp_sdns_intp_reg_refresh` | `tisp_sdns_intp_reg_refresh` | 18 | 14 | 0.778 | -4 | 2->2 | 0->0 |
| shorter | exact_name | `tisp_ydns_all_reg_refresh` | `tisp_ydns_all_reg_refresh` | 18 | 14 | 0.778 | -4 | 2->2 | 0->0 |
| shorter | exact_name | `private_copy_to_user` | `private_copy_to_user` | 17 | 10 | 0.588 | -7 | 1->1 | 1->0 |
| shorter | exact_name | `private_log2_fixed_to_fixed_64` | `private_log2_fixed_to_fixed_64` | 17 | 10 | 0.588 | -7 | 1->1 | 0->0 |
| shorter | exact_name | `tisp_lce_top_change_state` | `tisp_lce_top_change_state` | 17 | 10 | 0.588 | -7 | 0->0 | 1->1 |
| shorter | exact_name | `lce_compress_data` | `lce_compress_data` | 17 | 12 | 0.706 | -5 | 0->0 | 2->1 |
| shorter | exact_name | `tisp_clm_itp` | `tisp_clm_itp` | 17 | 13 | 0.765 | -4 | 1->1 | 0->0 |
| shorter | exact_name | `tisp_cdns_dn_params_refresh` | `tisp_cdns_dn_params_refresh` | 16 | 6 | 0.375 | -10 | 1->1 | 0->0 |
| shorter | exact_name | `tisp_clm_pm_resume` | `tisp_clm_pm_resume` | 16 | 6 | 0.375 | -10 | 2->1 | 0->0 |
| shorter | exact_name | `tisp_awb_get_weight` | `tisp_awb_get_weight` | 16 | 7 | 0.438 | -9 | 0->0 | 1->1 |
| shorter | exact_name | `tisp_dmsc_dn_params_refresh` | `tisp_dmsc_dn_params_refresh` | 16 | 10 | 0.625 | -6 | 1->1 | 0->0 |
| shorter | exact_name | `tisp_ydns_dn_params_refresh` | `tisp_ydns_dn_params_refresh` | 16 | 10 | 0.625 | -6 | 1->1 | 0->0 |
| shorter | exact_name | `tisp_csc_pm_resume` | `tisp_csc_pm_resume` | 16 | 12 | 0.750 | -4 | 2->0 | 0->0 |
| shorter | exact_name | `tisp_top_dn_params_refresh` | `tisp_top_dn_params_refresh` | 15 | 2 | 0.133 | -13 | 0->0 | 0->0 |
| shorter | exact_name | `tisp_ipc_triger` | `tisp_ipc_triger` | 15 | 7 | 0.467 | -8 | 1->0 | 0->0 |
| shorter | exact_name | `tisp_log2_fixed_to_fixed` | `tisp_log2_fixed_to_fixed` | 14 | 2 | 0.143 | -12 | 1->0 | 0->0 |
| shorter | exact_name | `tisp_day_or_night_s_ctrl` | `tisp_day_or_night_s_ctrl` | 14 | 10 | 0.714 | -4 | 1->1 | 0->0 |
| shorter | exact_name | `tisp_mdns_all_reg_refresh` | `tisp_mdns_all_reg_refresh` | 14 | 10 | 0.714 | -4 | 1->1 | 0->0 |
| shorter | exact_name | `tisp_set_brightness` | `tisp_set_brightness` | 14 | 10 | 0.714 | -4 | 1->1 | 0->0 |
| shorter | exact_name | `tisp_set_contrast` | `tisp_set_contrast` | 14 | 10 | 0.714 | -4 | 1->1 | 0->0 |
| shorter | exact_name | `tisp_set_hue` | `tisp_set_hue` | 14 | 10 | 0.714 | -4 | 1->1 | 0->0 |
| shorter | exact_name | `tisp_set_saturation` | `tisp_set_saturation` | 14 | 10 | 0.714 | -4 | 1->1 | 0->0 |
| shorter | exact_name | `tisp_af_api_get_weight` | `tisp_af_api_get_weight` | 13 | 2 | 0.154 | -11 | 0->0 | 0->0 |
| shorter | exact_name | `private_log2_fixed_to_fixed` | `private_log2_fixed_to_fixed` | 13 | 6 | 0.462 | -7 | 1->0 | 0->2 |
| shorter | exact_name | `tisp_awb_gain_reg` | `tisp_awb_gain_reg` | 13 | 8 | 0.615 | -5 | 0->0 | 0->0 |
| shorter | exact_name | `tisp_af_api_get_statis_info` | `tisp_af_api_get_statis_info` | 12 | 2 | 0.167 | -10 | 0->0 | 0->0 |
| shorter | exact_name | `tisp_max` | `tisp_max` | 12 | 2 | 0.167 | -10 | 0->0 | 3->0 |
| shorter | exact_name | `tisp_min` | `tisp_min` | 12 | 2 | 0.167 | -10 | 0->0 | 3->0 |
| shorter | exact_name | `tisp_tmo_interplate` | `tisp_tmo_interplate` | 12 | 2 | 0.167 | -10 | 0->0 | 0->0 |
| shorter | exact_name | `private_ktime_set` | `private_ktime_set` | 12 | 4 | 0.333 | -8 | 0->0 | 0->0 |
| shorter | exact_name | `tisp_s_tmo_curve` | `tisp_s_tmo_curve` | 12 | 8 | 0.667 | -4 | 1->1 | 0->0 |
| shorter | exact_name | `tisp_wdr_pm_get_regsize` | `tisp_wdr_pm_get_regsize` | 12 | 8 | 0.667 | -4 | 0->0 | 1->1 |
| shorter | exact_name | `tisp_ae_pm_resume` | `tisp_ae_pm_resume` | 12 | 9 | 0.750 | -3 | 1->0 | 0->0 |
| shorter | exact_name | `tisp_af_pm_resume` | `tisp_af_pm_resume` | 12 | 9 | 0.750 | -3 | 1->0 | 0->0 |
| shorter | exact_name | `tisp_cdns_pm_resume` | `tisp_cdns_pm_resume` | 12 | 9 | 0.750 | -3 | 1->0 | 0->0 |
| shorter | exact_name | `tisp_hldc_pm_resume` | `tisp_hldc_pm_resume` | 12 | 9 | 0.750 | -3 | 1->0 | 0->0 |
| shorter | exact_name | `tisp_msca_pm_resume` | `tisp_msca_pm_resume` | 12 | 9 | 0.750 | -3 | 1->0 | 0->0 |
| shorter | exact_name | `tisp_sdns_pm_resume` | `tisp_sdns_pm_resume` | 12 | 9 | 0.750 | -3 | 1->0 | 0->0 |
| shorter | exact_name | `tisp_ysp_pm_resume` | `tisp_ysp_pm_resume` | 12 | 9 | 0.750 | -3 | 1->0 | 0->0 |
| shorter | exact_name | `check_state` | `check_state` | 11 | 2 | 0.182 | -9 | 0->0 | 2->0 |
| shorter | exact_name | `fix_point_mult3_32` | `fix_point_mult3_32` | 11 | 2 | 0.182 | -9 | 1->0 | 0->0 |
| shorter | exact_name | `tisp_af_api_set_weight` | `tisp_af_api_set_weight` | 11 | 2 | 0.182 | -9 | 0->0 | 0->0 |
| shorter | exact_name | `tisp_wdr_interplate` | `tisp_wdr_interplate` | 11 | 2 | 0.182 | -9 | 0->0 | 0->0 |
| shorter | exact_name | `tisp_ae_algo_deinit_internal` | `tisp_ae_algo_deinit_internal` | 11 | 7 | 0.636 | -4 | 0->0 | 1->1 |
| shorter | exact_name | `tisp_awb_set_ct` | `tisp_awb_set_ct` | 11 | 7 | 0.636 | -4 | 0->0 | 0->0 |
| shorter | exact_name | `tisp_awb_get_frz` | `tisp_awb_get_frz` | 11 | 8 | 0.727 | -3 | 0->0 | 0->0 |
| shorter | exact_name | `tisp_g_af_statis_attr` | `tisp_g_af_statis_attr` | 10 | 2 | 0.200 | -8 | 1->0 | 0->0 |
| shorter | exact_name | `tisp_g_af_weight_attr` | `tisp_g_af_weight_attr` | 10 | 2 | 0.200 | -8 | 1->0 | 0->0 |
| shorter | exact_name | `tisp_g_bin_version` | `tisp_g_bin_version` | 10 | 2 | 0.200 | -8 | 1->0 | 0->0 |
| shorter | exact_name | `tisp_mdns_pm_resume` | `tisp_mdns_pm_resume` | 10 | 2 | 0.200 | -8 | 1->0 | 0->0 |
| shorter | exact_name | `tisp_s_af_weight_attr` | `tisp_s_af_weight_attr` | 10 | 2 | 0.200 | -8 | 1->0 | 0->0 |
| shorter | exact_name | `tisp_ydns_pm_resume` | `tisp_ydns_pm_resume` | 10 | 2 | 0.200 | -8 | 1->0 | 0->0 |
| shorter | exact_name | `tisp_ae_face_get` | `tisp_ae_face_get` | 10 | 6 | 0.600 | -4 | 1->1 | 0->0 |
| shorter | exact_name | `tisp_awb_set_frz` | `tisp_awb_set_frz` | 10 | 6 | 0.600 | -4 | 0->0 | 0->0 |
| shorter | exact_name | `tisp_g_awb_ct_trend_offset` | `tisp_g_awb_ct_trend_offset` | 10 | 6 | 0.600 | -4 | 1->1 | 0->0 |
| shorter | exact_name | `tisp_g_awb_weight_attr` | `tisp_g_awb_weight_attr` | 10 | 6 | 0.600 | -4 | 1->1 | 0->0 |
| shorter | exact_name | `tisp_s_awb_ct_trend_offset` | `tisp_s_awb_ct_trend_offset` | 10 | 6 | 0.600 | -4 | 1->1 | 0->0 |
| shorter | exact_name | `tisp_s_awb_weight_attr` | `tisp_s_awb_weight_attr` | 10 | 6 | 0.600 | -4 | 1->1 | 0->0 |
| shorter | exact_name | `tisp_sdns_dn_params_refresh` | `tisp_sdns_dn_params_refresh` | 9 | 2 | 0.222 | -7 | 0->0 | 0->0 |
| shorter | exact_name | `lsc_exchange_data` | `lsc_exchange_data` | 9 | 4 | 0.444 | -5 | 0->0 | 0->1 |
| shorter | exact_name | `tisp_ae_api_get_flicker_flag` | `tisp_ae_api_get_flicker_flag` | 9 | 5 | 0.556 | -4 | 0->0 | 0->0 |
| shorter | exact_name | `tisp_ae_api_set_ev_start` | `tisp_ae_api_set_ev_start` | 9 | 5 | 0.556 | -4 | 0->0 | 0->0 |
| shorter | exact_name | `tisp_bcsh_api_get_brightness` | `tisp_bcsh_api_get_brightness` | 9 | 5 | 0.556 | -4 | 0->0 | 0->0 |
| shorter | exact_name | `tisp_bcsh_api_get_contrast` | `tisp_bcsh_api_get_contrast` | 9 | 5 | 0.556 | -4 | 0->0 | 0->0 |
| shorter | exact_name | `tisp_bcsh_api_get_hue` | `tisp_bcsh_api_get_hue` | 9 | 5 | 0.556 | -4 | 0->0 | 0->0 |
| shorter | exact_name | `tisp_bcsh_api_get_saturation` | `tisp_bcsh_api_get_saturation` | 9 | 5 | 0.556 | -4 | 0->0 | 0->0 |
| shorter | exact_name | `tisp_get_lsc_error` | `tisp_get_lsc_error` | 9 | 5 | 0.556 | -4 | 0->0 | 0->0 |
| shorter | exact_name | `tisp_lsc_prepare_write_lut` | `tisp_lsc_prepare_write_lut` | 9 | 7 | 0.778 | -2 | 0->0 | 0->0 |
| shorter | exact_name | `tx_isp_module_deinit` | `tx_isp_module_deinit` | 8 | 2 | 0.250 | -6 | 0->0 | 1->0 |
| shorter | exact_name | `tisp_mdns_wdr_en` | `tisp_mdns_wdr_en` | 8 | 4 | 0.500 | -4 | 0->0 | 0->0 |
| shorter | exact_name | `cdns_remove` | `cdns_remove` | 8 | 5 | 0.625 | -3 | 0->0 | 0->0 |
| shorter | exact_name | `tisp_ae_par_sensor_trig` | `tisp_ae_par_sensor_trig` | 8 | 5 | 0.625 | -3 | 0->0 | 0->0 |
| shorter | exact_name | `tisp_bcsh_set_write_reg_event` | `tisp_bcsh_set_write_reg_event` | 8 | 5 | 0.625 | -3 | 0->0 | 0->0 |
