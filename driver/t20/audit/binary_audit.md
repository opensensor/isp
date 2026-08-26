# Binary Assembly Audit

- schema: `regtrace-binary-audit-v1`
- OEM: `/home/matteius/re-framework/tx-isp-t20.ko`
- recovered: `driver/t20/tx-isp-t20.ko`
- objdump counts exclude relocation records
- thresholds: min_oem_insns=24 stub_insns=8 collapse=0.50 similar=0.80..1.25 expansion=2.00

## Summary

| Metric | OEM | Recovered |
|---|---:|---:|
| Function symbols | 742 | 860 |
| Functions with disassembly | 742 | 860 |
| Executable section bytes | 205504 | 457948 |
| Initialized writable bytes | 43624 | 90968 |
| Uninitialized writable bytes | 34116 | 38128 |

- direct matches: 711
- replacement matches: 10 (missing=0)
- unmatched: OEM-only=21 recovered-only=117
- matched instructions: OEM=49162 recovered=100592 ratio=2.046
- classes: stub=0 collapsed=0 shorter=8 same_count=3 similar=37 larger=281 expanded=392

## Allocated Section Delta

| Section | OEM bytes | Recovered bytes | Delta |
|---|---:|---:|---:|
| `.MIPS.abiflags` | 24 | 24 | +0 |
| `.bss` | 34016 | 38128 | +4112 |
| `.data` | 43312 | 90656 | +47344 |
| `.exit.text` | 236 | 300 | +64 |
| `.gnu.linkonce.this_module` | 288 | 288 | +0 |
| `.init.text` | 76 | 48 | -28 |
| `.modinfo` | 176 | 228 | +52 |
| `.note.gnu.build-id` | 36 | 36 | +0 |
| `.reginfo` | 24 | 24 | +0 |
| `.rodata` | 10448 | 20800 | +10352 |
| `.rodata.cst16` | 16 | 0 | -16 |
| `.rodata.cst4` | 12 | 0 | -12 |
| `.rodata.cst8` | 8 | 0 | -8 |
| `.rodata.str1.4` | 12308 | 0 | -12308 |
| `.sbss` | 100 | 0 | -100 |
| `.text` | 203680 | 457600 | +253920 |
| `.text.unlikely` | 1512 | 0 | -1512 |
| `__ksymtab` | 48 | 48 | +0 |
| `__ksymtab_strings` | 130 | 130 | +0 |
| `__param` | 16 | 32 | +16 |
| `__verbose` | 24 | 24 | +0 |

## Function Outliers

Showing 250 of 819 outliers. JSON and CSV contain every comparison row.

| Class | Match | OEM symbol | Recovered symbol(s) | OEM insns | Recovered insns | Ratio | Delta | Calls | Branches |
|---|---|---|---|---:|---:|---:|---:|---:|---:|
| oem_only | oem_only | `apical_isp_stab_s_attr.isra.0` |  | 288 | 0 | 0.000 | -288 | 2->0 | 53->0 |
| oem_only | oem_only | `isp_core_frame_channel_streamoff.isra.0` |  | 232 | 0 | 0.000 | -232 | 16->0 | 26->0 |
| oem_only | oem_only | `ae_roi.part.1` |  | 164 | 0 | 0.000 | -164 | 5->0 | 14->0 |
| oem_only | oem_only | `awb_roi.part.2` |  | 164 | 0 | 0.000 | -164 | 5->0 | 14->0 |
| oem_only | oem_only | `AWB_mesh.isra.1` |  | 160 | 0 | 0.000 | -160 | 20->0 | 5->0 |
| oem_only | oem_only | `isp_configure_base_addr.isra.0` |  | 156 | 0 | 0.000 | -156 | 4->0 | 14->0 |
| oem_only | oem_only | `apical_isp_af_hist_s_attr.isra.0` |  | 155 | 0 | 0.000 | -155 | 28->0 | 1->0 |
| oem_only | oem_only | `apical_program_interrupt_event.part.0` |  | 145 | 0 | 0.000 | -145 | 16->0 | 18->0 |
| oem_only | oem_only | `apical_isp_ae_weight_s_attr.isra.0` |  | 73 | 0 | 0.000 | -73 | 4->0 | 3->0 |
| oem_only | oem_only | `apical_isp_awb_weight_s_attr.isra.0` |  | 73 | 0 | 0.000 | -73 | 4->0 | 3->0 |
| oem_only | oem_only | `isp_modify_dma_direction.isra.0` |  | 72 | 0 | 0.000 | -72 | 3->0 | 9->0 |
| oem_only | oem_only | `apical_isp_ae_weight_g_attr.isra.0` |  | 63 | 0 | 0.000 | -63 | 3->0 | 3->0 |
| oem_only | oem_only | `apical_isp_awb_weight_g_attr.isra.0` |  | 63 | 0 | 0.000 | -63 | 3->0 | 3->0 |
| oem_only | oem_only | `subdev_core_ops_streamoff.constprop.0` |  | 57 | 0 | 0.000 | -57 | 3->0 | 10->0 |
| oem_only | oem_only | `apical_isp_gamma_s_attr.isra.0` |  | 53 | 0 | 0.000 | -53 | 4->0 | 4->0 |
| oem_only | oem_only | `apical_isp_gamma_g_attr.isra.0` |  | 47 | 0 | 0.000 | -47 | 3->0 | 2->0 |
| oem_only | oem_only | `apical_isp_stab_g_attr.isra.0` |  | 41 | 0 | 0.000 | -41 | 4->0 | 1->0 |
| oem_only | oem_only | `isp_enable_dma_transfer.isra.0` |  | 41 | 0 | 0.000 | -41 | 2->0 | 7->0 |
| oem_only | oem_only | `isp_core_release_clk.isra.0` |  | 39 | 0 | 0.000 | -39 | 1->0 | 4->0 |
| oem_only | oem_only | `mipi_csih_dphy_write.part.0.constprop.0` |  | 36 | 0 | 0.000 | -36 | 3->0 | 1->0 |
| oem_only | oem_only | `mipi_csih_dphy_test_clock.isra.0` |  | 8 | 0 | 0.000 | -8 | 0->0 | 0->0 |
| shorter | exact_name | `tx_isp_video_in_subdev_close` | `tx_isp_video_in_subdev_close` | 113 | 79 | 0.699 | -34 | 6->4 | 13->5 |
| shorter | exact_name | `frame_buffer_manager_cleanup` | `frame_buffer_manager_cleanup` | 39 | 31 | 0.795 | -8 | 2->2 | 7->2 |
| shorter | exact_name | `apical_interrupt_ae_stats` | `apical_interrupt_ae_stats` | 27 | 19 | 0.704 | -8 | 1->0 | 1->0 |
| shorter | exact_name | `apical_interrupt_frame_buffer_ds2` | `apical_interrupt_frame_buffer_ds2` | 27 | 19 | 0.704 | -8 | 1->0 | 1->0 |
| shorter | exact_name | `apical_interrupt_frame_buffer_fr` | `apical_interrupt_frame_buffer_fr` | 27 | 19 | 0.704 | -8 | 1->0 | 1->0 |
| shorter | exact_name | `apical_fw_process` | `apical_fw_process` | 25 | 15 | 0.600 | -10 | 1->1 | 3->0 |
| shorter | exact_name | `init_module` | `init_module` | 19 | 12 | 0.632 | -7 | 2->1 | 1->0 |
| shorter | exact_name | `tx_isp_init` | `tx_isp_init` | 19 | 12 | 0.632 | -7 | 2->1 | 1->0 |
| expanded | exact_name | `apical_isp_day_or_night_s_ctrl_internal` | `apical_isp_day_or_night_s_ctrl_internal` | 1745 | 4677 | 2.680 | +2932 | 224->227 | 18->18 |
| expanded | exact_name | `awb_calc_avg_weighted_gr_gb_mesh` | `awb_calc_avg_weighted_gr_gb_mesh` | 1353 | 3207 | 2.370 | +1854 | 104->99 | 140->138 |
| expanded | exact_name | `apical_command` | `apical_command` | 1300 | 3119 | 2.399 | +1819 | 1->131 | 152->409 |
| expanded | replacement | `isp_core_ops_ioctl` | `isp_core_frame_channel_crop_capture`<br>`isp_core_frame_channel_enum_fmt`<br>`isp_core_frame_channel_queue_buffer`<br>`isp_core_frame_channel_scaler_capture`<br>`isp_core_frame_channel_set_crop`<br>`isp_core_frame_channel_set_fmt`<br>`isp_core_frame_channel_set_scaler`<br>`isp_core_frame_channel_streamoff`<br>`isp_core_frame_channel_streamon`<br>`isp_core_frame_channel_try_fmt`<br>`isp_core_ops_ioctl`<br>`isp_core_ops_private_ioctl`<br>`isp_core_set_clk` | 1003 | 2549 | 2.541 | +1546 | 72->93 | 139->187 |
| expanded | exact_name | `isp_info_show` | `isp_info_show` | 799 | 1780 | 2.228 | +981 | 144->144 | 10->24 |
| expanded | exact_name | `isp_core_interrupt_service_routine` | `isp_core_interrupt_service_routine` | 677 | 1388 | 2.050 | +711 | 59->60 | 72->93 |
| expanded | exact_name | `apical_custom_initialization` | `apical_custom_initialization` | 559 | 1403 | 2.510 | +844 | 113->116 | 9->9 |
| expanded | exact_name | `matrix_yuv_coefft_write_to_hardware` | `matrix_yuv_coefft_write_to_hardware` | 466 | 1215 | 2.607 | +749 | 119->132 | 16->12 |
| expanded | replacement | `video_in_core_ops_ioctl` | `subdev_core_ops_enum_input`<br>`subdev_core_ops_get_input`<br>`subdev_core_ops_register_sensor`<br>`subdev_core_ops_release_all_sensor`<br>`subdev_core_ops_release_sensor`<br>`subdev_core_ops_set_input`<br>`subdev_core_ops_streamoff`<br>`subdev_core_ops_streamon`<br>`video_in_core_ops_ioctl` | 423 | 1723 | 4.073 | +1300 | 22->55 | 73->174 |
| expanded | exact_name | `sharpening_update` | `sharpening_update` | 404 | 838 | 2.074 | +434 | 35->36 | 35->54 |
| expanded | exact_name | `sensor_load_binary_sequence` | `sensor_load_binary_sequence` | 304 | 647 | 2.128 | +343 | 8->10 | 34->34 |
| expanded | exact_name | `ae_calculate_target` | `ae_calculate_target` | 297 | 634 | 2.135 | +337 | 18->11 | 19->12 |
| expanded | exact_name | `apical_api_init_idx_array` | `apical_api_init_idx_array` | 295 | 1014 | 3.437 | +719 | 1->1 | 0->0 |
| expanded | exact_name | `apical_isp_init` | `apical_isp_init` | 281 | 587 | 2.089 | +306 | 42->43 | 1->0 |
| expanded | exact_name | `matrix_yuv_recompute` | `matrix_yuv_recompute` | 268 | 589 | 2.198 | +321 | 5->5 | 33->46 |
| expanded | exact_name | `scene_mode` | `scene_mode` | 257 | 564 | 2.195 | +307 | 21->14 | 20->64 |
| expanded | exact_name | `apical_cmd_process` | `apical_cmd_process` | 255 | 661 | 2.592 | +406 | 31->58 | 17->41 |
| expanded | exact_name | `tx_isp_vic_start` | `tx_isp_vic_start` | 231 | 680 | 2.944 | +449 | 1->2 | 42->73 |
| expanded | exact_name | `subdev_core_ops_set_input` | `subdev_core_ops_set_input` | 209 | 453 | 2.167 | +244 | 12->14 | 41->54 |
| expanded | exact_name | `cmos_inttime_update` | `cmos_inttime_update` | 204 | 455 | 2.230 | +251 | 11->12 | 20->45 |
| expanded | exact_name | `tx_isp_frame_channel_device_register` | `tx_isp_frame_channel_device_register` | 193 | 416 | 2.155 | +223 | 14->14 | 15->20 |
| expanded | exact_name | `color_matrix_write` | `color_matrix_write` | 188 | 456 | 2.426 | +268 | 45->46 | 3->2 |
| expanded | exact_name | `sinter_strength_calculate` | `sinter_strength_calculate` | 182 | 463 | 2.544 | +281 | 27->27 | 3->2 |
| expanded | exact_name | `dump_vic_reg` | `dump_vic_reg` | 172 | 466 | 2.709 | +294 | 26->27 | 1->0 |
| expanded | exact_name | `frame_channel_vidioc_default` | `frame_channel_vidioc_default` | 165 | 349 | 2.115 | +184 | 8->8 | 30->39 |
| expanded | exact_name | `awb_init` | `awb_init` | 162 | 386 | 2.383 | +224 | 22->22 | 7->14 |
| expanded | exact_name | `system_program_interrupt_event` | `system_program_interrupt_event` | 155 | 492 | 3.174 | +337 | 16->32 | 18->46 |
| expanded | exact_name | `apical_sbus_write_data` | `apical_sbus_write_data` | 153 | 423 | 2.765 | +270 | 6->8 | 22->30 |
| expanded | exact_name | `ae_calculate_exposure` | `ae_calculate_exposure` | 145 | 341 | 2.352 | +196 | 8->9 | 10->17 |
| expanded | exact_name | `apical_api_calibration` | `apical_api_calibration` | 143 | 346 | 2.420 | +203 | 6->14 | 28->42 |
| expanded | exact_name | `image_tuning_v4l2_open` | `image_tuning_v4l2_open` | 143 | 290 | 2.028 | +147 | 16->16 | 10->11 |
| expanded | exact_name | `apical_isp_process_interrupt` | `apical_isp_process_interrupt` | 129 | 279 | 2.163 | +150 | 7->8 | 9->9 |
| expanded | exact_name | `matrix_compute_hue_saturation` | `matrix_compute_hue_saturation` | 128 | 261 | 2.039 | +133 | 3->3 | 17->19 |
| expanded | exact_name | `image_resize_enable` | `image_resize_enable` | 127 | 386 | 3.039 | +259 | 1->10 | 37->60 |
| expanded | exact_name | `get_gmv_gauss_method_fast_v3` | `get_gmv_gauss_method_fast_v3` | 126 | 261 | 2.071 | +135 | 2->3 | 13->19 |
| expanded | replacement | `sinfo_show` | `tx_isp_sinfo_show` | 124 | 859 | 6.927 | +735 | 3->48 | 28->121 |
| expanded | exact_name | `compute_transfrom_matrix` | `compute_transfrom_matrix` | 124 | 248 | 2.000 | +124 | 10->10 | 12->18 |
| expanded | exact_name | `antifog_set_preset` | `antifog_set_preset` | 122 | 248 | 2.033 | +126 | 12->12 | 14->12 |
| expanded | exact_name | `spi_io_write_sample` | `spi_io_write_sample` | 117 | 292 | 2.496 | +175 | 4->5 | 18->29 |
| expanded | exact_name | `cmos_move_exposure_history` | `cmos_move_exposure_history` | 112 | 243 | 2.170 | +131 | 6->5 | 10->14 |
| expanded | exact_name | `tx_isp_sinfo_sensor_bind` | `tx_isp_sinfo_sensor_bind` | 110 | 265 | 2.409 | +155 | 3->8 | 14->25 |
| expanded | exact_name | `apply_dvi_sync_param` | `apply_dvi_sync_param` | 110 | 255 | 2.318 | +145 | 19->20 | 1->0 |
| expanded | exact_name | `frame_channel_v4l2_open` | `frame_channel_v4l2_open` | 110 | 241 | 2.191 | +131 | 10->11 | 11->13 |
| expanded | exact_name | `apply_dvi_fpga_sync_param` | `apply_dvi_fpga_sync_param` | 109 | 240 | 2.202 | +131 | 19->20 | 1->0 |
| expanded | exact_name | `apical_sbus_read_u32` | `apical_sbus_read_u32` | 107 | 216 | 2.019 | +109 | 3->3 | 17->21 |
| expanded | exact_name | `sensor_write_data` | `sensor_write_data` | 106 | 213 | 2.009 | +107 | 4->4 | 9->12 |
| expanded | exact_name | `flash_initialize` | `flash_initialize` | 101 | 321 | 3.178 | +220 | 31->32 | 1->0 |
| expanded | exact_name | `dump_csi_reg` | `dump_csi_reg` | 100 | 266 | 2.660 | +166 | 14->15 | 1->0 |
| expanded | exact_name | `_update_fr` | `_update_fr` | 99 | 226 | 2.283 | +127 | 10->10 | 5->9 |
| expanded | exact_name | `dis_update_bg_map` | `dis_update_bg_map` | 99 | 226 | 2.283 | +127 | 1->1 | 8->16 |
| expanded | exact_name | `update_composite_matrix` | `update_composite_matrix` | 98 | 236 | 2.408 | +138 | 0->0 | 15->17 |
| expanded | exact_name | `iir_filter_v4` | `iir_filter_v4` | 95 | 209 | 2.200 | +114 | 4->4 | 0->4 |
| expanded | exact_name | `get_gmv_gauss_method_fast_v2` | `get_gmv_gauss_method_fast_v2` | 91 | 191 | 2.099 | +100 | 0->0 | 11->13 |
| expanded | exact_name | `frame_channel_video_irq_notify` | `frame_channel_video_irq_notify` | 88 | 192 | 2.182 | +104 | 6->7 | 8->10 |
| expanded | exact_name | `calc_scaled_modulation_u16` | `calc_scaled_modulation_u16` | 87 | 335 | 3.851 | +248 | 0->0 | 13->25 |
| expanded | exact_name | `frame_channel_vb2_buffer_prepare` | `frame_channel_vb2_buffer_prepare` | 87 | 260 | 2.989 | +173 | 5->7 | 11->24 |
| expanded | exact_name | `awb_process_light_source` | `awb_process_light_source` | 87 | 241 | 2.770 | +154 | 1->1 | 12->19 |
| expanded | exact_name | `ae_exposure` | `ae_exposure` | 86 | 295 | 3.430 | +209 | 4->4 | 11->19 |
| expanded | exact_name | `register_value` | `register_value` | 86 | 195 | 2.267 | +109 | 2->6 | 22->24 |
| expanded | exact_name | `ae_calculate_exposure_ratio` | `ae_calculate_exposure_ratio` | 83 | 199 | 2.398 | +116 | 2->2 | 7->9 |
| expanded | exact_name | `AWB_fsm_switch_state` | `AWB_fsm_switch_state` | 82 | 178 | 2.171 | +96 | 2->12 | 21->31 |
| expanded | exact_name | `general_frame_start` | `general_frame_start` | 75 | 227 | 3.027 | +152 | 0->2 | 11->15 |
| expanded | exact_name | `calc_adjust_modulation_u16` | `calc_adjust_modulation_u16` | 75 | 216 | 2.880 | +141 | 0->0 | 12->8 |
| expanded | exact_name | `isp_sen_reg_read` | `isp_sen_reg_read` | 75 | 155 | 2.067 | +80 | 3->3 | 10->16 |
| expanded | exact_name | `calc_inv_equidistant_modulation_u16` | `calc_inv_equidistant_modulation_u16` | 73 | 239 | 3.274 | +166 | 0->0 | 10->19 |
| expanded | exact_name | `frame_channel_vb2_stop_streaming` | `frame_channel_vb2_stop_streaming` | 73 | 162 | 2.219 | +89 | 5->6 | 8->13 |
| expanded | exact_name | `matrix_yuv_initialize` | `matrix_yuv_initialize` | 72 | 202 | 2.806 | +130 | 8->8 | 1->2 |
| expanded | exact_name | `frame_channel_vb2_start_streaming` | `frame_channel_vb2_start_streaming` | 71 | 153 | 2.155 | +82 | 5->6 | 7->11 |
| expanded | exact_name | `apical_sbus_read_data_u32` | `apical_sbus_read_data_u32` | 70 | 190 | 2.714 | +120 | 3->3 | 7->11 |
| expanded | exact_name | `tx_isp_sinfo_driver_add` | `tx_isp_sinfo_driver_add` | 69 | 259 | 3.754 | +190 | 4->8 | 6->29 |
| expanded | exact_name | `image_resize_height` | `image_resize_height` | 69 | 224 | 3.246 | +155 | 0->0 | 17->38 |
| expanded | exact_name | `cmos_fsm_process_event` | `cmos_fsm_process_event` | 69 | 213 | 3.087 | +144 | 3->10 | 12->34 |
| expanded | exact_name | `image_resize_width` | `image_resize_width` | 69 | 207 | 3.000 | +138 | 0->0 | 17->35 |
| expanded | exact_name | `isp_io_write_sample` | `isp_io_write_sample` | 67 | 234 | 3.493 | +167 | 2->12 | 12->12 |
| expanded | exact_name | `i2c_io_write_sample` | `i2c_io_write_sample` | 67 | 198 | 2.955 | +131 | 3->3 | 9->16 |
| expanded | exact_name | `apical_frame_buffer_configure_all` | `apical_frame_buffer_configure_all` | 67 | 173 | 2.582 | +106 | 9->10 | 1->0 |
| expanded | exact_name | `crop_fsm_process_event` | `crop_fsm_process_event` | 65 | 132 | 2.031 | +67 | 3->6 | 16->19 |
| expanded | exact_name | `frame_channel_v4l2_close` | `frame_channel_v4l2_close` | 64 | 130 | 2.031 | +66 | 7->8 | 4->6 |
| expanded | exact_name | `dis_analyze_stats` | `dis_analyze_stats` | 63 | 132 | 2.095 | +69 | 3->3 | 8->11 |
| expanded | exact_name | `AWB_fsm_clear` | `AWB_fsm_clear` | 61 | 185 | 3.033 | +124 | 4->4 | 0->0 |
| expanded | exact_name | `ae_read_full_histogram_data` | `ae_read_full_histogram_data` | 61 | 160 | 2.623 | +99 | 2->2 | 4->8 |
| expanded | exact_name | `ds1_output_mode` | `ds1_output_mode` | 61 | 132 | 2.164 | +71 | 1->1 | 20->24 |
| expanded | exact_name | `fr_output_mode` | `fr_output_mode` | 61 | 128 | 2.098 | +67 | 1->1 | 20->22 |
| expanded | exact_name | `ds2_output_mode` | `ds2_output_mode` | 61 | 126 | 2.066 | +65 | 1->1 | 20->22 |
| expanded | exact_name | `sensor_read_black_pedestal` | `sensor_read_black_pedestal` | 60 | 129 | 2.150 | +69 | 5->12 | 9->11 |
| expanded | exact_name | `calc_inv_equidistant_modulation_u32` | `calc_inv_equidistant_modulation_u32` | 59 | 176 | 2.983 | +117 | 0->0 | 8->11 |
| expanded | exact_name | `cmos_long_exposure_update` | `cmos_long_exposure_update` | 59 | 128 | 2.169 | +69 | 2->3 | 4->6 |
| expanded | exact_name | `frame_channel_vb2_buffer_queue` | `frame_channel_vb2_buffer_queue` | 58 | 158 | 2.724 | +100 | 4->5 | 4->8 |
| expanded | exact_name | `dis_initialize` | `dis_initialize` | 58 | 124 | 2.138 | +66 | 5->5 | 3->4 |
| expanded | exact_name | `image_tuning_s_ctrl` | `image_tuning_s_ctrl` | 57 | 280 | 4.912 | +223 | 1->4 | 13->38 |
| expanded | exact_name | `math_exp2` | `math_exp2` | 57 | 134 | 2.351 | +77 | 1->1 | 2->2 |
| expanded | exact_name | `AWB_fsm_process_event` | `AWB_fsm_process_event` | 57 | 131 | 2.298 | +74 | 2->7 | 15->18 |
| expanded | exact_name | `tx_isp_disable_irq` | `tx_isp_disable_irq` | 57 | 129 | 2.263 | +72 | 3->5 | 4->5 |
| expanded | exact_name | `image_tuning_vidioc_default` | `image_tuning_vidioc_default` | 56 | 148 | 2.643 | +92 | 2->4 | 12->19 |
| expanded | exact_name | `apical_sensor_calibration_update` | `apical_sensor_calibration_update` | 56 | 142 | 2.536 | +86 | 3->3 | 7->8 |
| expanded | exact_name | `compute_weight` | `compute_weight` | 56 | 124 | 2.214 | +68 | 0->0 | 7->13 |
| expanded | exact_name | `isp_core_config_top_ctl_register` | `isp_core_config_top_ctl_register` | 55 | 842 | 15.309 | +787 | 3->4 | 7->119 |
| expanded | exact_name | `calc_modulation_u16` | `calc_modulation_u16` | 53 | 162 | 3.057 | +109 | 0->0 | 7->10 |
| expanded | exact_name | `calc_modulation_u32` | `calc_modulation_u32` | 53 | 129 | 2.434 | +76 | 0->0 | 7->11 |
| expanded | exact_name | `isp_irq_handle` | `isp_irq_handle` | 52 | 132 | 2.538 | +80 | 1->1 | 5->10 |
| expanded | exact_name | `image_crop_xoffset` | `image_crop_xoffset` | 51 | 127 | 2.490 | +76 | 0->0 | 15->16 |
| expanded | exact_name | `tx_isp_enable_irq` | `tx_isp_enable_irq` | 51 | 125 | 2.451 | +74 | 3->5 | 3->4 |
| expanded | exact_name | `isp_irq_thread_handle` | `isp_irq_thread_handle` | 51 | 116 | 2.275 | +65 | 1->1 | 7->16 |
| expanded | exact_name | `image_crop_yoffset` | `image_crop_yoffset` | 51 | 112 | 2.196 | +61 | 0->0 | 15->16 |
| expanded | exact_name | `cmos_set_exposure_target` | `cmos_set_exposure_target` | 50 | 112 | 2.240 | +62 | 2->2 | 5->7 |
| expanded | exact_name | `selftest_isp_interface` | `selftest_isp_interface` | 50 | 109 | 2.180 | +59 | 6->6 | 1->3 |
| expanded | exact_name | `sensor_fsm_process_state` | `sensor_fsm_process_state` | 49 | 116 | 2.367 | +67 | 1->1 | 10->25 |
| expanded | exact_name | `apical_sbus_read_u8` | `apical_sbus_read_u8` | 49 | 114 | 2.327 | +65 | 1->1 | 9->10 |
| expanded | exact_name | `math_log2` | `math_log2` | 48 | 98 | 2.042 | +50 | 1->1 | 7->8 |
| expanded | exact_name | `log2_int_to_fixed` | `log2_int_to_fixed` | 48 | 96 | 2.000 | +48 | 1->1 | 7->8 |
| expanded | exact_name | `AWB_fsm_process_state` | `AWB_fsm_process_state` | 47 | 116 | 2.468 | +69 | 1->1 | 10->26 |
| expanded | exact_name | `tx_isp_sinfo_driver_del` | `tx_isp_sinfo_driver_del` | 46 | 125 | 2.717 | +79 | 2->8 | 5->9 |
| expanded | exact_name | `noise_reduction_fsm_process_event` | `noise_reduction_fsm_process_event` | 46 | 102 | 2.217 | +56 | 3->3 | 10->6 |
| expanded | exact_name | `init_stab` | `init_stab` | 45 | 120 | 2.667 | +75 | 0->0 | 0->0 |
| expanded | exact_name | `apical_sbus_write_u8` | `apical_sbus_write_u8` | 45 | 114 | 2.533 | +69 | 0->1 | 10->11 |
| expanded | exact_name | `defect_pixel_initialize` | `defect_pixel_initialize` | 45 | 100 | 2.222 | +55 | 8->8 | 0->0 |
| expanded | exact_name | `color_matrix_fsm_process_event` | `color_matrix_fsm_process_event` | 45 | 95 | 2.111 | +50 | 2->5 | 10->12 |
| expanded | exact_name | `flash_fsm_process_event` | `flash_fsm_process_event` | 44 | 89 | 2.023 | +45 | 2->4 | 10->12 |
| expanded | exact_name | `matrix_yuv_fsm_process_event` | `matrix_yuv_fsm_process_event` | 44 | 89 | 2.023 | +45 | 2->4 | 10->12 |
| expanded | exact_name | `isp_vic_interrupt_service_routine` | `isp_vic_interrupt_service_routine` | 43 | 107 | 2.488 | +64 | 1->1 | 4->4 |
| expanded | exact_name | `iridix_fsm_process_event` | `iridix_fsm_process_event` | 43 | 87 | 2.023 | +44 | 2->4 | 9->11 |
| expanded | exact_name | `calc_equidistant_modulation_u32` | `calc_equidistant_modulation_u32` | 42 | 96 | 2.286 | +54 | 0->0 | 4->8 |
| expanded | exact_name | `matrix_matrix_multiply` | `matrix_matrix_multiply` | 39 | 102 | 2.615 | +63 | 0->0 | 5->6 |
| expanded | exact_name | `cmos_fsm_clear` | `cmos_fsm_clear` | 38 | 121 | 3.184 | +83 | 1->1 | 0->0 |
| expanded | exact_name | `get_gmv_gauss_init` | `get_gmv_gauss_init` | 38 | 104 | 2.737 | +66 | 0->0 | 11->12 |
| expanded | exact_name | `image_tuning_g_volatile_ctrl` | `image_tuning_g_volatile_ctrl` | 38 | 84 | 2.211 | +46 | 1->1 | 8->11 |
| expanded | exact_name | `tx_isp_media_pipeline_reset` | `tx_isp_media_pipeline_reset` | 38 | 80 | 2.105 | +42 | 1->1 | 8->12 |
| expanded | exact_name | `histogram_lum` | `histogram_lum` | 37 | 87 | 2.351 | +50 | 2->2 | 3->4 |
| expanded | exact_name | `cmos_store_frame_exposure_set` | `cmos_store_frame_exposure_set` | 35 | 86 | 2.457 | +51 | 1->1 | 0->0 |
| expanded | exact_name | `get_quantised_integration_time` | `get_quantised_integration_time` | 35 | 72 | 2.057 | +37 | 1->1 | 3->7 |
| expanded | exact_name | `tx_isp_sinfo_sensor_unbind` | `tx_isp_sinfo_sensor_unbind` | 34 | 127 | 3.735 | +93 | 1->6 | 5->9 |
| expanded | exact_name | `wdr_mode` | `wdr_mode` | 34 | 95 | 2.794 | +61 | 0->0 | 10->12 |
| expanded | exact_name | `isp_fops_read` | `isp_fops_read` | 33 | 72 | 2.182 | +39 | 3->4 | 1->6 |
| expanded | exact_name | `isp_fops_write` | `isp_fops_write` | 33 | 72 | 2.182 | +39 | 3->4 | 1->6 |
| expanded | exact_name | `tx_isp_sinfo_exit` | `tx_isp_sinfo_exit` | 33 | 71 | 2.152 | +38 | 4->7 | 1->2 |
| expanded | exact_name | `tx_isp_sinfo_init` | `tx_isp_sinfo_init` | 32 | 408 | 12.750 | +376 | 3->8 | 2->48 |
| expanded | exact_name | `_GET_HDR_TABLE_INDEX` | `_GET_HDR_TABLE_INDEX` | 32 | 145 | 4.531 | +113 | 0->0 | 9->33 |
| expanded | exact_name | `ae_exposure_correction` | `ae_exposure_correction` | 31 | 79 | 2.548 | +48 | 2->2 | 0->0 |
| expanded | exact_name | `get_common_estimations` | `get_common_estimations` | 30 | 76 | 2.533 | +46 | 0->0 | 5->9 |
| expanded | exact_name | `frame_buffer_manager_create` | `frame_buffer_manager_create` | 30 | 64 | 2.133 | +34 | 2->3 | 2->4 |
| expanded | exact_name | `dis_mode` | `dis_mode` | 29 | 75 | 2.586 | +46 | 0->0 | 7->9 |
| expanded | exact_name | `init_isp_set` | `init_isp_set` | 29 | 59 | 2.034 | +30 | 2->3 | 4->4 |
| expanded | exact_name | `awb_blue_gain` | `awb_blue_gain` | 28 | 67 | 2.393 | +39 | 0->0 | 7->8 |
| expanded | exact_name | `awb_red_gain` | `awb_red_gain` | 28 | 64 | 2.286 | +36 | 0->0 | 7->9 |
| expanded | exact_name | `APICAL_WRITE_32` | `APICAL_WRITE_32` | 28 | 58 | 2.071 | +30 | 0->1 | 4->4 |
| expanded | exact_name | `_process_fps_cnt` | `_process_fps_cnt` | 27 | 77 | 2.852 | +50 | 1->1 | 2->4 |
| expanded | exact_name | `matrix_vector_multiply` | `matrix_vector_multiply` | 27 | 72 | 2.667 | +45 | 0->0 | 4->4 |
| expanded | exact_name | `brightness_strength` | `brightness_strength` | 26 | 59 | 2.269 | +33 | 1->1 | 4->6 |
| expanded | exact_name | `saturation_strength` | `saturation_strength` | 26 | 59 | 2.269 | +33 | 1->1 | 4->6 |
| expanded | exact_name | `contrast_strength` | `contrast_strength` | 26 | 56 | 2.154 | +30 | 1->1 | 4->5 |
| expanded | exact_name | `APICAL_WRITE_16` | `APICAL_WRITE_16` | 26 | 53 | 2.038 | +27 | 0->1 | 3->3 |
| expanded | exact_name | `ae_split_preset` | `ae_split_preset` | 26 | 52 | 2.000 | +26 | 1->1 | 5->6 |
| expanded | exact_name | `sharpening_fsm_clear` | `sharpening_fsm_clear` | 25 | 105 | 4.200 | +80 | 0->0 | 0->0 |
| expanded | exact_name | `af_roi` | `af_roi` | 25 | 97 | 3.880 | +72 | 0->0 | 3->7 |
| expanded | exact_name | `dis_clip_gmv_vector` | `dis_clip_gmv_vector` | 25 | 67 | 2.680 | +42 | 0->0 | 5->5 |
| expanded | exact_name | `register_size` | `register_size` | 25 | 58 | 2.320 | +33 | 0->0 | 5->7 |
| expanded | exact_name | `sharpening_strength` | `sharpening_strength` | 24 | 73 | 3.042 | +49 | 0->0 | 4->8 |
| expanded | exact_name | `mem_read_u32` | `mem_read_u32` | 24 | 67 | 2.792 | +43 | 0->0 | 5->6 |
| expanded | exact_name | `au_read_histogram` | `au_read_histogram` | 24 | 61 | 2.542 | +37 | 0->0 | 1->2 |
| expanded | exact_name | `system_timer_timestamp` | `system_timer_timestamp` | 24 | 61 | 2.542 | +37 | 2->2 | 2->2 |
| expanded | exact_name | `div_fixed` | `div_fixed` | 24 | 54 | 2.250 | +30 | 2->1 | 1->2 |
| expanded | exact_name | `flash_mode` | `flash_mode` | 24 | 53 | 2.208 | +29 | 1->1 | 3->4 |
| expanded | exact_name | `log16` | `log16` | 23 | 54 | 2.348 | +31 | 0->0 | 2->2 |
| expanded | exact_name | `luts_fetch` | `luts_fetch` | 23 | 52 | 2.261 | +29 | 0->0 | 5->4 |
| expanded | exact_name | `cmos_get_fps` | `cmos_get_fps` | 22 | 54 | 2.455 | +32 | 2->2 | 1->2 |
| expanded | exact_name | `color_matrix_fsm_switch_state` | `color_matrix_fsm_switch_state` | 22 | 54 | 2.455 | +32 | 0->3 | 7->8 |
| expanded | exact_name | `cmos_alloc_sensor_digital_gain` | `cmos_alloc_sensor_digital_gain` | 22 | 48 | 2.182 | +26 | 1->1 | 2->5 |
| expanded | exact_name | `frame_channel_vb2_get_userptr` | `frame_channel_vb2_get_userptr` | 22 | 45 | 2.045 | +23 | 1->1 | 2->2 |
| expanded | exact_name | `isp_core_ops_s_ctrl` | `isp_core_ops_s_ctrl` | 22 | 45 | 2.045 | +23 | 1->2 | 2->2 |
| expanded | exact_name | `mem_write_u32` | `mem_write_u32` | 20 | 89 | 4.450 | +69 | 0->0 | 3->6 |
| expanded | exact_name | `APICAL_WRITE_8` | `APICAL_WRITE_8` | 20 | 43 | 2.150 | +23 | 0->1 | 3->3 |
| expanded | exact_name | `tx_isp_video_in_g_register` | `tx_isp_video_in_g_register` | 20 | 42 | 2.100 | +22 | 1->1 | 3->4 |
| expanded | exact_name | `tx_isp_video_in_s_register` | `tx_isp_video_in_s_register` | 20 | 42 | 2.100 | +22 | 1->1 | 3->4 |
| expanded | exact_name | `APICAL_READ_8` | `APICAL_READ_8` | 20 | 40 | 2.000 | +20 | 0->1 | 3->4 |
| expanded | exact_name | `validate_borders` | `validate_borders` | 19 | 47 | 2.474 | +28 | 0->0 | 4->7 |
| expanded | exact_name | `flash_time` | `flash_time` | 19 | 39 | 2.053 | +20 | 1->1 | 2->4 |
| expanded | exact_name | `log2_fixed_to_fixed` | `log2_fixed_to_fixed` | 18 | 154 | 8.556 | +136 | 1->0 | 0->14 |
| expanded | exact_name | `apical_api_read_buffer` | `apical_api_read_buffer` | 18 | 65 | 3.611 | +47 | 0->0 | 4->5 |
| expanded | exact_name | `gain_log2` | `gain_log2` | 18 | 64 | 3.556 | +46 | 0->0 | 2->4 |
| expanded | exact_name | `ae_compensation` | `ae_compensation` | 18 | 47 | 2.611 | +29 | 0->0 | 4->6 |
| expanded | exact_name | `antiflicker_mode` | `antiflicker_mode` | 18 | 47 | 2.611 | +29 | 0->0 | 4->5 |
| expanded | exact_name | `sensor_alloc_integration_time` | `sensor_alloc_integration_time` | 18 | 41 | 2.278 | +23 | 0->0 | 2->2 |
| expanded | exact_name | `dis_get_default_settings` | `dis_get_default_settings` | 18 | 40 | 2.222 | +22 | 0->0 | 1->1 |
| expanded | exact_name | `sensor_fps_control` | `sensor_fps_control` | 17 | 45 | 2.647 | +28 | 0->0 | 0->0 |
| expanded | exact_name | `is_yuv_format` | `is_yuv_format` | 17 | 41 | 2.412 | +24 | 0->0 | 4->6 |
| expanded | exact_name | `selftest_calibration_revision` | `selftest_calibration_revision` | 16 | 56 | 3.500 | +40 | 0->0 | 1->2 |
| expanded | exact_name | `apical_loop_buffer_write_u8` | `apical_loop_buffer_write_u8` | 16 | 51 | 3.188 | +35 | 0->0 | 0->4 |
| expanded | exact_name | `sensor_init_output` | `sensor_init_output` | 16 | 47 | 2.938 | +31 | 0->0 | 4->5 |
| expanded | exact_name | `AE_fsm_clear` | `AE_fsm_clear` | 16 | 45 | 2.812 | +29 | 0->0 | 0->0 |
| expanded | exact_name | `defect_pixel_fsm_switch_state` | `defect_pixel_fsm_switch_state` | 16 | 43 | 2.688 | +27 | 0->2 | 5->6 |
| expanded | exact_name | `dis_update_output` | `dis_update_output` | 16 | 43 | 2.688 | +27 | 0->0 | 0->0 |
| expanded | exact_name | `noise_reduction_fsm_switch_state` | `noise_reduction_fsm_switch_state` | 16 | 43 | 2.688 | +27 | 0->2 | 5->6 |
| expanded | exact_name | `dis_fsm_switch_state` | `dis_fsm_switch_state` | 16 | 41 | 2.562 | +25 | 0->2 | 5->5 |
| expanded | exact_name | `iridix_fsm_switch_state` | `iridix_fsm_switch_state` | 16 | 40 | 2.500 | +24 | 0->0 | 5->7 |
| expanded | exact_name | `flash_fsm_switch_state` | `flash_fsm_switch_state` | 16 | 39 | 2.438 | +23 | 0->0 | 5->5 |
| expanded | exact_name | `csi_s_stream` | `csi_s_stream` | 16 | 38 | 2.375 | +22 | 0->2 | 4->3 |
| expanded | exact_name | `ae_roi` | `ae_roi` | 15 | 46 | 3.067 | +31 | 0->0 | 3->4 |
| expanded | exact_name | `awb_roi` | `awb_roi` | 15 | 46 | 3.067 | +31 | 0->0 | 3->4 |
| expanded | exact_name | `system_manual_sinter` | `system_manual_sinter` | 15 | 43 | 2.867 | +28 | 0->0 | 3->4 |
| expanded | exact_name | `system_exposure_ratio` | `system_exposure_ratio` | 15 | 42 | 2.800 | +27 | 0->0 | 3->4 |
| expanded | exact_name | `system_manual_temper` | `system_manual_temper` | 15 | 42 | 2.800 | +27 | 0->0 | 3->4 |
| expanded | exact_name | `system_awb_blue_gain` | `system_awb_blue_gain` | 15 | 41 | 2.733 | +26 | 0->0 | 3->4 |
| expanded | exact_name | `system_dis_y` | `system_dis_y` | 15 | 41 | 2.733 | +26 | 0->0 | 3->4 |
| expanded | exact_name | `system_max_integration_time` | `system_max_integration_time` | 15 | 41 | 2.733 | +26 | 0->0 | 3->4 |
| expanded | exact_name | `system_maximum_iridix_strength` | `system_maximum_iridix_strength` | 15 | 41 | 2.733 | +26 | 0->0 | 3->4 |
| expanded | exact_name | `system_calibrate_bad_pixels` | `system_calibrate_bad_pixels` | 15 | 40 | 2.667 | +25 | 0->0 | 3->4 |
| expanded | exact_name | `system_exposure_bright_target` | `system_exposure_bright_target` | 15 | 40 | 2.667 | +25 | 0->0 | 3->4 |
| expanded | exact_name | `system_exposure_dark_target` | `system_exposure_dark_target` | 15 | 40 | 2.667 | +25 | 0->0 | 3->4 |
| expanded | exact_name | `system_manual_exposure` | `system_manual_exposure` | 15 | 40 | 2.667 | +25 | 0->0 | 3->4 |
| expanded | exact_name | `system_manual_exposure_ratio` | `system_manual_exposure_ratio` | 15 | 40 | 2.667 | +25 | 0->0 | 3->3 |
| expanded | exact_name | `system_manual_integration_time` | `system_manual_integration_time` | 15 | 40 | 2.667 | +25 | 0->0 | 3->4 |
| expanded | exact_name | `system_manual_saturation` | `system_manual_saturation` | 15 | 40 | 2.667 | +25 | 0->0 | 3->4 |
| expanded | exact_name | `system_max_exposure_ratio` | `system_max_exposure_ratio` | 15 | 40 | 2.667 | +25 | 0->0 | 3->4 |
| expanded | exact_name | `system_maximum_directional_sharpening` | `system_maximum_directional_sharpening` | 15 | 40 | 2.667 | +25 | 0->0 | 3->4 |
| expanded | exact_name | `system_maximum_temper_strength` | `system_maximum_temper_strength` | 15 | 40 | 2.667 | +25 | 0->0 | 3->4 |
| expanded | exact_name | `system_minimum_directional_sharpening` | `system_minimum_directional_sharpening` | 15 | 40 | 2.667 | +25 | 0->0 | 3->4 |
| expanded | exact_name | `system_temper_threshold_target` | `system_temper_threshold_target` | 15 | 40 | 2.667 | +25 | 0->0 | 3->4 |
| expanded | exact_name | `system_anti_flicker_frequency` | `system_anti_flicker_frequency` | 15 | 39 | 2.600 | +24 | 0->0 | 3->4 |
| expanded | exact_name | `system_awb_red_gain` | `system_awb_red_gain` | 15 | 39 | 2.600 | +24 | 0->0 | 3->4 |
| expanded | exact_name | `system_directional_sharpening_target` | `system_directional_sharpening_target` | 15 | 39 | 2.600 | +24 | 0->0 | 3->4 |
| expanded | exact_name | `system_iridix_strength_target` | `system_iridix_strength_target` | 15 | 39 | 2.600 | +24 | 0->0 | 3->4 |

## Replacement Map

| OEM symbol | Recovered symbol(s) |
|---|---|
| `isp_core_ops_g_ctrl` | `apical_isp_core_ops_g_ctrl`<br>`isp_core_ops_g_ctrl` |
| `isp_core_ops_ioctl` | `isp_core_frame_channel_crop_capture`<br>`isp_core_frame_channel_enum_fmt`<br>`isp_core_frame_channel_queue_buffer`<br>`isp_core_frame_channel_scaler_capture`<br>`isp_core_frame_channel_set_crop`<br>`isp_core_frame_channel_set_fmt`<br>`isp_core_frame_channel_set_scaler`<br>`isp_core_frame_channel_streamoff`<br>`isp_core_frame_channel_streamon`<br>`isp_core_frame_channel_try_fmt`<br>`isp_core_ops_ioctl`<br>`isp_core_ops_private_ioctl`<br>`isp_core_set_clk` |
| `isp_csi_ops_ioctl` | `isp_csi_ops_ioctl`<br>`isp_csi_ops_private_ioctl` |
| `sinfo_count_open` | `tx_isp_sinfo_count_open` |
| `sinfo_count_show` | `tx_isp_sinfo_count_show` |
| `sinfo_open` | `tx_isp_sinfo_open` |
| `sinfo_show` | `tx_isp_sinfo_show` |
| `sinfo_slot_publish` | `tx_isp_sinfo_slot_publish` |
| `vic_core_ops_ioctl` | `vic_core_ops_ioctl`<br>`vic_core_ops_private_ioctl` |
| `video_in_core_ops_ioctl` | `subdev_core_ops_enum_input`<br>`subdev_core_ops_get_input`<br>`subdev_core_ops_register_sensor`<br>`subdev_core_ops_release_all_sensor`<br>`subdev_core_ops_release_sensor`<br>`subdev_core_ops_set_input`<br>`subdev_core_ops_streamoff`<br>`subdev_core_ops_streamon`<br>`video_in_core_ops_ioctl` |
