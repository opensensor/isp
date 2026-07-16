# Binary Assembly Audit

- schema: `regtrace-binary-audit-v1`
- OEM: `../tx-isp-t23.ko`
- recovered: `driver/t23/tx_isp_t23_recovered.ko`
- objdump counts exclude relocation records
- thresholds: min_oem_insns=24 stub_insns=8 collapse=0.50 similar=0.80..1.25 expansion=2.00

## Summary

| Metric | OEM | Recovered |
|---|---:|---:|
| Function symbols | 1043 | 1322 |
| Functions with disassembly | 1043 | 1322 |
| Executable section bytes | 443376 | 357904 |
| Initialized writable bytes | 216768 | 46512 |
| Uninitialized writable bytes | 164688 | 1102000 |

- direct matches: 948
- replacement matches: 34 (missing=0)
- unmatched: OEM-only=61 recovered-only=340
- matched instructions: OEM=106302 recovered=71920 ratio=0.677
- classes: stub=55 collapsed=95 shorter=200 same_count=198 similar=365 larger=30 expanded=39

## Allocated Section Delta

| Section | OEM bytes | Recovered bytes | Delta |
|---|---:|---:|---:|
| `.MIPS.abiflags` | 24 | 24 | +0 |
| `.bss` | 164688 | 1102000 | +937312 |
| `.data` | 216480 | 46224 | -170256 |
| `.exit.text` | 1184 | 0 | -1184 |
| `.gnu.linkonce.this_module` | 288 | 288 | +0 |
| `.init.text` | 16 | 0 | -16 |
| `.modinfo` | 1260 | 5660 | +4400 |
| `.note.gnu.build-id` | 36 | 36 | +0 |
| `.reginfo` | 24 | 24 | +0 |
| `.rodata` | 6720 | 74656 | +67936 |
| `.rodata.str1.4` | 22728 | 20696 | -2032 |
| `.text` | 442176 | 356576 | -85600 |
| `.text.unlikely` | 0 | 1328 | +1328 |
| `__ksymtab` | 208 | 176 | -32 |
| `__ksymtab_strings` | 537 | 477 | -60 |
| `__param` | 224 | 2192 | +1968 |

## Function Outliers

Showing 250 of 820 outliers. JSON and CSV contain every comparison row.

| Class | Match | OEM symbol | Recovered symbol(s) | OEM insns | Recovered insns | Ratio | Delta | Calls | Branches |
|---|---|---|---|---:|---:|---:|---:|---:|---:|
| stub | exact_name | `Tiziano_adr_fpga` | `Tiziano_adr_fpga` | 3210 | 2 | 0.001 | -3208 | 148->0 | 263->0 |
| stub | exact_name | `tisp_defog_soft_process` | `tisp_defog_soft_process` | 1121 | 2 | 0.002 | -1119 | 22->0 | 134->0 |
| stub | exact_name | `frame_channel_unlocked_ioctl` | `frame_channel_unlocked_ioctl` | 899 | 2 | 0.002 | -897 | 56->0 | 171->0 |
| stub | exact_name | `tx_isp_vic_start` | `tx_isp_vic_start` | 606 | 5 | 0.008 | -601 | 13->0 | 75->1 |
| stub | exact_name | `ispcore_pad_event_handle` | `ispcore_pad_event_handle` | 548 | 4 | 0.007 | -544 | 26->0 | 87->1 |
| stub | exact_name | `ispcore_core_ops_init` | `ispcore_core_ops_init` | 501 | 8 | 0.016 | -493 | 18->0 | 129->2 |
| stub | exact_name | `defog_3x3_5x5_params_init` | `defog_3x3_5x5_params_init` | 483 | 2 | 0.004 | -481 | 34->0 | 37->0 |
| stub | exact_name | `dump_ivdc_regs` | `dump_ivdc_regs` | 433 | 2 | 0.005 | -431 | 46->0 | 1->0 |
| stub | exact_name | `tisp_ae_algo_handle` | `tisp_ae_algo_handle` | 322 | 2 | 0.006 | -320 | 40->0 | 8->0 |
| stub | exact_name | `video_input_cmd_set` | `video_input_cmd_set` | 308 | 3 | 0.010 | -305 | 20->0 | 50->0 |
| stub | exact_name | `tiziano_defog_params_refresh` | `tiziano_defog_params_refresh` | 279 | 2 | 0.007 | -277 | 43->0 | 3->0 |
| stub | exact_name | `tisp_ae1_process_impl` | `tisp_ae1_process_impl` | 273 | 2 | 0.007 | -271 | 11->0 | 26->0 |
| stub | exact_name | `tisp_msca_api_set_line` | `tisp_msca_api_set_line` | 273 | 2 | 0.007 | -271 | 22->0 | 23->0 |
| stub | exact_name | `tx_isp_probe` | `tx_isp_probe` | 235 | 2 | 0.009 | -233 | 23->0 | 25->0 |
| stub | exact_name | `tisp_sdns_wdr_en` | `tisp_sdns_wdr_en` | 231 | 2 | 0.009 | -229 | 0->0 | 3->0 |
| stub | exact_name | `csi_core_ops_init` | `csi_core_ops_init` | 188 | 5 | 0.027 | -183 | 6->0 | 22->1 |
| stub | exact_name | `tisp_cust_mode_s_ctrl` | `tisp_cust_mode_s_ctrl` | 161 | 6 | 0.037 | -155 | 22->0 | 9->0 |
| stub | exact_name | `subsection_map` | `subsection_map` | 155 | 3 | 0.019 | -152 | 4->0 | 15->0 |
| stub | exact_name | `tisp_msca_api_set_scaler_level_control` | `tisp_msca_api_set_scaler_level_control` | 131 | 2 | 0.015 | -129 | 7->0 | 7->0 |
| stub | exact_name | `tisp_msca_params_update` | `tisp_msca_params_update` | 130 | 2 | 0.015 | -128 | 11->0 | 4->0 |
| stub | exact_name | `tisp_event_process` | `tisp_event_process` | 129 | 2 | 0.016 | -127 | 8->0 | 6->0 |
| stub | exact_name | `tisp_hldc_calc_para` | `tisp_hldc_calc_para` | 127 | 2 | 0.016 | -125 | 1->0 | 5->0 |
| stub | exact_name | `ispcore_slake_module` | `ispcore_slake_module` | 114 | 2 | 0.018 | -112 | 6->0 | 18->0 |
| stub | exact_name | `dump_csi_reg` | `dump_csi_reg` | 100 | 2 | 0.020 | -98 | 14->0 | 1->0 |
| stub | exact_name | `private_log2_int_to_fixed_64` | `private_log2_int_to_fixed_64` | 96 | 2 | 0.021 | -94 | 4->0 | 8->0 |
| stub | exact_name | `tisp_set_fps` | `tisp_set_fps` | 94 | 2 | 0.021 | -92 | 5->0 | 3->0 |
| stub | exact_name | `tisp_lsc_upside_down_lut` | `tisp_lsc_upside_down_lut` | 87 | 2 | 0.023 | -85 | 5->0 | 3->0 |
| stub | exact_name | `isp_frame_done_wait` | `isp_frame_done_wait` | 82 | 4 | 0.049 | -78 | 3->0 | 8->0 |
| stub | exact_name | `ispcore_irq_fs_work` | `ispcore_irq_fs_work` | 79 | 6 | 0.076 | -73 | 1->0 | 12->0 |
| stub | exact_name | `tisp_defog_max_filter3` | `tisp_defog_max_filter3` | 78 | 2 | 0.026 | -76 | 0->0 | 14->1 |
| stub | exact_name | `isp_malloc_buffer` | `isp_malloc_buffer` | 72 | 2 | 0.028 | -70 | 4->0 | 10->0 |
| stub | exact_name | `tisp_stream_on` | `tisp_stream_on` | 71 | 2 | 0.028 | -69 | 6->0 | 1->0 |
| stub | exact_name | `tx_isp_notify` | `tx_isp_notify` | 69 | 2 | 0.029 | -67 | 1->0 | 12->0 |
| stub | exact_name | `__vb2_queue_cancel` | `__vb2_queue_cancel` | 67 | 2 | 0.030 | -65 | 5->0 | 5->0 |
| stub | exact_name | `tisp_log2_int_to_fixed` | `tisp_log2_int_to_fixed` | 65 | 2 | 0.031 | -63 | 0->0 | 14->0 |
| stub | exact_name | `tx_isp_video_link_stream` | `tx_isp_video_link_stream` | 64 | 2 | 0.031 | -62 | 2->0 | 12->0 |
| stub | exact_name | `ivdc_slake_module` | `ivdc_slake_module` | 63 | 6 | 0.095 | -57 | 4->0 | 9->1 |
| stub | exact_name | `subsection_up` | `subsection_up` | 61 | 3 | 0.049 | -58 | 0->0 | 9->0 |
| stub | exact_name | `tx_isp_subdev_deinit` | `tx_isp_subdev_deinit` | 59 | 5 | 0.085 | -54 | 8->0 | 6->1 |
| stub | exact_name | `vic_core_s_stream` | `vic_core_s_stream` | 50 | 5 | 0.100 | -45 | 3->0 | 8->1 |
| stub | exact_name | `ispcore_sensor_ops_ioctl` | `ispcore_sensor_ops_ioctl` | 46 | 2 | 0.043 | -44 | 1->0 | 8->0 |
| stub | exact_name | `tx_vic_disable_irq` | `tx_vic_disable_irq` | 45 | 2 | 0.044 | -43 | 3->0 | 6->0 |
| stub | exact_name | `fs_activate_module` | `fs_activate_module` | 43 | 2 | 0.047 | -41 | 1->0 | 7->0 |
| stub | exact_name | `tx_vic_enable_irq` | `tx_vic_enable_irq` | 42 | 2 | 0.048 | -40 | 3->0 | 5->0 |
| stub | exact_name | `tisp_s_BacklightComp` | `tisp_s_BacklightComp` | 41 | 4 | 0.098 | -37 | 4->0 | 0->1 |
| stub | exact_name | `isp_irq_thread_handle` | `isp_irq_thread_handle` | 39 | 2 | 0.051 | -37 | 2->0 | 7->0 |
| stub | exact_name | `tisp_s_module_control` | `tisp_s_module_control` | 37 | 2 | 0.054 | -35 | 3->0 | 1->0 |
| stub | exact_name | `ae1_interrupt_static` | `ae1_interrupt_static` | 36 | 2 | 0.056 | -34 | 3->0 | 0->0 |
| stub | exact_name | `tx_isp_vin_remove` | `tx_isp_vin_remove` | 35 | 4 | 0.114 | -31 | 4->0 | 1->1 |
| stub | exact_name | `__enqueue_in_driver` | `__enqueue_in_driver` | 32 | 5 | 0.156 | -27 | 2->0 | 2->0 |
| stub | exact_name | `tisp_g_module_control` | `tisp_g_module_control` | 31 | 2 | 0.065 | -29 | 2->0 | 1->0 |
| stub | exact_name | `tisp_g_wb_mode` | `tisp_g_wb_mode` | 29 | 2 | 0.069 | -27 | 1->0 | 1->0 |
| stub | exact_name | `tx_isp_frame_chan_deinit` | `tx_isp_frame_chan_deinit` | 29 | 2 | 0.069 | -27 | 3->0 | 2->0 |
| stub | exact_name | `tisp_s_awb_cluster` | `tisp_s_awb_cluster` | 26 | 4 | 0.154 | -22 | 1->0 | 2->1 |
| stub | exact_name | `csi_video_s_stream` | `csi_video_s_stream` | 26 | 5 | 0.192 | -21 | 1->0 | 5->1 |
| collapsed | exact_name | `tiziano_adr_algorithm` | `tiziano_adr_algorithm` | 3274 | 702 | 0.214 | -2572 | 1->0 | 207->83 |
| collapsed | exact_name | `Tiziano_Awb_Ct_Detect` | `Tiziano_Awb_Ct_Detect` | 1845 | 235 | 0.127 | -1610 | 35->6 | 197->25 |
| collapsed | exact_name | `ae0_tune2` | `ae0_tune2` | 1535 | 31 | 0.020 | -1504 | 91->2 | 172->1 |
| collapsed | exact_name | `apical_isp_core_ops_s_ctrl` | `apical_isp_core_ops_s_ctrl` | 1295 | 528 | 0.408 | -767 | 59->26 | 274->117 |
| collapsed | exact_name | `tiziano_defog_init` | `tiziano_defog_init` | 1181 | 426 | 0.361 | -755 | 127->49 | 53->15 |
| collapsed | exact_name | `tiziano_adr_params_init` | `tiziano_adr_params_init` | 1158 | 242 | 0.209 | -916 | 126->41 | 2->0 |
| collapsed | exact_name | `tiziano_adr_init` | `tiziano_adr_init` | 981 | 262 | 0.267 | -719 | 62->31 | 61->12 |
| collapsed | exact_name | `apical_isp_core_ops_g_ctrl` | `apical_isp_core_ops_g_ctrl` | 975 | 464 | 0.476 | -511 | 52->22 | 219->116 |
| collapsed | exact_name | `tisp_msca_ch_curve_write` | `tisp_msca_ch_curve_write` | 762 | 102 | 0.134 | -660 | 120->3 | 15->4 |
| collapsed | exact_name | `Tiziano_awb_fpga` | `Tiziano_awb_fpga` | 724 | 205 | 0.283 | -519 | 10->9 | 70->16 |
| collapsed | exact_name | `isp_vic_cmd_set` | `isp_vic_cmd_set` | 678 | 22 | 0.032 | -656 | 64->1 | 78->1 |
| collapsed | exact_name | `tisp_code_tuning_ioctl` | `tisp_code_tuning_ioctl` | 643 | 37 | 0.058 | -606 | 34->1 | 89->5 |
| collapsed | exact_name | `tisp_init` | `tisp_init` | 605 | 293 | 0.484 | -312 | 95->39 | 33->24 |
| collapsed | exact_name | `tiziano_bcsh_Tccm_RGBYUV` | `tiziano_bcsh_Tccm_RGBYUV` | 559 | 220 | 0.394 | -339 | 22->10 | 36->17 |
| collapsed | exact_name | `ispcore_interrupt_service_routine` | `ispcore_interrupt_service_routine` | 471 | 204 | 0.433 | -267 | 24->11 | 54->29 |
| collapsed | exact_name | `tiziano_load_parameters` | `tiziano_load_parameters` | 463 | 128 | 0.276 | -335 | 51->17 | 46->8 |
| collapsed | exact_name | `frame_chan_event` | `frame_chan_event` | 453 | 91 | 0.201 | -362 | 21->5 | 50->9 |
| collapsed | exact_name | `ivdc_pad_event_handle` | `ivdc_pad_event_handle` | 424 | 13 | 0.031 | -411 | 11->0 | 62->1 |
| collapsed | exact_name | `tisp_ae0_process_impl` | `tisp_ae0_process_impl` | 421 | 49 | 0.116 | -372 | 18->1 | 36->6 |
| collapsed | exact_name | `proc_ivdc_writel` | `proc_ivdc_writel` | 353 | 67 | 0.190 | -286 | 29->4 | 40->7 |
| collapsed | exact_name | `ae0_weight_mean2` | `ae0_weight_mean2` | 326 | 111 | 0.340 | -215 | 7->5 | 6->6 |
| collapsed | exact_name | `tisp_lsc_write_lut_datas` | `tisp_lsc_write_lut_datas` | 325 | 93 | 0.286 | -232 | 7->12 | 25->2 |
| collapsed | exact_name | `tisp_adr_param_array_set` | `tisp_adr_param_array_set` | 293 | 139 | 0.474 | -154 | 50->25 | 3->1 |
| collapsed | exact_name | `tisp_core_switch_bin` | `tisp_core_switch_bin` | 274 | 115 | 0.420 | -159 | 33->14 | 24->7 |
| collapsed | exact_name | `tiziano_ct_bcsh_interpolation` | `tiziano_ct_bcsh_interpolation` | 272 | 94 | 0.346 | -178 | 2->3 | 41->14 |
| collapsed | exact_name | `isp_framesource_show` | `isp_framesource_show` | 269 | 48 | 0.178 | -221 | 27->3 | 23->5 |
| collapsed | exact_name | `tiziano_ct_ccm_interpolation` | `tiziano_ct_ccm_interpolation` | 257 | 108 | 0.420 | -149 | 1->1 | 40->16 |
| collapsed | exact_name | `tiziano_ae_init` | `tiziano_ae_init` | 249 | 30 | 0.120 | -219 | 27->3 | 9->1 |
| collapsed | exact_name | `Tiziano_ae0_fpga` | `Tiziano_ae0_fpga` | 241 | 103 | 0.427 | -138 | 6->4 | 21->9 |
| collapsed | exact_name | `tx_isp_fs_probe` | `tx_isp_fs_probe` | 233 | 54 | 0.232 | -179 | 21->6 | 21->2 |
| collapsed | exact_name | `tisp_lsc_mirror_flip` | `tisp_lsc_mirror_flip` | 229 | 21 | 0.092 | -208 | 16->3 | 13->1 |
| collapsed | exact_name | `cm_control` | `cm_control` | 229 | 25 | 0.109 | -204 | 9->0 | 15->3 |
| collapsed | exact_name | `tisp_ae_manual_set` | `tisp_ae_manual_set` | 227 | 89 | 0.392 | -138 | 10->8 | 12->12 |
| collapsed | exact_name | `tisp_vic_ctrl_ioctl` | `tisp_vic_ctrl_ioctl` | 206 | 10 | 0.049 | -196 | 16->1 | 31->0 |
| collapsed | replacement | `tisp_sdns_gaussian_k_cfg` | `regtrace_t23_sdns_gaussian_k_cfg` | 202 | 16 | 0.079 | -186 | 64->1 | 0->1 |
| collapsed | exact_name | `tisp_s_osd_block_attr` | `tisp_s_osd_block_attr` | 168 | 59 | 0.351 | -109 | 3->1 | 10->1 |
| collapsed | exact_name | `tiziano_ae_init_exp_th` | `tiziano_ae_init_exp_th` | 166 | 30 | 0.181 | -136 | 4->1 | 6->2 |
| collapsed | exact_name | `ispcore_irq_thread_handle` | `ispcore_irq_thread_handle` | 164 | 34 | 0.207 | -130 | 9->2 | 25->5 |
| collapsed | exact_name | `tiziano_gib_deir_ir_interpolation` | `tiziano_gib_deir_ir_interpolation` | 164 | 34 | 0.207 | -130 | 6->0 | 20->8 |
| collapsed | exact_name | `tiziano_bcsh_Toffset_RGBYUV` | `tiziano_bcsh_Toffset_RGBYUV` | 154 | 56 | 0.364 | -98 | 9->1 | 9->4 |
| collapsed | exact_name | `subdev_sensor_ops_set_input` | `subdev_sensor_ops_set_input` | 147 | 10 | 0.068 | -137 | 10->0 | 29->3 |
| collapsed | exact_name | `tisp_msca_write_reg` | `tisp_msca_write_reg` | 143 | 48 | 0.336 | -95 | 4->4 | 16->5 |
| collapsed | exact_name | `printf_func0` | `printf_func0` | 139 | 55 | 0.396 | -84 | 12->3 | 14->4 |
| collapsed | replacement | `tisp_sdns_gaussian_x_cfg` | `regtrace_t23_sdns_gaussian_x_cfg` | 138 | 16 | 0.116 | -122 | 32->1 | 0->1 |
| collapsed | exact_name | `tx_isp_ivdc_probe` | `tx_isp_ivdc_probe` | 135 | 17 | 0.126 | -118 | 13->2 | 9->0 |
| collapsed | exact_name | `tx_isp_csi_probe` | `tx_isp_csi_probe` | 134 | 17 | 0.127 | -117 | 14->2 | 9->0 |
| collapsed | exact_name | `isp_core_cmd_set` | `isp_core_cmd_set` | 131 | 42 | 0.321 | -89 | 8->4 | 22->3 |
| collapsed | exact_name | `tiziano_adr_gamma_refresh` | `tiziano_adr_gamma_refresh` | 119 | 40 | 0.336 | -79 | 3->4 | 9->3 |
| collapsed | exact_name | `vic_sensor_ops_ioctl` | `vic_sensor_ops_ioctl` | 118 | 16 | 0.136 | -102 | 6->0 | 21->2 |
| collapsed | exact_name | `tisp_ae_target` | `tisp_ae_target` | 117 | 43 | 0.368 | -74 | 4->3 | 13->3 |
| collapsed | exact_name | `ispcore_core_ops_ioctl` | `ispcore_core_ops_ioctl` | 116 | 37 | 0.319 | -79 | 3->2 | 27->7 |
| collapsed | exact_name | `tisp_af_set_attr_refresh` | `tisp_af_set_attr_refresh` | 112 | 45 | 0.402 | -67 | 0->5 | 1->0 |
| collapsed | exact_name | `tisp_s_defog_str_internal` | `tisp_s_defog_str_internal` | 110 | 29 | 0.264 | -81 | 9->0 | 3->1 |
| collapsed | replacement | `tisp_sdns_r_s_mv_cfg` | `regtrace_t23_sdns_r_s_mv_cfg` | 106 | 38 | 0.358 | -68 | 16->3 | 0->3 |
| collapsed | exact_name | `tisp_ae_g_scene_luma` | `tisp_ae_g_scene_luma` | 106 | 42 | 0.396 | -64 | 4->0 | 4->5 |
| collapsed | exact_name | `fix_point_mult2_64` | `fix_point_mult2_64` | 104 | 41 | 0.394 | -63 | 5->5 | 0->0 |
| collapsed | exact_name | `tisp_lsc_lut_valid_judge` | `tisp_lsc_lut_valid_judge` | 102 | 24 | 0.235 | -78 | 2->2 | 14->1 |
| collapsed | exact_name | `tiziano_lsc_init` | `tiziano_lsc_init` | 101 | 23 | 0.228 | -78 | 8->2 | 5->1 |
| collapsed | exact_name | `frame_channel_vidioc_set_fmt` | `frame_channel_vidioc_set_fmt` | 100 | 23 | 0.230 | -77 | 7->3 | 17->3 |
| collapsed | exact_name | `tx_isp_vic_probe` | `tx_isp_vic_probe` | 96 | 17 | 0.177 | -79 | 11->2 | 4->0 |
| collapsed | exact_name | `tisp_s_wb_mode` | `tisp_s_wb_mode` | 90 | 24 | 0.267 | -66 | 1->1 | 12->1 |
| collapsed | exact_name | `tiziano_sharpen_init` | `tiziano_sharpen_init` | 85 | 29 | 0.341 | -56 | 2->2 | 2->2 |
| collapsed | exact_name | `tisp_defog_img_filter5` | `tisp_defog_img_filter5` | 85 | 40 | 0.471 | -45 | 0->0 | 5->4 |
| collapsed | exact_name | `tisp_defog_wdr_en` | `tisp_defog_wdr_en` | 81 | 13 | 0.160 | -68 | 2->1 | 3->1 |
| collapsed | replacement | `tisp_sdns_sp_d_b_wei_np_array_cfg` | `regtrace_t23_sdns_sp_d_b_wei_np_array_cfg` | 80 | 31 | 0.388 | -49 | 6->1 | 0->2 |
| collapsed | replacement | `tisp_sdns_sp_d_w_wei_np_array_cfg` | `regtrace_t23_sdns_sp_d_w_wei_np_array_cfg` | 80 | 31 | 0.388 | -49 | 6->1 | 0->2 |
| collapsed | replacement | `tisp_sdns_sp_ud_b_wei_np_array_cfg` | `regtrace_t23_sdns_sp_ud_b_wei_np_array_cfg` | 80 | 31 | 0.388 | -49 | 6->1 | 0->2 |
| collapsed | replacement | `tisp_sdns_sp_ud_w_wei_np_array_cfg` | `regtrace_t23_sdns_sp_ud_w_wei_np_array_cfg` | 80 | 31 | 0.388 | -49 | 6->1 | 0->2 |
| collapsed | exact_name | `tisp_msca_set_omi_api` | `tisp_msca_set_omi_api` | 79 | 37 | 0.468 | -42 | 9->5 | 5->1 |
| collapsed | exact_name | `tiziano_ae_dn_params_refresh` | `tiziano_ae_dn_params_refresh` | 73 | 29 | 0.397 | -44 | 7->3 | 0->1 |
| collapsed | exact_name | `frame_channel_vidioc_get_fmt` | `frame_channel_vidioc_get_fmt` | 69 | 30 | 0.435 | -39 | 5->2 | 7->2 |
| collapsed | exact_name | `tisp_sharpen_wdr_en` | `tisp_sharpen_wdr_en` | 67 | 13 | 0.194 | -54 | 0->0 | 2->2 |
| collapsed | exact_name | `isp_csi_show` | `isp_csi_show` | 64 | 12 | 0.188 | -52 | 4->1 | 9->0 |
| collapsed | replacement | `tisp_sdns_w_thres_cfg` | `regtrace_t23_sdns_w_thres_cfg` | 62 | 19 | 0.306 | -43 | 8->1 | 0->1 |
| collapsed | replacement | `tisp_sdns_sp_uu_np_array_cfg` | `regtrace_t23_sdns_sp_uu_np_array_cfg` | 62 | 25 | 0.403 | -37 | 4->1 | 0->1 |
| collapsed | exact_name | `tisp_ctr_md_np_cfg` | `tisp_ctr_md_np_cfg` | 62 | 26 | 0.419 | -36 | 4->1 | 0->1 |
| collapsed | exact_name | `tisp_ctr_std_np_cfg` | `tisp_ctr_std_np_cfg` | 62 | 26 | 0.419 | -36 | 4->1 | 0->1 |
| collapsed | exact_name | `tx_isp_csi_remove` | `tx_isp_csi_remove` | 61 | 10 | 0.164 | -51 | 6->1 | 1->0 |
| collapsed | exact_name | `sensor_set_mode` | `sensor_set_mode` | 60 | 13 | 0.217 | -47 | 2->1 | 4->0 |
| collapsed | replacement | `tisp_sdns_d_s1_thres_cfg` | `regtrace_t23_sdns_d_s1_thres_cfg` | 59 | 22 | 0.373 | -37 | 8->1 | 0->2 |
| collapsed | exact_name | `tisp_ae_s_comp` | `tisp_ae_s_comp` | 50 | 22 | 0.440 | -28 | 1->1 | 3->0 |
| collapsed | exact_name | `isp_pre_frame_dequeue` | `isp_pre_frame_dequeue` | 48 | 15 | 0.312 | -33 | 4->2 | 0->0 |
| collapsed | exact_name | `tisp_csccr_write_reg` | `tisp_csccr_write_reg` | 46 | 22 | 0.478 | -24 | 4->4 | 1->1 |
| collapsed | exact_name | `tx_isp_fs_remove` | `tx_isp_fs_remove` | 46 | 23 | 0.500 | -23 | 5->3 | 3->1 |
| collapsed | exact_name | `isp_ch1_frame_dequeue_delay` | `isp_ch1_frame_dequeue_delay` | 40 | 16 | 0.400 | -24 | 4->2 | 0->0 |
| collapsed | exact_name | `find_new_buffer` | `find_new_buffer` | 36 | 18 | 0.500 | -18 | 1->1 | 3->0 |
| collapsed | exact_name | `tisp_awb_algo_handle` | `tisp_awb_algo_handle` | 33 | 9 | 0.273 | -24 | 1->0 | 3->1 |
| collapsed | exact_name | `tisp_set_sensor_analog_gain_short` | `tisp_set_sensor_analog_gain_short` | 33 | 14 | 0.424 | -19 | 4->1 | 0->0 |
| collapsed | exact_name | `dump_vic_reg` | `dump_vic_reg` | 32 | 15 | 0.469 | -17 | 1->1 | 1->0 |
| collapsed | exact_name | `tx_isp_vin_activate_subdev` | `tx_isp_vin_activate_subdev` | 29 | 10 | 0.345 | -19 | 2->0 | 1->1 |
| collapsed | exact_name | `tisp_ae0_process` | `tisp_ae0_process` | 29 | 13 | 0.448 | -16 | 3->2 | 2->1 |
| collapsed | exact_name | `tx_isp_disable_irq` | `tx_isp_disable_irq` | 28 | 10 | 0.357 | -18 | 3->1 | 1->0 |
| collapsed | exact_name | `tx_isp_enable_irq` | `tx_isp_enable_irq` | 27 | 10 | 0.370 | -17 | 3->1 | 1->0 |
| collapsed | exact_name | `tisp_ae_trig` | `tisp_ae_trig` | 27 | 12 | 0.444 | -15 | 1->1 | 1->0 |
| collapsed | exact_name | `tiziano_gamma_init` | `tiziano_gamma_init` | 24 | 10 | 0.417 | -14 | 2->1 | 2->0 |
| oem_only | oem_only | `tx_isp_unlocked_ioctl` |  | 1198 | 0 | 0.000 | -1198 | 80->0 | 221->0 |
| oem_only | oem_only | `isp_info_show.isra.1` |  | 625 | 0 | 0.000 | -625 | 57->0 | 111->0 |
| oem_only | oem_only | `tx_isp_create_graph_and_nodes` |  | 162 | 0 | 0.000 | -162 | 10->0 | 21->0 |
| oem_only | oem_only | `apical_isp_af_hist_s_attr.isra.48` |  | 153 | 0 | 0.000 | -153 | 3->0 | 8->0 |
| oem_only | oem_only | `apical_isp_af_hist_g_attr.isra.98` |  | 124 | 0 | 0.000 | -124 | 2->0 | 0->0 |
| oem_only | oem_only | `apical_isp_autozoom_g_attr.isra.71` |  | 117 | 0 | 0.000 | -117 | 6->0 | 13->0 |
| oem_only | oem_only | `isp_core_tunning_unlocked_ioctl` |  | 97 | 0 | 0.000 | -97 | 8->0 | 17->0 |
| oem_only | oem_only | `ivdc_misc_unlocked_ioctl` |  | 97 | 0 | 0.000 | -97 | 6->0 | 18->0 |
| oem_only | oem_only | `ispcore_set_clk_parent.isra.0` |  | 93 | 0 | 0.000 | -93 | 3->0 | 10->0 |
| oem_only | oem_only | `apical_isp_ae_s_roi.isra.34` |  | 85 | 0 | 0.000 | -85 | 7->0 | 8->0 |
| oem_only | oem_only | `apical_isp_ae_zone_weight_s_attr.isra.45` |  | 85 | 0 | 0.000 | -85 | 7->0 | 8->0 |
| oem_only | oem_only | `apical_isp_af_weight_s_attr.isra.49` |  | 85 | 0 | 0.000 | -85 | 7->0 | 8->0 |
| oem_only | oem_only | `apical_isp_expr_s_ctrl.isra.33` |  | 80 | 0 | 0.000 | -80 | 4->0 | 13->0 |
| oem_only | oem_only | `apical_isp_ae_g_roi.isra.80` |  | 74 | 0 | 0.000 | -74 | 6->0 | 6->0 |
| oem_only | oem_only | `apical_isp_ae_zone_weight_g_attr.isra.92` |  | 74 | 0 | 0.000 | -74 | 6->0 | 6->0 |
| oem_only | oem_only | `apical_isp_af_weight_g_attr.isra.99` |  | 74 | 0 | 0.000 | -74 | 6->0 | 6->0 |
| oem_only | oem_only | `tx_isp_video_link_destroy.isra.9` |  | 74 | 0 | 0.000 | -74 | 4->0 | 9->0 |
| oem_only | oem_only | `apical_isp_autozoom_s_attr.isra.28` |  | 70 | 0 | 0.000 | -70 | 4->0 | 5->0 |
| oem_only | oem_only | `private_reset_tx_isp_module` |  | 50 | 0 | 0.000 | -50 | 1->0 | 4->0 |
| oem_only | oem_only | `apical_isp_ae_hist_origin_g_attr.isra.95` |  | 47 | 0 | 0.000 | -47 | 6->0 | 2->0 |
| oem_only | oem_only | `apical_isp_ev_g_attr.isra.78` |  | 47 | 0 | 0.000 | -47 | 3->0 | 2->0 |
| oem_only | oem_only | `tx_isp_module_init` |  | 47 | 0 | 0.000 | -47 | 3->0 | 4->0 |
| oem_only | oem_only | `apical_isp_awb_zone_statis_g_attr.isra.97` |  | 46 | 0 | 0.000 | -46 | 6->0 | 2->0 |
| oem_only | oem_only | `apical_isp_gamma_g_attr.isra.79` |  | 45 | 0 | 0.000 | -45 | 3->0 | 3->0 |
| oem_only | oem_only | `apical_isp_gamma_s_attr.isra.32` |  | 44 | 0 | 0.000 | -44 | 3->0 | 3->0 |
| oem_only | oem_only | `apical_isp_awb_zone_weight_g_attr.isra.115` |  | 42 | 0 | 0.000 | -42 | 4->0 | 2->0 |
| oem_only | oem_only | `apical_isp_awb_zone_weight_s_attr.isra.68` |  | 41 | 0 | 0.000 | -41 | 4->0 | 2->0 |
| oem_only | oem_only | `apical_isp_expr_g_ctrl.isra.75` |  | 31 | 0 | 0.000 | -31 | 2->0 | 0->0 |
| oem_only | oem_only | `apical_isp_max_again_g_ctrl.isra.76` |  | 31 | 0 | 0.000 | -31 | 2->0 | 2->0 |
| oem_only | oem_only | `apical_isp_max_dgain_g_ctrl.isra.77` |  | 31 | 0 | 0.000 | -31 | 2->0 | 2->0 |
| oem_only | oem_only | `tx_isp_reg_set` |  | 23 | 0 | 0.000 | -23 | 0->0 | 2->0 |
| oem_only | oem_only | `apical_isp_ae_zone_g_ctrl.isra.87` |  | 20 | 0 | 0.000 | -20 | 2->0 | 0->0 |
| oem_only | oem_only | `apical_isp_af_zone_g_ctrl.isra.88` |  | 20 | 0 | 0.000 | -20 | 2->0 | 0->0 |
| oem_only | oem_only | `tiziano_isp_ae_manual_attr_g_ctrl.isra.106` |  | 20 | 0 | 0.000 | -20 | 2->0 | 0->0 |
| oem_only | oem_only | `tiziano_bcsh_StrenCal.part.0` |  | 19 | 0 | 0.000 | -19 | 0->0 | 0->0 |
| oem_only | oem_only | `tx_isp_module_deinit` |  | 8 | 0 | 0.000 | -8 | 0->0 | 2->0 |
| oem_only | oem_only | `private_clk_is_enabled` |  | 5 | 0 | 0.000 | -5 | 0->0 | 1->0 |
| oem_only | oem_only | `private_dev_get_drvdata` |  | 5 | 0 | 0.000 | -5 | 0->0 | 1->0 |
| oem_only | oem_only | `private_dev_set_drvdata` |  | 5 | 0 | 0.000 | -5 | 0->0 | 1->0 |
| oem_only | oem_only | `private_free_irq` |  | 5 | 0 | 0.000 | -5 | 0->0 | 1->0 |
| oem_only | oem_only | `private_gpio_direction_input` |  | 5 | 0 | 0.000 | -5 | 0->0 | 1->0 |
| oem_only | oem_only | `private_gpio_set_debounce` |  | 5 | 0 | 0.000 | -5 | 0->0 | 1->0 |
| oem_only | oem_only | `private_i2c_register_driver` |  | 5 | 0 | 0.000 | -5 | 0->0 | 1->0 |
| oem_only | oem_only | `private_init_waitqueue_head` |  | 5 | 0 | 0.000 | -5 | 0->0 | 1->0 |
| oem_only | oem_only | `private_jz_proc_mkdir` |  | 5 | 0 | 0.000 | -5 | 0->0 | 1->0 |
| oem_only | oem_only | `private_jzgpio_ctrl_pull` |  | 5 | 0 | 0.000 | -5 | 0->0 | 1->0 |
| oem_only | oem_only | `private_kthread_run` |  | 5 | 0 | 0.000 | -5 | 0->0 | 1->0 |
| oem_only | oem_only | `private_kthread_should_stop` |  | 5 | 0 | 0.000 | -5 | 0->0 | 1->0 |
| oem_only | oem_only | `private_kthread_stop` |  | 5 | 0 | 0.000 | -5 | 0->0 | 1->0 |
| oem_only | oem_only | `private_misc_register` |  | 5 | 0 | 0.000 | -5 | 0->0 | 1->0 |
| oem_only | oem_only | `private_proc_create_data` |  | 5 | 0 | 0.000 | -5 | 0->0 | 1->0 |
| oem_only | oem_only | `private_sched_clock` |  | 5 | 0 | 0.000 | -5 | 0->0 | 1->0 |
| oem_only | oem_only | `private_seq_read` |  | 5 | 0 | 0.000 | -5 | 0->0 | 1->0 |
| oem_only | oem_only | `private_simple_strtoull` |  | 5 | 0 | 0.000 | -5 | 0->0 | 1->0 |
| oem_only | oem_only | `private_wait_event_interruptible` |  | 5 | 0 | 0.000 | -5 | 0->0 | 1->0 |
| oem_only | oem_only | `private_wait_for_completion_interruptible` |  | 5 | 0 | 0.000 | -5 | 0->0 | 1->0 |
| oem_only | oem_only | `private_wait_for_completion_timeout` |  | 5 | 0 | 0.000 | -5 | 0->0 | 1->0 |
| oem_only | oem_only | `private_wake_up` |  | 5 | 0 | 0.000 | -5 | 0->0 | 1->0 |
| oem_only | oem_only | `private_wake_up_all` |  | 5 | 0 | 0.000 | -5 | 0->0 | 1->0 |
| oem_only | oem_only | `tisp_hldc_par_refresh.part.1` |  | 5 | 0 | 0.000 | -5 | 0->0 | 1->0 |
| oem_only | oem_only | `tx_isp_module_exit` |  | 4 | 0 | 0.000 | -4 | 0->0 | 1->0 |
| shorter | exact_name | `tisp_mdns_intp` | `tisp_mdns_intp` | 2659 | 1810 | 0.681 | -849 | 377->300 | 0->0 |
| shorter | exact_name | `tiziano_defog_params_init` | `tiziano_defog_params_init` | 1229 | 639 | 0.520 | -590 | 94->128 | 0->6 |
| shorter | exact_name | `JZ_Isp_Awb` | `JZ_Isp_Awb` | 483 | 249 | 0.516 | -234 | 6->5 | 57->21 |
| shorter | exact_name | `tiziano_bcsh_update` | `tiziano_bcsh_update` | 455 | 270 | 0.593 | -185 | 6->14 | 26->15 |
| shorter | replacement | `tisp_mdns_c_2d_param_cfg` | `regtrace_t23_mdns_c_2d_param_cfg.isra.0` | 422 | 334 | 0.791 | -88 | 31->30 | 2->1 |
| shorter | exact_name | `tiziano_bcsh_lut_parameter` | `tiziano_bcsh_lut_parameter` | 373 | 238 | 0.638 | -135 | 29->29 | 13->4 |
| shorter | exact_name | `subsection` | `subsection` | 372 | 237 | 0.637 | -135 | 20->16 | 17->13 |
| shorter | exact_name | `tisp_ae1_expt` | `tisp_ae1_expt` | 351 | 183 | 0.521 | -168 | 23->21 | 30->14 |
| shorter | exact_name | `tisp_defog_param_array_set` | `tisp_defog_param_array_set` | 260 | 203 | 0.781 | -57 | 46->38 | 1->1 |
| shorter | exact_name | `tisp_clm_interp_by_ct` | `tisp_clm_interp_by_ct` | 254 | 145 | 0.571 | -109 | 8->6 | 35->17 |
| shorter | exact_name | `tiziano_sdns_init` | `tiziano_sdns_init` | 242 | 157 | 0.649 | -85 | 2->4 | 2->2 |
| shorter | exact_name | `Tiziano_af_fpga` | `Tiziano_af_fpga` | 229 | 156 | 0.681 | -73 | 13->11 | 5->5 |
| shorter | exact_name | `tisp_ae0_get_hist` | `tisp_ae0_get_hist` | 206 | 158 | 0.767 | -48 | 5->5 | 22->17 |
| shorter | exact_name | `tiziano_ae_set_hardware_param` | `tiziano_ae_set_hardware_param` | 206 | 162 | 0.786 | -44 | 19->19 | 7->5 |
| shorter | exact_name | `tisp_mdns_set_malloc_cfg` | `tisp_mdns_set_malloc_cfg` | 200 | 145 | 0.725 | -55 | 32->32 | 4->4 |
| shorter | exact_name | `tisp_msca_init_chx_cfg` | `tisp_msca_init_chx_cfg` | 199 | 147 | 0.739 | -52 | 17->26 | 8->9 |
| shorter | exact_name | `tisp_msca_normalized` | `tisp_msca_normalized` | 196 | 138 | 0.704 | -58 | 4->1 | 22->15 |
| shorter | exact_name | `tisp_af_param_array_get` | `tisp_af_param_array_get` | 169 | 114 | 0.675 | -55 | 19->19 | 0->0 |
| shorter | exact_name | `tisp_s_adr_str_internal` | `tisp_s_adr_str_internal` | 146 | 75 | 0.514 | -71 | 1->0 | 7->6 |
| shorter | exact_name | `tisp_log2_int_to_fixed_64` | `tisp_log2_int_to_fixed_64` | 129 | 94 | 0.729 | -35 | 3->2 | 16->9 |
| shorter | exact_name | `tisp_bcsh_set_attr` | `tisp_bcsh_set_attr` | 128 | 92 | 0.719 | -36 | 17->11 | 4->5 |
| shorter | exact_name | `jz_isp_ccm` | `jz_isp_ccm` | 128 | 99 | 0.773 | -29 | 5->5 | 13->10 |
| shorter | exact_name | `tiziano_af_params_refresh` | `tiziano_af_params_refresh` | 124 | 93 | 0.750 | -31 | 19->19 | 0->0 |
| shorter | exact_name | `tisp_msca_api_set_mask` | `tisp_msca_api_set_mask` | 123 | 75 | 0.610 | -48 | 8->8 | 4->2 |
| shorter | exact_name | `tisp_hldc_set_attr` | `tisp_hldc_set_attr` | 123 | 76 | 0.618 | -47 | 8->6 | 2->2 |
| shorter | exact_name | `tisp_vic_mmap` | `tisp_vic_mmap` | 123 | 95 | 0.772 | -28 | 9->8 | 14->8 |
| shorter | exact_name | `tisp_ae0_ctrls_update` | `tisp_ae0_ctrls_update` | 123 | 97 | 0.789 | -26 | 1->1 | 12->8 |
| shorter | exact_name | `tisp_msca_api_set_osd` | `tisp_msca_api_set_osd` | 117 | 69 | 0.590 | -48 | 14->14 | 2->0 |
| shorter | exact_name | `tisp_g_ev_attr` | `tisp_g_ev_attr` | 117 | 78 | 0.667 | -39 | 9->2 | 0->0 |
| shorter | exact_name | `tisp_set_ae0_ag` | `tisp_set_ae0_ag` | 113 | 82 | 0.726 | -31 | 6->6 | 7->3 |
| shorter | exact_name | `fix_point_intp` | `fix_point_intp` | 102 | 54 | 0.529 | -48 | 4->4 | 12->4 |
| shorter | exact_name | `vic_mdma_enable` | `vic_mdma_enable` | 100 | 79 | 0.790 | -21 | 0->0 | 6->3 |
| shorter | exact_name | `tisp_ae_algo_init` | `tisp_ae_algo_init` | 99 | 65 | 0.657 | -34 | 2->2 | 2->2 |
| shorter | exact_name | `Tiziano_awb_set_gain` | `Tiziano_awb_set_gain` | 99 | 66 | 0.667 | -33 | 7->6 | 4->1 |
| shorter | exact_name | `fix_point_div_64` | `fix_point_div_64` | 94 | 51 | 0.543 | -43 | 3->3 | 9->3 |
| shorter | exact_name | `tiziano_set_parameter_clm` | `tiziano_set_parameter_clm` | 91 | 49 | 0.538 | -42 | 6->6 | 4->1 |
| shorter | exact_name | `ispcore_sync_sensor_attr` | `ispcore_sync_sensor_attr` | 91 | 60 | 0.659 | -31 | 4->4 | 7->7 |
| shorter | exact_name | `tisp_event_push` | `tisp_event_push` | 88 | 63 | 0.716 | -25 | 4->1 | 2->1 |
| shorter | exact_name | `tisp_msca_para_calc` | `tisp_msca_para_calc` | 86 | 62 | 0.721 | -24 | 0->6 | 17->10 |

## Replacement Map

| OEM symbol | Recovered symbol(s) |
|---|---|
| `tisp_mdns_c_2d_param_cfg` | `regtrace_t23_mdns_c_2d_param_cfg.isra.0` |
| `tisp_mdns_c_3d_param_cfg` | `regtrace_t23_mdns_c_3d_param_cfg.isra.0` |
| `tisp_mdns_y_2d_param_cfg` | `regtrace_t23_mdns_y_2d_param_cfg.isra.0` |
| `tisp_mdns_y_3d_param_cfg` | `regtrace_t23_mdns_y_3d_param_cfg.isra.0` |
| `tisp_sdns_d_s1_thres_cfg` | `regtrace_t23_sdns_d_s1_thres_cfg` |
| `tisp_sdns_dark_light_tt_opt_cfg` | `regtrace_t23_sdns_dark_light_tt_opt_cfg` |
| `tisp_sdns_g_det_val_div_cfg` | `regtrace_t23_sdns_g_det_val_div_cfg` |
| `tisp_sdns_gaussian_k_cfg` | `regtrace_t23_sdns_gaussian_k_cfg` |
| `tisp_sdns_gaussian_x_cfg` | `regtrace_t23_sdns_gaussian_x_cfg` |
| `tisp_sdns_gaussian_y_cfg` | `regtrace_t23_sdns_gaussian_y_cfg` |
| `tisp_sdns_grad_thres_opt_cfg` | `regtrace_t23_sdns_grad_thres_opt_cfg` |
| `tisp_sdns_h_line_cfg` | `regtrace_t23_sdns_h_line_cfg` |
| `tisp_sdns_h_mv_cfg` | `regtrace_t23_sdns_h_mv_cfg` |
| `tisp_sdns_h_mv_wei_opt_cfg` | `regtrace_t23_sdns_h_mv_wei_opt_cfg` |
| `tisp_sdns_h_s_cfg` | `regtrace_t23_sdns_h_s_cfg` |
| `tisp_sdns_hls_en_ave_filter_cfg` | `regtrace_t23_sdns_hls_en_ave_filter_cfg` |
| `tisp_sdns_mv_seg_number_num_thres_cfg` | `regtrace_t23_sdns_mv_seg_number_num_thres_cfg` |
| `tisp_sdns_r_s_mv_cfg` | `regtrace_t23_sdns_r_s_mv_cfg` |
| `tisp_sdns_sp_d_b_wei_np_array_cfg` | `regtrace_t23_sdns_sp_d_b_wei_np_array_cfg` |
| `tisp_sdns_sp_d_w_wei_np_array_cfg` | `regtrace_t23_sdns_sp_d_w_wei_np_array_cfg` |
| `tisp_sdns_sp_std_en_seg_opt_cfg` | `regtrace_t23_sdns_sp_std_en_seg_opt_cfg` |
| `tisp_sdns_sp_ud_b_limit_srd_ll_hl_flat_cfg` | `regtrace_t23_sdns_sp_ud_b_limit_srd_ll_hl_flat_cfg` |
| `tisp_sdns_sp_ud_b_stren_cfg` | `regtrace_t23_sdns_sp_ud_b_stren_cfg` |
| `tisp_sdns_sp_ud_b_wei_np_array_cfg` | `regtrace_t23_sdns_sp_ud_b_wei_np_array_cfg` |
| `tisp_sdns_sp_ud_stren_shift_opt_cfg` | `regtrace_t23_sdns_sp_ud_stren_shift_opt_cfg` |
| `tisp_sdns_sp_ud_v2_v1_coef_w_wei_opt_cfg` | `regtrace_t23_sdns_sp_ud_v2_v1_coef_w_wei_opt_cfg` |
| `tisp_sdns_sp_ud_w_limit_b_wei_opt_cfg` | `regtrace_t23_sdns_sp_ud_w_limit_b_wei_opt_cfg` |
| `tisp_sdns_sp_ud_w_stren_cfg` | `regtrace_t23_sdns_sp_ud_w_stren_cfg` |
| `tisp_sdns_sp_ud_w_wei_np_array_cfg` | `regtrace_t23_sdns_sp_ud_w_wei_np_array_cfg` |
| `tisp_sdns_sp_uu_cfg` | `regtrace_t23_sdns_sp_uu_cfg` |
| `tisp_sdns_sp_uu_np_array_cfg` | `regtrace_t23_sdns_sp_uu_np_array_cfg` |
| `tisp_sdns_sp_v2_d_w_b_ll_hl_flat_cfg` | `regtrace_t23_sdns_sp_v2_d_w_b_ll_hl_flat_cfg` |
| `tisp_sdns_w_thres_cfg` | `regtrace_t23_sdns_w_thres_cfg` |
| `tiziano_sdns_params_refresh` | `regtrace_t23_source_sdns_load_tuning` |
