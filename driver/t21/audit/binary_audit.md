# Binary Assembly Audit

- schema: `regtrace-binary-audit-v1`
- OEM: `../tx-isp-t21.ko`
- recovered: `driver/t21/tx-isp-t21.ko`
- objdump counts exclude relocation records
- thresholds: min_oem_insns=24 stub_insns=8 collapse=0.50 similar=0.80..1.25 expansion=2.00

## Summary

| Metric | OEM | Recovered |
|---|---:|---:|
| Function symbols | 630 | 682 |
| Functions with disassembly | 630 | 682 |
| Executable section bytes | 215240 | 169952 |
| Initialized writable bytes | 283936 | 276528 |
| Uninitialized writable bytes | 101040 | 452144 |

- direct matches: 612
- replacement matches: 0 (missing=0)
- unmatched: OEM-only=18 recovered-only=70
- matched instructions: OEM=52693 recovered=41497 ratio=0.788
- classes: stub=4 collapsed=25 shorter=110 same_count=137 similar=318 larger=11 expanded=7

## Allocated Section Delta

| Section | OEM bytes | Recovered bytes | Delta |
|---|---:|---:|---:|
| `.MIPS.abiflags` | 24 | 24 | +0 |
| `.bss` | 101040 | 452144 | +351104 |
| `.data` | 283648 | 276240 | -7408 |
| `.exit.text` | 832 | 0 | -832 |
| `.gnu.linkonce.this_module` | 288 | 288 | +0 |
| `.init.text` | 72 | 0 | -72 |
| `.modinfo` | 252 | 252 | +0 |
| `.note.gnu.build-id` | 36 | 36 | +0 |
| `.reginfo` | 24 | 24 | +0 |
| `.rodata` | 7296 | 5408 | -1888 |
| `.rodata.cst16` | 0 | 80 | +80 |
| `.rodata.cst32` | 0 | 192 | +192 |
| `.rodata.cst4` | 0 | 16 | +16 |
| `.rodata.cst8` | 0 | 16 | +16 |
| `.rodata.str1.4` | 11016 | 9512 | -1504 |
| `.text` | 214336 | 169952 | -44384 |
| `__ksymtab` | 240 | 240 | +0 |
| `__ksymtab_strings` | 637 | 637 | +0 |
| `__param` | 32 | 32 | +0 |

## Function Outliers

Showing 245 of 245 outliers. JSON and CSV contain every comparison row.

| Class | Match | OEM symbol | Recovered symbol(s) | OEM insns | Recovered insns | Ratio | Delta | Calls | Branches |
|---|---|---|---|---:|---:|---:|---:|---:|---:|
| stub | exact_name | `Tiziano_Awb_Ct_Detect` | `Tiziano_Awb_Ct_Detect` | 1439 | 2 | 0.001 | -1437 | 43->0 | 133->0 |
| stub | exact_name | `tisp_log2_int_to_fixed_64` | `tisp_log2_int_to_fixed_64` | 130 | 4 | 0.031 | -126 | 3->0 | 15->1 |
| stub | exact_name | `tisp_math_exp2` | `tisp_math_exp2` | 57 | 4 | 0.070 | -53 | 1->0 | 2->1 |
| stub | exact_name | `tisp_sdns_intp_reg_refresh` | `tisp_sdns_intp_reg_refresh` | 27 | 4 | 0.148 | -23 | 5->0 | 0->1 |
| collapsed | exact_name | `Tiziano_adr_fpga` | `Tiziano_adr_fpga` | 2379 | 39 | 0.016 | -2340 | 136->1 | 164->2 |
| collapsed | exact_name | `ae_tune2` | `ae_tune2` | 1147 | 27 | 0.024 | -1120 | 68->1 | 113->1 |
| collapsed | exact_name | `apical_isp_core_ops_s_ctrl` | `apical_isp_core_ops_s_ctrl` | 691 | 249 | 0.360 | -442 | 26->10 | 153->59 |
| collapsed | exact_name | `Tiziano_defog_fpga` | `Tiziano_defog_fpga` | 627 | 179 | 0.285 | -448 | 24->4 | 77->12 |
| collapsed | exact_name | `tisp_mdns_param_array_get` | `tisp_mdns_param_array_get` | 558 | 48 | 0.086 | -510 | 2->2 | 132->4 |
| collapsed | exact_name | `tiziano_defog_algorithm` | `tiziano_defog_algorithm` | 465 | 171 | 0.368 | -294 | 2->2 | 26->16 |
| collapsed | exact_name | `isp_vic_cmd_set` | `isp_vic_cmd_set` | 436 | 168 | 0.385 | -268 | 41->16 | 53->18 |
| collapsed | exact_name | `tisp_ae_process_impl` | `tisp_ae_process_impl` | 432 | 123 | 0.285 | -309 | 15->1 | 23->3 |
| collapsed | exact_name | `tiziano_adr_algorithm` | `tiziano_adr_algorithm` | 397 | 75 | 0.189 | -322 | 1->1 | 25->1 |
| collapsed | exact_name | `Tiziano_ae_fpga` | `Tiziano_ae_fpga` | 230 | 94 | 0.409 | -136 | 6->6 | 20->6 |
| collapsed | exact_name | `tisp_dmsc_param_array_get` | `tisp_dmsc_param_array_get` | 180 | 48 | 0.267 | -132 | 2->2 | 47->4 |
| collapsed | exact_name | `tisp_dmsc_param_array_set` | `tisp_dmsc_param_array_set` | 179 | 47 | 0.263 | -132 | 3->2 | 48->4 |
| collapsed | exact_name | `tisp_dpc_param_array_get` | `tisp_dpc_param_array_get` | 143 | 48 | 0.336 | -95 | 2->2 | 35->4 |
| collapsed | exact_name | `jz_isp_ccm` | `jz_isp_ccm` | 142 | 59 | 0.415 | -83 | 5->4 | 16->4 |
| collapsed | exact_name | `tisp_sdns_param_array_get` | `tisp_sdns_param_array_get` | 132 | 48 | 0.364 | -84 | 2->2 | 32->4 |
| collapsed | exact_name | `tisp_adr_param_array_get` | `tisp_adr_param_array_get` | 127 | 47 | 0.370 | -80 | 2->2 | 28->4 |
| collapsed | exact_name | `tisp_awb_param_array_get` | `tisp_awb_param_array_get` | 123 | 47 | 0.382 | -76 | 2->2 | 27->4 |
| collapsed | exact_name | `tisp_defog_param_array_get` | `tisp_defog_param_array_get` | 116 | 48 | 0.414 | -68 | 2->2 | 25->4 |
| collapsed | exact_name | `tisp_ae_param_array_get` | `tisp_ae_param_array_get` | 112 | 48 | 0.429 | -64 | 2->2 | 23->4 |
| collapsed | exact_name | `tisp_gib_param_array_get` | `tisp_gib_param_array_get` | 110 | 48 | 0.436 | -62 | 2->2 | 24->4 |
| collapsed | exact_name | `tisp_gib_param_array_set` | `tisp_gib_param_array_set` | 110 | 50 | 0.455 | -60 | 3->3 | 25->4 |
| collapsed | exact_name | `tisp_af_param_array_get` | `tisp_af_param_array_get` | 105 | 20 | 0.190 | -85 | 2->1 | 21->1 |
| collapsed | exact_name | `tisp_ccm_param_array_set` | `tisp_ccm_param_array_set` | 62 | 24 | 0.387 | -38 | 3->2 | 9->1 |
| collapsed | exact_name | `tisp_ev_update` | `tisp_ev_update` | 35 | 16 | 0.457 | -19 | 4->1 | 1->1 |
| collapsed | exact_name | `tisp_g_Gamma` | `tisp_g_Gamma` | 26 | 12 | 0.462 | -14 | 2->1 | 1->0 |
| oem_only | oem_only | `apical_isp_ae_s_roi.isra.45` |  | 86 | 0 | 0.000 | -86 | 7->0 | 10->0 |
| oem_only | oem_only | `apical_isp_ae_zone_weight_s_attr.isra.50` |  | 86 | 0 | 0.000 | -86 | 7->0 | 10->0 |
| oem_only | oem_only | `apical_isp_af_weight_s_attr.isra.54` |  | 86 | 0 | 0.000 | -86 | 7->0 | 10->0 |
| oem_only | oem_only | `apical_isp_wb_g_ctrl.isra.65` |  | 80 | 0 | 0.000 | -80 | 4->0 | 15->0 |
| oem_only | oem_only | `apical_isp_ae_g_roi.isra.64` |  | 77 | 0 | 0.000 | -77 | 6->0 | 8->0 |
| oem_only | oem_only | `apical_isp_ae_zone_weight_g_attr.isra.70` |  | 77 | 0 | 0.000 | -77 | 6->0 | 8->0 |
| oem_only | oem_only | `apical_isp_af_weight_g_attr.isra.74` |  | 77 | 0 | 0.000 | -77 | 6->0 | 8->0 |
| oem_only | oem_only | `tx_isp_video_link_destroy.isra.1` |  | 72 | 0 | 0.000 | -72 | 4->0 | 9->0 |
| oem_only | oem_only | `apical_isp_gamma_g_attr.isra.63` |  | 49 | 0 | 0.000 | -49 | 3->0 | 3->0 |
| oem_only | oem_only | `apical_isp_ev_g_attr.isra.62` |  | 46 | 0 | 0.000 | -46 | 3->0 | 2->0 |
| oem_only | oem_only | `apical_isp_gamma_s_attr.isra.44` |  | 46 | 0 | 0.000 | -46 | 3->0 | 4->0 |
| oem_only | oem_only | `apical_isp_af_hist_g_attr.isra.73` |  | 43 | 0 | 0.000 | -43 | 2->0 | 0->0 |
| oem_only | oem_only | `apical_isp_max_again_g_ctrl.isra.60` |  | 30 | 0 | 0.000 | -30 | 2->0 | 2->0 |
| oem_only | oem_only | `apical_isp_max_dgain_g_ctrl.isra.61` |  | 30 | 0 | 0.000 | -30 | 2->0 | 2->0 |
| oem_only | oem_only | `apical_isp_ae_zone_g_ctrl.isra.69` |  | 19 | 0 | 0.000 | -19 | 2->0 | 0->0 |
| oem_only | oem_only | `tx_isp_driver_init` |  | 18 | 0 | 0.000 | -18 | 2->0 | 1->0 |
| oem_only | oem_only | `tisp_hldc_par_refresh.part.0` |  | 15 | 0 | 0.000 | -15 | 1->0 | 1->0 |
| oem_only | oem_only | `tx_isp_driver_exit` |  | 11 | 0 | 0.000 | -11 | 1->0 | 1->0 |
| shorter | exact_name | `frame_channel_unlocked_ioctl` | `frame_channel_unlocked_ioctl` | 953 | 605 | 0.635 | -348 | 57->26 | 209->65 |
| shorter | exact_name | `tx_isp_unlocked_ioctl` | `tx_isp_unlocked_ioctl` | 766 | 437 | 0.570 | -329 | 40->29 | 141->62 |
| shorter | exact_name | `tisp_mdns_y_2d_param_cfg` | `tisp_mdns_y_2d_param_cfg` | 645 | 494 | 0.766 | -151 | 30->30 | 0->0 |
| shorter | exact_name | `Tiziano_awb_fpga` | `Tiziano_awb_fpga` | 625 | 413 | 0.661 | -212 | 14->12 | 55->35 |
| shorter | exact_name | `tisp_param_operate_process` | `tisp_param_operate_process` | 608 | 484 | 0.796 | -124 | 20->23 | 104->91 |
| shorter | exact_name | `apical_isp_core_ops_g_ctrl` | `apical_isp_core_ops_g_ctrl` | 441 | 262 | 0.594 | -179 | 20->8 | 113->72 |
| shorter | exact_name | `ispcore_core_ops_init` | `ispcore_core_ops_init` | 434 | 291 | 0.671 | -143 | 15->17 | 114->39 |
| shorter | exact_name | `subdev_sensor_ops_ioctl` | `subdev_sensor_ops_ioctl` | 371 | 280 | 0.755 | -91 | 26->21 | 74->51 |
| shorter | exact_name | `jz_isp_lsc_ct` | `jz_isp_lsc_ct` | 356 | 220 | 0.618 | -136 | 12->6 | 48->20 |
| shorter | exact_name | `ispcore_interrupt_service_routine` | `ispcore_interrupt_service_routine` | 297 | 197 | 0.663 | -100 | 12->11 | 32->20 |
| shorter | exact_name | `isp_framesource_show` | `isp_framesource_show` | 293 | 202 | 0.689 | -91 | 24->22 | 23->9 |
| shorter | exact_name | `tx_isp_vic_start` | `tx_isp_vic_start` | 233 | 138 | 0.592 | -95 | 3->1 | 46->29 |
| shorter | exact_name | `cm_control` | `cm_control` | 229 | 140 | 0.611 | -89 | 9->9 | 15->2 |
| shorter | exact_name | `JZ_Isp_Awb` | `JZ_Isp_Awb` | 206 | 161 | 0.782 | -45 | 3->3 | 14->10 |
| shorter | exact_name | `tisp_mdns_c_adj_param_cfg` | `tisp_mdns_c_adj_param_cfg` | 158 | 116 | 0.734 | -42 | 9->8 | 0->1 |
| shorter | exact_name | `tisp_day_or_night_s_ctrl` | `tisp_day_or_night_s_ctrl` | 129 | 99 | 0.767 | -30 | 20->14 | 5->5 |
| shorter | exact_name | `tisp_adr_process` | `tisp_adr_process` | 124 | 89 | 0.718 | -35 | 11->10 | 6->5 |
| shorter | exact_name | `tiziano_af_params_refresh` | `tiziano_af_params_refresh` | 124 | 93 | 0.750 | -31 | 19->19 | 0->0 |
| shorter | exact_name | `tisp_mdns_sta_func_cfg` | `tisp_mdns_sta_func_cfg` | 123 | 92 | 0.748 | -31 | 8->7 | 12->1 |
| shorter | exact_name | `tisp_sdns_c_param_cfg` | `tisp_sdns_c_param_cfg` | 122 | 84 | 0.689 | -38 | 5->5 | 0->0 |
| shorter | exact_name | `fix_point_mult2` | `fix_point_mult2` | 109 | 87 | 0.798 | -22 | 5->5 | 1->3 |
| shorter | exact_name | `fix_point_intp` | `fix_point_intp` | 103 | 56 | 0.544 | -47 | 4->2 | 12->7 |
| shorter | exact_name | `tisp_set_ag_func` | `tisp_set_ag_func` | 102 | 80 | 0.784 | -22 | 6->6 | 5->0 |
| shorter | exact_name | `tisp_dmsc_fc_par_cfg` | `tisp_dmsc_fc_par_cfg` | 96 | 59 | 0.615 | -37 | 9->8 | 0->2 |
| shorter | exact_name | `tiziano_set_parameter_clm` | `tiziano_set_parameter_clm` | 93 | 62 | 0.667 | -31 | 6->6 | 4->4 |
| shorter | exact_name | `tisp_s_wb_mode` | `tisp_s_wb_mode` | 90 | 53 | 0.589 | -37 | 1->1 | 12->12 |
| shorter | exact_name | `tisp_deinit` | `tisp_deinit` | 86 | 62 | 0.721 | -24 | 9->7 | 7->5 |
| shorter | exact_name | `tisp_sharpen_param_array_get` | `tisp_sharpen_param_array_get` | 81 | 48 | 0.593 | -33 | 2->2 | 15->4 |
| shorter | exact_name | `ispcore_irq_thread_handle` | `ispcore_irq_thread_handle` | 73 | 57 | 0.781 | -16 | 2->2 | 10->6 |
| shorter | exact_name | `tiziano_lsc_lut_parameter` | `tiziano_lsc_lut_parameter` | 69 | 47 | 0.681 | -22 | 5->5 | 2->1 |
| shorter | exact_name | `tx_isp_release` | `tx_isp_release` | 65 | 47 | 0.723 | -18 | 2->2 | 15->9 |
| shorter | exact_name | `tisp_sharpen_v1_sigma_np_cfg` | `tisp_sharpen_v1_sigma_np_cfg` | 63 | 43 | 0.683 | -20 | 3->3 | 0->0 |
| shorter | exact_name | `tx_vic_disable_irq` | `tx_vic_disable_irq` | 63 | 46 | 0.730 | -17 | 4->3 | 10->4 |
| shorter | exact_name | `tisp_set_saturation` | `tisp_set_saturation` | 63 | 47 | 0.746 | -16 | 4->2 | 5->3 |
| shorter | exact_name | `tisp_dmsc_sp_d_sigma_3_np_cfg` | `tisp_dmsc_sp_d_sigma_3_np_cfg` | 62 | 42 | 0.677 | -20 | 3->3 | 0->0 |
| shorter | exact_name | `tisp_ccm_param_array_get` | `tisp_ccm_param_array_get` | 62 | 46 | 0.742 | -16 | 2->2 | 9->4 |
| shorter | exact_name | `ISPAWBInterpolation1` | `ISPAWBInterpolation1` | 60 | 45 | 0.750 | -15 | 4->2 | 1->0 |
| shorter | exact_name | `isp_irq_handle` | `isp_irq_handle` | 56 | 41 | 0.732 | -15 | 2->2 | 9->5 |
| shorter | exact_name | `ISPAWBInterpolation2` | `ISPAWBInterpolation2` | 55 | 43 | 0.782 | -12 | 4->4 | 1->2 |
| shorter | exact_name | `isp_i2c_new_subdev_board` | `isp_i2c_new_subdev_board` | 54 | 28 | 0.519 | -26 | 6->3 | 6->2 |
| shorter | exact_name | `tisp_event_init` | `tisp_event_init` | 53 | 41 | 0.774 | -12 | 1->1 | 2->2 |
| shorter | exact_name | `tx_isp_open` | `tx_isp_open` | 53 | 41 | 0.774 | -12 | 1->1 | 9->6 |
| shorter | exact_name | `tisp_dpc_s_par_cfg` | `tisp_dpc_s_par_cfg` | 52 | 39 | 0.750 | -13 | 4->3 | 0->1 |
| shorter | exact_name | `tiziano_gib_deir_reg` | `tiziano_gib_deir_reg` | 50 | 38 | 0.760 | -12 | 3->3 | 1->1 |
| shorter | exact_name | `ispcore_sensor_ops_ioctl` | `ispcore_sensor_ops_ioctl` | 50 | 39 | 0.780 | -11 | 1->1 | 8->6 |
| shorter | exact_name | `tx_vic_enable_irq` | `tx_vic_enable_irq` | 49 | 33 | 0.673 | -16 | 3->2 | 9->4 |
| shorter | exact_name | `vic_core_s_stream` | `vic_core_s_stream` | 49 | 37 | 0.755 | -12 | 1->1 | 15->7 |
| shorter | exact_name | `ispcore_sensor_ops_release_all_sensor` | `ispcore_sensor_ops_release_all_sensor` | 44 | 33 | 0.750 | -11 | 1->1 | 8->6 |
| shorter | exact_name | `tiziano_ccm_lut_parameter` | `tiziano_ccm_lut_parameter` | 43 | 27 | 0.628 | -16 | 1->1 | 3->3 |
| shorter | exact_name | `tiziano_gamma_lut_parameter` | `tiziano_gamma_lut_parameter` | 43 | 33 | 0.767 | -10 | 3->3 | 1->1 |
| shorter | exact_name | `tisp_channel_start` | `tisp_channel_start` | 41 | 30 | 0.732 | -11 | 4->4 | 4->3 |
| shorter | exact_name | `private_leading_one_position_64` | `private_leading_one_position_64` | 39 | 28 | 0.718 | -11 | 0->0 | 7->0 |
| shorter | exact_name | `vic_sensor_ops_sync_sensor_attr` | `vic_sensor_ops_sync_sensor_attr` | 39 | 29 | 0.744 | -10 | 2->2 | 11->5 |
| shorter | exact_name | `tisp_gib_deir_ir_update` | `tisp_gib_deir_ir_update` | 39 | 30 | 0.769 | -9 | 1->1 | 4->4 |
| shorter | exact_name | `fix_point_div_32` | `fix_point_div_32` | 38 | 29 | 0.763 | -9 | 1->0 | 6->6 |
| shorter | exact_name | `tx_isp_vic_remove` | `tx_isp_vic_remove` | 37 | 25 | 0.676 | -12 | 4->4 | 3->0 |
| shorter | exact_name | `fix_point_sub_64` | `fix_point_sub_64` | 34 | 20 | 0.588 | -14 | 1->1 | 3->2 |
| shorter | exact_name | `fix_point_sub` | `fix_point_sub` | 34 | 23 | 0.676 | -11 | 1->1 | 3->2 |
| shorter | exact_name | `tx_isp_vin_remove` | `tx_isp_vin_remove` | 33 | 26 | 0.788 | -7 | 4->4 | 2->0 |
| shorter | exact_name | `tisp_s_ae_attr` | `tisp_s_ae_attr` | 31 | 20 | 0.645 | -11 | 1->0 | 2->1 |
| shorter | exact_name | `isp_core_tuning_event` | `isp_core_tuning_event` | 30 | 21 | 0.700 | -9 | 1->1 | 4->4 |
| shorter | exact_name | `tisp_ae_get_y_zone` | `tisp_ae_get_y_zone` | 30 | 23 | 0.767 | -7 | 3->3 | 0->0 |
| shorter | exact_name | `fix_point_mult3` | `fix_point_mult3` | 29 | 23 | 0.793 | -6 | 1->1 | 1->1 |
| shorter | exact_name | `tisp_ae_get_hist_custome` | `tisp_ae_get_hist_custome` | 29 | 23 | 0.793 | -6 | 3->3 | 0->0 |
| shorter | exact_name | `tisp_ccm_ev_update` | `tisp_ccm_ev_update` | 28 | 16 | 0.571 | -12 | 1->1 | 1->2 |
| shorter | exact_name | `tisp_ccm_ct_update` | `tisp_ccm_ct_update` | 28 | 19 | 0.679 | -9 | 1->1 | 1->1 |
| shorter | exact_name | `system_reg_write_ae` | `system_reg_write_ae` | 28 | 22 | 0.786 | -6 | 1->1 | 4->4 |
| shorter | exact_name | `tisp_af_set_attr_refresh` | `tisp_af_set_attr_refresh` | 26 | 14 | 0.538 | -12 | 0->0 | 1->1 |
| shorter | exact_name | `tisp_sdns_par_refresh` | `tisp_sdns_par_refresh` | 26 | 17 | 0.654 | -9 | 1->0 | 3->3 |
| shorter | exact_name | `tiziano_ae_dn_params_refresh` | `tiziano_ae_dn_params_refresh` | 23 | 18 | 0.783 | -5 | 2->1 | 0->1 |
| shorter | exact_name | `private_kmalloc` | `private_kmalloc` | 22 | 4 | 0.182 | -18 | 2->0 | 1->1 |
| shorter | exact_name | `tisp_ct_update` | `tisp_ct_update` | 20 | 15 | 0.750 | -5 | 2->2 | 0->0 |
| shorter | exact_name | `tisp_s_wb_attr` | `tisp_s_wb_attr` | 19 | 4 | 0.211 | -15 | 1->0 | 0->1 |
| shorter | exact_name | `tiziano_af_dn_params_refresh` | `tiziano_af_dn_params_refresh` | 19 | 11 | 0.579 | -8 | 2->1 | 2->1 |
| shorter | exact_name | `sensor_get_normal_fps` | `sensor_get_normal_fps` | 19 | 13 | 0.684 | -6 | 0->0 | 0->0 |
| shorter | exact_name | `tiziano_awb_dn_params_refresh` | `tiziano_awb_dn_params_refresh` | 19 | 14 | 0.737 | -5 | 2->1 | 0->1 |
| shorter | exact_name | `private_request_module` | `private_request_module` | 19 | 15 | 0.789 | -4 | 1->1 | 0->0 |
| shorter | exact_name | `tisp_log2_fixed_to_fixed` | `tisp_log2_fixed_to_fixed` | 18 | 4 | 0.222 | -14 | 1->0 | 0->1 |
| shorter | exact_name | `private_seq_printf` | `private_seq_printf` | 18 | 12 | 0.667 | -6 | 1->1 | 0->0 |
| shorter | exact_name | `tisp_sharpen_intp_reg_refresh` | `tisp_sharpen_intp_reg_refresh` | 18 | 14 | 0.778 | -4 | 3->2 | 0->1 |
| shorter | exact_name | `tisp_s_af_attr` | `tisp_s_af_attr` | 17 | 4 | 0.235 | -13 | 1->0 | 0->1 |
| shorter | exact_name | `tisp_g_ae_min` | `tisp_g_ae_min` | 17 | 6 | 0.353 | -11 | 1->0 | 0->0 |
| shorter | exact_name | `tiziano_lsc_init` | `tiziano_lsc_init` | 17 | 6 | 0.353 | -11 | 2->0 | 0->0 |
| shorter | exact_name | `tiziano_ccm_dn_params_refresh` | `tiziano_ccm_dn_params_refresh` | 17 | 13 | 0.765 | -4 | 2->2 | 0->0 |
| shorter | exact_name | `netlink_rcv_msg` | `netlink_rcv_msg` | 16 | 4 | 0.250 | -12 | 0->0 | 4->0 |
| shorter | exact_name | `tisp_dmsc_sp_alias_par_cfg` | `tisp_dmsc_sp_alias_par_cfg` | 16 | 10 | 0.625 | -6 | 1->0 | 0->1 |
| shorter | exact_name | `tisp_mdns_refresh` | `tisp_mdns_refresh` | 14 | 4 | 0.286 | -10 | 1->0 | 0->1 |
| shorter | exact_name | `tisp_ae_process` | `tisp_ae_process` | 14 | 10 | 0.714 | -4 | 2->2 | 0->0 |
| shorter | exact_name | `tisp_s_ae_min` | `tisp_s_ae_min` | 14 | 11 | 0.786 | -3 | 1->1 | 0->0 |
| shorter | exact_name | `tiziano_adr_dn_params_refresh` | `tiziano_adr_dn_params_refresh` | 14 | 11 | 0.786 | -3 | 2->1 | 0->1 |
| shorter | exact_name | `tiziano_clm_dn_params_refresh` | `tiziano_clm_dn_params_refresh` | 14 | 11 | 0.786 | -3 | 2->1 | 0->1 |
| shorter | exact_name | `tiziano_clm_init` | `tiziano_clm_init` | 14 | 11 | 0.786 | -3 | 2->1 | 0->1 |
| shorter | exact_name | `tiziano_defog_dn_params_refresh` | `tiziano_defog_dn_params_refresh` | 14 | 11 | 0.786 | -3 | 2->1 | 0->1 |
| shorter | exact_name | `tiziano_gamma_dn_params_refresh` | `tiziano_gamma_dn_params_refresh` | 14 | 11 | 0.786 | -3 | 2->1 | 0->1 |
| shorter | exact_name | `tiziano_gamma_init` | `tiziano_gamma_init` | 14 | 11 | 0.786 | -3 | 2->1 | 0->1 |
| shorter | exact_name | `tisp_ae_manual_set` | `tisp_ae_manual_set` | 13 | 8 | 0.615 | -5 | 0->0 | 0->0 |
| shorter | exact_name | `jz_isp_ccm_para2reg` | `jz_isp_ccm_para2reg` | 12 | 6 | 0.500 | -6 | 0->0 | 2->1 |
| shorter | exact_name | `tisp_dpc_refresh` | `tisp_dpc_refresh` | 11 | 5 | 0.455 | -6 | 1->0 | 0->1 |
| shorter | exact_name | `tisp_sharpen_refresh` | `tisp_sharpen_refresh` | 11 | 5 | 0.455 | -6 | 1->0 | 0->1 |
| shorter | exact_name | `tisp_adr_ev_update` | `tisp_adr_ev_update` | 10 | 2 | 0.200 | -8 | 0->0 | 0->0 |
| shorter | exact_name | `tisp_defog_ev_update` | `tisp_defog_ev_update` | 10 | 2 | 0.200 | -8 | 0->0 | 0->0 |
| shorter | exact_name | `tisp_defog_process` | `tisp_defog_process` | 10 | 4 | 0.400 | -6 | 1->0 | 0->1 |
| shorter | exact_name | `tisp_g_wb_attr` | `tisp_g_wb_attr` | 10 | 4 | 0.400 | -6 | 1->0 | 0->1 |
| shorter | exact_name | `tisp_sdns_refresh` | `tisp_sdns_refresh` | 10 | 4 | 0.400 | -6 | 1->0 | 0->1 |
| shorter | exact_name | `tiziano_lsc_dn_params_refresh` | `tiziano_lsc_dn_params_refresh` | 10 | 4 | 0.400 | -6 | 1->0 | 0->1 |
| shorter | exact_name | `tisp_g_drc_strength` | `tisp_g_drc_strength` | 10 | 5 | 0.500 | -5 | 1->0 | 0->0 |
| shorter | exact_name | `tisp_day_or_night_g_ctrl` | `tisp_day_or_night_g_ctrl` | 10 | 7 | 0.700 | -3 | 0->0 | 1->0 |
| shorter | exact_name | `tiziano_ae_s_ev_start` | `tiziano_ae_s_ev_start` | 6 | 2 | 0.333 | -4 | 0->0 | 0->0 |
| shorter | exact_name | `tisp_netlink_event_set_cb` | `tisp_netlink_event_set_cb` | 4 | 2 | 0.500 | -2 | 0->0 | 0->0 |
| shorter | exact_name | `tisp_s_ev_start` | `tisp_s_ev_start` | 4 | 2 | 0.500 | -2 | 0->0 | 1->0 |
| expanded | exact_name | `init_module` | `init_module` | 18 | 2510 | 139.444 | +2492 | 2->2 | 1->1 |
| expanded | exact_name | `private_log2_fixed_to_fixed_64` | `private_log2_fixed_to_fixed_64` | 18 | 82 | 4.556 | +64 | 1->1 | 0->8 |
| expanded | exact_name | `tisp_log2_fixed_to_fixed_64` | `tisp_log2_fixed_to_fixed_64` | 18 | 81 | 4.500 | +63 | 1->1 | 0->8 |
| expanded | exact_name | `private_log2_fixed_to_fixed` | `private_log2_fixed_to_fixed` | 18 | 56 | 3.111 | +38 | 1->0 | 0->9 |
| expanded | exact_name | `isp_core_tuning_deinit` | `isp_core_tuning_deinit` | 7 | 14 | 2.000 | +7 | 0->1 | 2->1 |
| expanded | exact_name | `private_spin_unlock_irqrestore` | `private_spin_unlock_irqrestore` | 5 | 20 | 4.000 | +15 | 0->1 | 1->2 |
| expanded | exact_name | `private_misc_deregister` | `private_misc_deregister` | 5 | 11 | 2.200 | +6 | 0->1 | 1->0 |
| larger | exact_name | `tisp_sdns_param_array_set` | `tisp_sdns_param_array_set` | 134 | 192 | 1.433 | +58 | 4->3 | 32->33 |
| larger | exact_name | `tisp_adr_param_array_set` | `tisp_adr_param_array_set` | 127 | 169 | 1.331 | +42 | 4->3 | 28->29 |
| larger | exact_name | `tisp_defog_param_array_set` | `tisp_defog_param_array_set` | 116 | 154 | 1.328 | +38 | 4->3 | 25->26 |
| larger | exact_name | `tisp_set_fps` | `tisp_set_fps` | 53 | 69 | 1.302 | +16 | 3->3 | 3->3 |
| larger | exact_name | `private_log2_int_to_fixed` | `private_log2_int_to_fixed` | 48 | 62 | 1.292 | +14 | 1->0 | 7->9 |
| larger | exact_name | `fix_point_mult3_32` | `fix_point_mult3_32` | 20 | 30 | 1.500 | +10 | 1->0 | 1->0 |
| larger | exact_name | `tisp_hldc_par_refresh` | `tisp_hldc_par_refresh` | 19 | 25 | 1.316 | +6 | 2->2 | 1->2 |
| larger | exact_name | `private_init_completion` | `private_init_completion` | 5 | 9 | 1.800 | +4 | 0->0 | 1->1 |
| larger | exact_name | `private_gpio_free` | `private_gpio_free` | 5 | 8 | 1.600 | +3 | 0->0 | 1->2 |
| larger | exact_name | `tisp_awb_set_frz` | `tisp_awb_set_frz` | 4 | 6 | 1.500 | +2 | 0->0 | 0->0 |
| larger | exact_name | `tisp_s_wb_frz` | `tisp_s_wb_frz` | 4 | 6 | 1.500 | +2 | 0->0 | 1->0 |
| recovered_only | recovered_only |  | `__private_spin_lock_irqsave.constprop.0` | 0 | 16 | n/a | +0 | 0->1 | 0->1 |
| recovered_only | recovered_only |  | `apical_isp_ae_g_roi_isra_64` | 0 | 62 | n/a | +0 | 0->6 | 0->4 |
| recovered_only | recovered_only |  | `apical_isp_ae_s_roi_isra_45` | 0 | 67 | n/a | +0 | 0->5 | 0->8 |
| recovered_only | recovered_only |  | `apical_isp_ae_zone_g_ctrl_isra_69` | 0 | 19 | n/a | +0 | 0->2 | 0->0 |
| recovered_only | recovered_only |  | `apical_isp_ae_zone_weight_g_attr_isra_70` | 0 | 58 | n/a | +0 | 0->5 | 0->4 |
| recovered_only | recovered_only |  | `apical_isp_ae_zone_weight_s_attr_isra_50` | 0 | 71 | n/a | +0 | 0->6 | 0->7 |
| recovered_only | recovered_only |  | `apical_isp_af_hist_g_attr_isra_73` | 0 | 17 | n/a | +0 | 0->2 | 0->0 |
| recovered_only | recovered_only |  | `apical_isp_af_weight_g_attr_isra_74` | 0 | 59 | n/a | +0 | 0->5 | 0->4 |
| recovered_only | recovered_only |  | `apical_isp_af_weight_s_attr_isra_54` | 0 | 71 | n/a | +0 | 0->6 | 0->7 |
| recovered_only | recovered_only |  | `apical_isp_ev_g_attr_isra_62` | 0 | 32 | n/a | +0 | 0->3 | 0->2 |
| recovered_only | recovered_only |  | `apical_isp_gamma_g_attr_isra_63` | 0 | 41 | n/a | +0 | 0->3 | 0->3 |
| recovered_only | recovered_only |  | `apical_isp_gamma_s_attr_isra_44` | 0 | 20 | n/a | +0 | 0->1 | 0->2 |
| recovered_only | recovered_only |  | `apical_isp_max_again_g_ctrl_isra_60` | 0 | 26 | n/a | +0 | 0->2 | 0->2 |
| recovered_only | recovered_only |  | `apical_isp_max_dgain_g_ctrl_isra_61` | 0 | 26 | n/a | +0 | 0->2 | 0->2 |
| recovered_only | recovered_only |  | `apical_isp_wb_g_ctrl_isra_65` | 0 | 55 | n/a | +0 | 0->4 | 0->4 |
| recovered_only | recovered_only |  | `private_spin_unlock_irqrestore.constprop.0` | 0 | 20 | n/a | +0 | 0->1 | 0->2 |
| recovered_only | recovered_only |  | `regtrace_seq_printf` | 0 | 13 | n/a | +0 | 0->1 | 0->0 |
| recovered_only | recovered_only |  | `spin_lock` | 0 | 4 | n/a | +0 | 0->0 | 0->0 |
| recovered_only | recovered_only |  | `sub_6924` | 0 | 2 | n/a | +0 | 0->0 | 0->0 |
| recovered_only | recovered_only |  | `sub_692c` | 0 | 2 | n/a | +0 | 0->0 | 0->0 |
| recovered_only | recovered_only |  | `sub_6934` | 0 | 2 | n/a | +0 | 0->0 | 0->0 |
| recovered_only | recovered_only |  | `sub_69a0` | 0 | 2 | n/a | +0 | 0->0 | 0->0 |
| recovered_only | recovered_only |  | `sub_69bc` | 0 | 2 | n/a | +0 | 0->0 | 0->0 |
| recovered_only | recovered_only |  | `sub_69ec` | 0 | 2 | n/a | +0 | 0->0 | 0->0 |
| recovered_only | recovered_only |  | `sub_6b58` | 0 | 2 | n/a | +0 | 0->0 | 0->0 |
| recovered_only | recovered_only |  | `sub_6b60` | 0 | 2 | n/a | +0 | 0->0 | 0->0 |
| recovered_only | recovered_only |  | `sub_6b68` | 0 | 2 | n/a | +0 | 0->0 | 0->0 |
| recovered_only | recovered_only |  | `sub_6b70` | 0 | 2 | n/a | +0 | 0->0 | 0->0 |
| recovered_only | recovered_only |  | `sub_6b78` | 0 | 2 | n/a | +0 | 0->0 | 0->0 |
| recovered_only | recovered_only |  | `sub_6c20` | 0 | 2 | n/a | +0 | 0->0 | 0->0 |
| recovered_only | recovered_only |  | `sub_6c28` | 0 | 5 | n/a | +0 | 0->0 | 0->0 |
| recovered_only | recovered_only |  | `sub_6c38` | 0 | 2 | n/a | +0 | 0->0 | 0->0 |
| recovered_only | recovered_only |  | `sub_6c44` | 0 | 2 | n/a | +0 | 0->0 | 0->0 |
| recovered_only | recovered_only |  | `sub_6c4c` | 0 | 2 | n/a | +0 | 0->0 | 0->0 |
| recovered_only | recovered_only |  | `sub_6c54` | 0 | 2 | n/a | +0 | 0->0 | 0->0 |
| recovered_only | recovered_only |  | `sub_6c5c` | 0 | 2 | n/a | +0 | 0->0 | 0->0 |
| recovered_only | recovered_only |  | `sub_6c64` | 0 | 2 | n/a | +0 | 0->0 | 0->0 |
| recovered_only | recovered_only |  | `sub_6c6c` | 0 | 2 | n/a | +0 | 0->0 | 0->0 |
| recovered_only | recovered_only |  | `sub_6c74` | 0 | 2 | n/a | +0 | 0->0 | 0->0 |
| recovered_only | recovered_only |  | `sub_6c7c` | 0 | 2 | n/a | +0 | 0->0 | 0->0 |
| recovered_only | recovered_only |  | `sub_6c84` | 0 | 2 | n/a | +0 | 0->0 | 0->0 |
| recovered_only | recovered_only |  | `sub_6c8c` | 0 | 2 | n/a | +0 | 0->0 | 0->0 |
| recovered_only | recovered_only |  | `sub_6c94` | 0 | 2 | n/a | +0 | 0->0 | 0->0 |
| recovered_only | recovered_only |  | `sub_6c9c` | 0 | 2 | n/a | +0 | 0->0 | 0->0 |
| recovered_only | recovered_only |  | `sub_6ca4` | 0 | 2 | n/a | +0 | 0->0 | 0->0 |
| recovered_only | recovered_only |  | `sub_6cac` | 0 | 2 | n/a | +0 | 0->0 | 0->0 |
| recovered_only | recovered_only |  | `sub_6cb4` | 0 | 2 | n/a | +0 | 0->0 | 0->0 |
| recovered_only | recovered_only |  | `sub_6cbc` | 0 | 2 | n/a | +0 | 0->0 | 0->0 |
| recovered_only | recovered_only |  | `sub_6cc4` | 0 | 2 | n/a | +0 | 0->0 | 0->0 |
| recovered_only | recovered_only |  | `sub_6cd0` | 0 | 5 | n/a | +0 | 0->0 | 0->0 |
| recovered_only | recovered_only |  | `sub_6ce4` | 0 | 2 | n/a | +0 | 0->0 | 0->0 |
| recovered_only | recovered_only |  | `sub_6cec` | 0 | 2 | n/a | +0 | 0->0 | 0->0 |
| recovered_only | recovered_only |  | `sub_6cf4` | 0 | 2 | n/a | +0 | 0->0 | 0->0 |
| recovered_only | recovered_only |  | `sub_6cfc` | 0 | 2 | n/a | +0 | 0->0 | 0->0 |
| recovered_only | recovered_only |  | `sub_6d04` | 0 | 2 | n/a | +0 | 0->0 | 0->0 |
| recovered_only | recovered_only |  | `sub_70a0` | 0 | 2 | n/a | +0 | 0->0 | 0->0 |
| recovered_only | recovered_only |  | `sub_70b0` | 0 | 2 | n/a | +0 | 0->0 | 0->0 |
| recovered_only | recovered_only |  | `tisp_ccm_param_array_set.part.0` | 0 | 30 | n/a | +0 | 0->2 | 0->2 |
| recovered_only | recovered_only |  | `tisp_hldc_par_refresh_part_0` | 0 | 15 | n/a | +0 | 0->1 | 0->1 |
| recovered_only | recovered_only |  | `tx_isp_core_platform_device_platform_release` | 0 | 2 | n/a | +0 | 0->0 | 0->0 |
| recovered_only | recovered_only |  | `tx_isp_dispatch_sensor_ioctl` | 0 | 40 | n/a | +0 | 0->1 | 0->8 |
| recovered_only | recovered_only |  | `tx_isp_fs_platform_device_platform_release` | 0 | 2 | n/a | +0 | 0->0 | 0->0 |
| recovered_only | recovered_only |  | `tx_isp_irq_line_disable` | 0 | 10 | n/a | +0 | 0->1 | 0->0 |
| recovered_only | recovered_only |  | `tx_isp_irq_line_enable` | 0 | 10 | n/a | +0 | 0->1 | 0->0 |
| recovered_only | recovered_only |  | `tx_isp_platform_device_platform_release` | 0 | 2 | n/a | +0 | 0->0 | 0->0 |
| recovered_only | recovered_only |  | `tx_isp_vic_platform_device_platform_release` | 0 | 2 | n/a | +0 | 0->0 | 0->0 |
| recovered_only | recovered_only |  | `tx_isp_video_link_destroy_isra_1` | 0 | 72 | n/a | +0 | 0->4 | 0->12 |
| recovered_only | recovered_only |  | `tx_isp_vin_platform_device_platform_release` | 0 | 2 | n/a | +0 | 0->0 | 0->0 |
| recovered_only | recovered_only |  | `vic_core_ops_ioctl_1` | 0 | 25 | n/a | +0 | 0->1 | 0->4 |
| recovered_only | recovered_only |  | `video_input_strncmp` | 0 | 17 | n/a | +0 | 0->0 | 0->4 |
