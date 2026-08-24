# Binary Assembly Audit

- schema: `regtrace-binary-audit-v1`
- OEM: `/home/matteius/re-framework/tx-isp-t30.ko`
- recovered: `driver/t30/tx-isp-t30.ko`
- objdump counts exclude relocation records
- thresholds: min_oem_insns=24 stub_insns=8 collapse=0.50 similar=0.80..1.25 expansion=2.00

## Summary

| Metric | OEM | Recovered |
|---|---:|---:|
| Function symbols | 847 | 858 |
| Functions with disassembly | 847 | 858 |
| Executable section bytes | 233368 | 497984 |
| Initialized writable bytes | 56688 | 48864 |
| Uninitialized writable bytes | 34676 | 412480 |

- direct matches: 820
- replacement matches: 4 (missing=0)
- unmatched: OEM-only=23 recovered-only=34
- matched instructions: OEM=56179 recovered=118992 ratio=2.118
- classes: stub=0 collapsed=0 shorter=4 same_count=0 similar=32 larger=294 expanded=494

## Allocated Section Delta

| Section | OEM bytes | Recovered bytes | Delta |
|---|---:|---:|---:|
| `.MIPS.abiflags` | 24 | 24 | +0 |
| `.bss` | 34576 | 412480 | +377904 |
| `.data` | 56400 | 48576 | -7824 |
| `.exit.text` | 1368 | 0 | -1368 |
| `.gnu.linkonce.this_module` | 288 | 288 | +0 |
| `.init.text` | 200 | 0 | -200 |
| `.modinfo` | 920 | 920 | +0 |
| `.note.gnu.build-id` | 36 | 36 | +0 |
| `.reginfo` | 24 | 24 | +0 |
| `.rodata` | 7104 | 13728 | +6624 |
| `.rodata.cst16` | 48 | 0 | -48 |
| `.rodata.cst4` | 16 | 0 | -16 |
| `.rodata.str1.4` | 14828 | 0 | -14828 |
| `.sbss` | 100 | 0 | -100 |
| `.text` | 230704 | 497984 | +267280 |
| `.text.unlikely` | 1096 | 0 | -1096 |
| `__ksymtab` | 216 | 0 | -216 |
| `__ksymtab_strings` | 592 | 0 | -592 |
| `__param` | 208 | 208 | +0 |

## Function Outliers

Showing 250 of 849 outliers. JSON and CSV contain every comparison row.

| Class | Match | OEM symbol | Recovered symbol(s) | OEM insns | Recovered insns | Ratio | Delta | Calls | Branches |
|---|---|---|---|---:|---:|---:|---:|---:|---:|
| oem_only | oem_only | `ispcore_frame_channel_streamoff.isra.0` |  | 242 | 0 | 0.000 | -242 | 15->0 | 26->0 |
| oem_only | oem_only | `mscaler_frame_channel_streamoff.isra.0` |  | 170 | 0 | 0.000 | -170 | 9->0 | 14->0 |
| oem_only | oem_only | `ae_roi.part.1` |  | 164 | 0 | 0.000 | -164 | 5->0 | 14->0 |
| oem_only | oem_only | `awb_roi.part.2` |  | 164 | 0 | 0.000 | -164 | 5->0 | 14->0 |
| oem_only | oem_only | `AWB_mesh.isra.1` |  | 160 | 0 | 0.000 | -160 | 20->0 | 5->0 |
| oem_only | oem_only | `apical_program_interrupt_event.part.0` |  | 145 | 0 | 0.000 | -145 | 16->0 | 18->0 |
| oem_only | oem_only | `ispcore_frame_channel_set_crop.isra.0` |  | 129 | 0 | 0.000 | -129 | 11->0 | 13->0 |
| oem_only | oem_only | `apical_isp_table_s_attr.isra.0` |  | 116 | 0 | 0.000 | -116 | 10->0 | 8->0 |
| oem_only | oem_only | `isp_configure_base_addr.isra.0` |  | 102 | 0 | 0.000 | -102 | 2->0 | 9->0 |
| oem_only | oem_only | `isp_set_buffer_address_vflip_disable.isra.0` |  | 75 | 0 | 0.000 | -75 | 2->0 | 4->0 |
| oem_only | oem_only | `apical_isp_awb_weight_s_attr.isra.0` |  | 73 | 0 | 0.000 | -73 | 4->0 | 3->0 |
| oem_only | oem_only | `apical_isp_ae_weight_g_attr.isra.0` |  | 63 | 0 | 0.000 | -63 | 3->0 | 3->0 |
| oem_only | oem_only | `apical_isp_awb_weight_g_attr.isra.0` |  | 63 | 0 | 0.000 | -63 | 3->0 | 3->0 |
| oem_only | oem_only | `isp_modify_dma_direction.isra.0` |  | 61 | 0 | 0.000 | -61 | 2->0 | 6->0 |
| oem_only | oem_only | `tx_isp_video_link_destroy.isra.0` |  | 55 | 0 | 0.000 | -55 | 4->0 | 4->0 |
| oem_only | oem_only | `tx_isp_init` |  | 50 | 0 | 0.000 | -50 | 6->0 | 6->0 |
| oem_only | oem_only | `apical_isp_gamma_g_attr.isra.0` |  | 47 | 0 | 0.000 | -47 | 3->0 | 2->0 |
| oem_only | oem_only | `__enqueue_in_driver.isra.0` |  | 43 | 0 | 0.000 | -43 | 3->0 | 3->0 |
| oem_only | oem_only | `apical_isp_stab_g_attr.isra.0` |  | 41 | 0 | 0.000 | -41 | 4->0 | 1->0 |
| oem_only | oem_only | `isp_enable_dma_transfer.isra.0` |  | 41 | 0 | 0.000 | -41 | 2->0 | 7->0 |
| oem_only | oem_only | `isp_subdev_release_clks.isra.0` |  | 34 | 0 | 0.000 | -34 | 2->0 | 3->0 |
| oem_only | oem_only | `subdev_video_destroy_link.isra.0` |  | 22 | 0 | 0.000 | -22 | 0->0 | 3->0 |
| oem_only | oem_only | `tx_isp_exit` |  | 18 | 0 | 0.000 | -18 | 2->0 | 1->0 |
| shorter | replacement | `apical_isp_core_ops_s_ctrl` | `apical_isp_ae_weight_s_attr_isra_0`<br>`apical_isp_core_ops_s_ctrl`<br>`apical_isp_gamma_s_attr_isra_0`<br>`apical_isp_stab_s_attr_isra_0` | 2619 | 1527 | 0.583 | -1092 | 312->44 | 232->143 |
| shorter | exact_name | `apical_interrupt_frame_buffer_ds` | `apical_interrupt_frame_buffer_ds` | 27 | 19 | 0.704 | -8 | 1->0 | 1->0 |
| shorter | exact_name | `apical_interrupt_frame_buffer_ds2` | `apical_interrupt_frame_buffer_ds2` | 27 | 19 | 0.704 | -8 | 1->0 | 1->0 |
| shorter | exact_name | `apical_fw_process` | `apical_fw_process` | 25 | 15 | 0.600 | -10 | 1->1 | 3->0 |
| expanded | exact_name | `apical_command` | `apical_command` | 1300 | 2877 | 2.213 | +1577 | 1->131 | 152->406 |
| expanded | exact_name | `apical_isp_core_ops_g_ctrl` | `apical_isp_core_ops_g_ctrl` | 1266 | 4821 | 3.808 | +3555 | 115->24 | 139->851 |
| expanded | exact_name | `frame_channel_unlocked_ioctl` | `frame_channel_unlocked_ioctl` | 946 | 3018 | 3.190 | +2072 | 65->72 | 153->245 |
| expanded | exact_name | `ispcore_pad_event_handle` | `ispcore_pad_event_handle` | 891 | 2725 | 3.058 | +1834 | 55->55 | 152->246 |
| expanded | exact_name | `isp_info_show` | `isp_info_show` | 813 | 1800 | 2.214 | +987 | 144->140 | 13->18 |
| expanded | exact_name | `ispcore_interrupt_service_routine` | `ispcore_interrupt_service_routine` | 702 | 2129 | 3.033 | +1427 | 58->58 | 92->166 |
| expanded | exact_name | `tx_isp_unlocked_ioctl` | `tx_isp_unlocked_ioctl` | 693 | 2217 | 3.199 | +1524 | 40->47 | 117->203 |
| expanded | exact_name | `apical_custom_initialization` | `apical_custom_initialization` | 592 | 1592 | 2.689 | +1000 | 125->125 | 9->13 |
| expanded | exact_name | `ncu_pad_event_handle` | `ncu_pad_event_handle` | 589 | 1682 | 2.856 | +1093 | 37->38 | 65->128 |
| expanded | exact_name | `matrix_yuv_coefft_write_to_hardware` | `matrix_yuv_coefft_write_to_hardware` | 466 | 1197 | 2.569 | +731 | 119->126 | 16->12 |
| expanded | exact_name | `mscaler_pad_event_handle` | `mscaler_pad_event_handle` | 461 | 990 | 2.148 | +529 | 22->30 | 56->91 |
| expanded | exact_name | `load_tx_isp_parameters` | `load_tx_isp_parameters` | 435 | 1345 | 3.092 | +910 | 29->29 | 43->91 |
| expanded | exact_name | `ispcore_video_s_stream` | `ispcore_video_s_stream` | 433 | 1136 | 2.624 | +703 | 36->36 | 42->68 |
| expanded | exact_name | `sharpening_update` | `sharpening_update` | 404 | 879 | 2.176 | +475 | 35->36 | 35->43 |
| expanded | exact_name | `tx_isp_subdev_init` | `tx_isp_subdev_init` | 343 | 904 | 2.636 | +561 | 25->25 | 41->59 |
| expanded | exact_name | `subdev_sensor_ops_ioctl` | `subdev_sensor_ops_ioctl` | 342 | 808 | 2.363 | +466 | 26->36 | 60->103 |
| expanded | exact_name | `sensor_load_binary_sequence` | `sensor_load_binary_sequence` | 304 | 647 | 2.128 | +343 | 8->10 | 34->33 |
| expanded | exact_name | `ae_calculate_target` | `ae_calculate_target` | 297 | 872 | 2.936 | +575 | 18->11 | 19->20 |
| expanded | exact_name | `apical_api_init_idx_array` | `apical_api_init_idx_array` | 295 | 1014 | 3.437 | +719 | 1->1 | 0->0 |
| expanded | exact_name | `_update_ds2` | `_update_ds2` | 294 | 605 | 2.058 | +311 | 32->32 | 18->32 |
| expanded | exact_name | `apical_isp_init` | `apical_isp_init` | 281 | 594 | 2.114 | +313 | 42->43 | 1->0 |
| expanded | exact_name | `tx_isp_mscaler_probe` | `tx_isp_mscaler_probe` | 257 | 697 | 2.712 | +440 | 16->16 | 22->27 |
| expanded | exact_name | `scene_mode` | `scene_mode` | 257 | 535 | 2.082 | +278 | 21->14 | 20->66 |
| expanded | exact_name | `mscaler_video_s_stream` | `mscaler_video_s_stream` | 256 | 546 | 2.133 | +290 | 21->31 | 29->34 |
| expanded | exact_name | `isp_framesource_show` | `isp_framesource_show` | 241 | 525 | 2.178 | +284 | 22->23 | 14->18 |
| expanded | exact_name | `video_input_cmd_set` | `video_input_cmd_set` | 241 | 484 | 2.008 | +243 | 15->19 | 33->47 |
| expanded | exact_name | `ldc_core_ops_init` | `ldc_core_ops_init` | 240 | 655 | 2.729 | +415 | 21->21 | 24->50 |
| expanded | exact_name | `tx_isp_probe` | `tx_isp_probe` | 220 | 462 | 2.100 | +242 | 21->34 | 23->29 |
| expanded | exact_name | `isp_vic_cmd_set` | `isp_vic_cmd_set` | 219 | 541 | 2.470 | +322 | 12->13 | 26->44 |
| expanded | exact_name | `tx_isp_fs_probe` | `tx_isp_fs_probe` | 216 | 550 | 2.546 | +334 | 17->16 | 18->37 |
| expanded | exact_name | `sharpening_initialize` | `sharpening_initialize` | 193 | 423 | 2.192 | +230 | 24->27 | 16->15 |
| expanded | exact_name | `color_matrix_write` | `color_matrix_write` | 188 | 417 | 2.218 | +229 | 45->46 | 3->2 |
| expanded | exact_name | `csi_core_ops_init` | `csi_core_ops_init` | 185 | 576 | 3.114 | +391 | 8->8 | 15->23 |
| expanded | exact_name | `sinter_strength_calculate` | `sinter_strength_calculate` | 182 | 465 | 2.555 | +283 | 27->27 | 3->2 |
| expanded | exact_name | `dump_vic_reg` | `dump_vic_reg` | 172 | 422 | 2.453 | +250 | 26->27 | 1->2 |
| expanded | exact_name | `isp_mscaler_show` | `isp_mscaler_show` | 166 | 354 | 2.133 | +188 | 14->15 | 10->14 |
| expanded | exact_name | `mscaler_sync_sensor_attr` | `mscaler_sync_sensor_attr` | 162 | 535 | 3.302 | +373 | 7->23 | 24->43 |
| expanded | exact_name | `awb_init` | `awb_init` | 162 | 413 | 2.549 | +251 | 22->21 | 7->26 |
| expanded | exact_name | `system_program_interrupt_event` | `system_program_interrupt_event` | 155 | 443 | 2.858 | +288 | 16->17 | 18->50 |
| expanded | exact_name | `apical_sbus_write_data` | `apical_sbus_write_data` | 153 | 442 | 2.889 | +289 | 6->8 | 22->38 |
| expanded | exact_name | `isp_core_tunning_unlocked_ioctl` | `isp_core_tunning_unlocked_ioctl` | 152 | 466 | 3.066 | +314 | 13->19 | 18->39 |
| expanded | exact_name | `orientation_vflip` | `orientation_vflip` | 151 | 302 | 2.000 | +151 | 14->16 | 20->22 |
| expanded | exact_name | `tx_isp_create_graph_and_nodes` | `tx_isp_create_graph_and_nodes` | 149 | 367 | 2.463 | +218 | 10->7 | 19->27 |
| expanded | exact_name | `tx_isp_ldc_show` | `tx_isp_ldc_show` | 149 | 367 | 2.463 | +218 | 12->15 | 15->15 |
| expanded | exact_name | `color_matrix_change_CCMs` | `color_matrix_change_CCMs` | 146 | 292 | 2.000 | +146 | 7->8 | 10->16 |
| expanded | exact_name | `ae_calculate_exposure` | `ae_calculate_exposure` | 145 | 435 | 3.000 | +290 | 8->9 | 10->15 |
| expanded | exact_name | `apical_api_calibration` | `apical_api_calibration` | 143 | 414 | 2.895 | +271 | 6->6 | 28->35 |
| expanded | exact_name | `frame_chan_event` | `frame_chan_event` | 140 | 402 | 2.871 | +262 | 8->8 | 12->22 |
| expanded | exact_name | `subdev_sensor_ops_set_input` | `subdev_sensor_ops_set_input` | 135 | 405 | 3.000 | +270 | 9->9 | 27->37 |
| expanded | exact_name | `tx_isp_sync_ldc` | `tx_isp_sync_ldc` | 130 | 468 | 3.600 | +338 | 1->2 | 9->18 |
| expanded | exact_name | `isp_vic_interrupt_service_routine` | `isp_vic_interrupt_service_routine` | 129 | 379 | 2.938 | +250 | 11->11 | 8->20 |
| expanded | exact_name | `apical_isp_process_interrupt` | `apical_isp_process_interrupt` | 129 | 266 | 2.062 | +137 | 7->8 | 9->9 |
| expanded | exact_name | `matrix_compute_hue_saturation` | `matrix_compute_hue_saturation` | 128 | 277 | 2.164 | +149 | 3->3 | 17->20 |
| expanded | exact_name | `image_resize_enable` | `image_resize_enable` | 127 | 381 | 3.000 | +254 | 1->10 | 37->60 |
| expanded | exact_name | `get_gmv_gauss_method_fast_v3` | `get_gmv_gauss_method_fast_v3` | 126 | 263 | 2.087 | +137 | 2->3 | 13->19 |
| expanded | exact_name | `sinfo_show` | `sinfo_show` | 124 | 349 | 2.815 | +225 | 3->10 | 28->54 |
| expanded | exact_name | `compute_transfrom_matrix` | `compute_transfrom_matrix` | 124 | 249 | 2.008 | +125 | 10->10 | 12->18 |
| expanded | exact_name | `apical_sbus_write_u32` | `apical_sbus_write_u32` | 119 | 271 | 2.277 | +152 | 1->3 | 18->21 |
| expanded | exact_name | `awb_normalise` | `awb_normalise` | 118 | 244 | 2.068 | +126 | 9->10 | 3->7 |
| expanded | exact_name | `spi_io_write_sample` | `spi_io_write_sample` | 117 | 294 | 2.513 | +177 | 4->5 | 18->29 |
| expanded | exact_name | `cmos_move_exposure_history` | `cmos_move_exposure_history` | 112 | 262 | 2.339 | +150 | 6->6 | 10->16 |
| expanded | exact_name | `ispcore_core_ops_ioctl` | `ispcore_core_ops_ioctl` | 112 | 241 | 2.152 | +129 | 3->8 | 25->37 |
| expanded | exact_name | `tx_isp_sinfo_sensor_bind` | `tx_isp_sinfo_sensor_bind` | 110 | 312 | 2.836 | +202 | 3->3 | 14->22 |
| expanded | exact_name | `apply_dvi_sync_param` | `apply_dvi_sync_param` | 110 | 262 | 2.382 | +152 | 19->20 | 1->0 |
| expanded | exact_name | `apply_dvi_fpga_sync_param` | `apply_dvi_fpga_sync_param` | 109 | 230 | 2.110 | +121 | 19->20 | 1->0 |
| expanded | exact_name | `isp_subdev_init_clks` | `isp_subdev_init_clks` | 108 | 253 | 2.343 | +145 | 8->8 | 12->14 |
| expanded | exact_name | `sensor_write_data` | `sensor_write_data` | 106 | 212 | 2.000 | +106 | 4->4 | 9->12 |
| expanded | exact_name | `flash_initialize` | `flash_initialize` | 101 | 235 | 2.327 | +134 | 31->32 | 1->0 |
| expanded | exact_name | `_update_fr` | `_update_fr` | 99 | 215 | 2.172 | +116 | 10->10 | 5->9 |
| expanded | exact_name | `dis_update_bg_map` | `dis_update_bg_map` | 99 | 209 | 2.111 | +110 | 1->1 | 8->17 |
| expanded | exact_name | `update_composite_matrix` | `update_composite_matrix` | 98 | 224 | 2.286 | +126 | 0->0 | 15->17 |
| expanded | exact_name | `get_gmv_gauss_method_fast_v2` | `get_gmv_gauss_method_fast_v2` | 91 | 200 | 2.198 | +109 | 0->0 | 11->14 |
| expanded | exact_name | `calc_scaled_modulation_u16` | `calc_scaled_modulation_u16` | 87 | 335 | 3.851 | +248 | 0->0 | 13->25 |
| expanded | exact_name | `awb_process_light_source` | `awb_process_light_source` | 87 | 242 | 2.782 | +155 | 1->1 | 12->19 |
| expanded | exact_name | `ae_exposure` | `ae_exposure` | 86 | 308 | 3.581 | +222 | 4->4 | 11->19 |
| expanded | exact_name | `register_value` | `register_value` | 86 | 189 | 2.198 | +103 | 2->6 | 22->24 |
| expanded | exact_name | `cmos_update_exposure_partitioning_lut` | `cmos_update_exposure_partitioning_lut` | 84 | 186 | 2.214 | +102 | 2->4 | 10->15 |
| expanded | exact_name | `tx_isp_vic_probe` | `tx_isp_vic_probe` | 84 | 174 | 2.071 | +90 | 9->9 | 4->5 |
| expanded | exact_name | `ae_calculate_exposure_ratio` | `ae_calculate_exposure_ratio` | 83 | 191 | 2.301 | +108 | 2->2 | 7->9 |
| expanded | exact_name | `mscaler_core_interrupt_service_routine` | `mscaler_core_interrupt_service_routine` | 82 | 224 | 2.732 | +142 | 9->9 | 8->11 |
| expanded | exact_name | `sensor_fsm_process_event` | `sensor_fsm_process_event` | 78 | 166 | 2.128 | +88 | 5->12 | 17->20 |
| expanded | exact_name | `iir_filter_v3` | `iir_filter_v3` | 76 | 191 | 2.513 | +115 | 4->4 | 0->4 |
| expanded | exact_name | `tx_isp_csi_slake_subdev` | `tx_isp_csi_slake_subdev` | 75 | 223 | 2.973 | +148 | 5->5 | 12->24 |
| expanded | exact_name | `general_frame_start` | `general_frame_start` | 75 | 192 | 2.560 | +117 | 0->2 | 11->22 |
| expanded | exact_name | `ldc_restart_module` | `ldc_restart_module` | 74 | 181 | 2.446 | +107 | 1->1 | 4->10 |
| expanded | exact_name | `isp_malloc_buffer` | `isp_malloc_buffer` | 74 | 151 | 2.041 | +77 | 4->5 | 11->13 |
| expanded | replacement | `apical_isp_ae_weight_s_attr.isra.0` | `apical_isp_ae_weight_s_attr_isra_0` | 73 | 211 | 2.890 | +138 | 4->5 | 3->7 |
| expanded | exact_name | `calc_inv_equidistant_modulation_u16` | `calc_inv_equidistant_modulation_u16` | 73 | 194 | 2.658 | +121 | 0->0 | 10->14 |
| expanded | exact_name | `matrix_yuv_initialize` | `matrix_yuv_initialize` | 72 | 149 | 2.069 | +77 | 8->8 | 1->2 |
| expanded | exact_name | `apical_sbus_read_data_u32` | `apical_sbus_read_data_u32` | 70 | 200 | 2.857 | +130 | 3->3 | 7->15 |
| expanded | exact_name | `image_resize_width` | `image_resize_width` | 69 | 207 | 3.000 | +138 | 0->0 | 17->35 |
| expanded | exact_name | `cmos_fsm_process_event` | `cmos_fsm_process_event` | 69 | 199 | 2.884 | +130 | 3->3 | 12->36 |
| expanded | exact_name | `sensor_set_mode` | `sensor_set_mode` | 67 | 212 | 3.164 | +145 | 3->3 | 7->13 |
| expanded | exact_name | `i2c_io_write_sample` | `i2c_io_write_sample` | 67 | 201 | 3.000 | +134 | 3->3 | 9->16 |
| expanded | exact_name | `apical_frame_buffer_configure_all` | `apical_frame_buffer_configure_all` | 67 | 169 | 2.522 | +102 | 9->10 | 1->0 |
| expanded | exact_name | `isp_io_write_sample` | `isp_io_write_sample` | 67 | 146 | 2.179 | +79 | 2->6 | 12->13 |
| expanded | exact_name | `isp_free_buffer` | `isp_free_buffer` | 67 | 140 | 2.090 | +73 | 2->3 | 13->16 |
| expanded | exact_name | `ldc_core_interrupt_service_routine` | `ldc_core_interrupt_service_routine` | 65 | 185 | 2.846 | +120 | 4->4 | 4->10 |
| expanded | exact_name | `__vb2_queue_cancel` | `__vb2_queue_cancel` | 65 | 135 | 2.077 | +70 | 4->4 | 5->8 |
| expanded | exact_name | `dis_analyze_stats` | `dis_analyze_stats` | 63 | 127 | 2.016 | +64 | 3->3 | 8->11 |
| expanded | exact_name | `tx_isp_sync_ncu` | `tx_isp_sync_ncu` | 62 | 214 | 3.452 | +152 | 1->1 | 8->15 |
| expanded | exact_name | `tx_isp_csi_activate_subdev` | `tx_isp_csi_activate_subdev` | 61 | 202 | 3.311 | +141 | 3->3 | 10->24 |
| expanded | exact_name | `tx_isp_vin_slake_subdev` | `tx_isp_vin_slake_subdev` | 61 | 190 | 3.115 | +129 | 6->6 | 7->14 |
| expanded | exact_name | `AWB_fsm_clear` | `AWB_fsm_clear` | 61 | 181 | 2.967 | +120 | 4->4 | 0->0 |
| expanded | exact_name | `ae_read_full_histogram_data` | `ae_read_full_histogram_data` | 61 | 161 | 2.639 | +100 | 2->2 | 4->8 |
| expanded | exact_name | `ds1_output_mode` | `ds1_output_mode` | 61 | 128 | 2.098 | +67 | 1->1 | 20->22 |
| expanded | exact_name | `ds2_output_mode` | `ds2_output_mode` | 61 | 128 | 2.098 | +67 | 1->1 | 20->22 |
| expanded | exact_name | `fr_output_mode` | `fr_output_mode` | 61 | 127 | 2.082 | +66 | 1->1 | 20->22 |
| expanded | exact_name | `tx_isp_subdev_deinit` | `tx_isp_subdev_deinit` | 60 | 167 | 2.783 | +107 | 8->8 | 6->12 |
| expanded | exact_name | `find_subdev_link_pad` | `find_subdev_link_pad` | 59 | 203 | 3.441 | +144 | 1->1 | 10->18 |
| expanded | exact_name | `calc_inv_equidistant_modulation_u32` | `calc_inv_equidistant_modulation_u32` | 59 | 142 | 2.407 | +83 | 0->0 | 8->12 |
| expanded | exact_name | `cmos_long_exposure_update` | `cmos_long_exposure_update` | 59 | 124 | 2.102 | +65 | 2->3 | 4->6 |
| expanded | exact_name | `tx_isp_vic_activate_subdev` | `tx_isp_vic_activate_subdev` | 58 | 178 | 3.069 | +120 | 3->3 | 8->18 |
| expanded | exact_name | `math_exp2` | `math_exp2` | 57 | 135 | 2.368 | +78 | 1->1 | 2->2 |
| expanded | exact_name | `AWB_fsm_process_event` | `AWB_fsm_process_event` | 57 | 133 | 2.333 | +76 | 2->7 | 15->18 |
| expanded | exact_name | `configure_channel_dma_addr` | `configure_channel_dma_addr` | 56 | 142 | 2.536 | +86 | 0->0 | 6->9 |
| expanded | exact_name | `compute_weight` | `compute_weight` | 56 | 135 | 2.411 | +79 | 0->0 | 7->13 |
| expanded | exact_name | `calc_equidistant_modulation_u16` | `calc_equidistant_modulation_u16` | 56 | 118 | 2.107 | +62 | 0->0 | 4->9 |
| expanded | exact_name | `calc_modulation_u16` | `calc_modulation_u16` | 53 | 173 | 3.264 | +120 | 0->0 | 7->10 |
| expanded | exact_name | `calc_modulation_u32` | `calc_modulation_u32` | 53 | 164 | 3.094 | +111 | 0->0 | 7->11 |
| expanded | replacement | `apical_isp_gamma_s_attr.isra.0` | `apical_isp_gamma_s_attr_isra_0` | 53 | 157 | 2.962 | +104 | 4->5 | 4->8 |
| expanded | exact_name | `image_crop_xoffset` | `image_crop_xoffset` | 51 | 122 | 2.392 | +71 | 0->0 | 15->16 |
| expanded | exact_name | `image_crop_yoffset` | `image_crop_yoffset` | 51 | 117 | 2.294 | +66 | 0->0 | 15->16 |
| expanded | exact_name | `private_math_exp2` | `private_math_exp2` | 50 | 130 | 2.600 | +80 | 1->1 | 2->2 |
| expanded | exact_name | `sensor_fsm_process_state` | `sensor_fsm_process_state` | 49 | 158 | 3.224 | +109 | 1->5 | 10->25 |
| expanded | exact_name | `tx_isp_release` | `tx_isp_release` | 49 | 149 | 3.041 | +100 | 2->2 | 9->17 |
| expanded | exact_name | `apical_sbus_read_u8` | `apical_sbus_read_u8` | 49 | 113 | 2.306 | +64 | 1->1 | 9->10 |
| expanded | exact_name | `tx_isp_open` | `tx_isp_open` | 48 | 150 | 3.125 | +102 | 1->1 | 9->15 |
| expanded | exact_name | `__fill_v4l2_buffer` | `__fill_v4l2_buffer` | 48 | 111 | 2.312 | +63 | 1->1 | 6->7 |
| expanded | exact_name | `math_log2` | `math_log2` | 48 | 107 | 2.229 | +59 | 1->1 | 7->8 |
| expanded | exact_name | `log2_int_to_fixed` | `log2_int_to_fixed` | 48 | 98 | 2.042 | +50 | 1->1 | 7->8 |
| expanded | exact_name | `AWB_fsm_process_state` | `AWB_fsm_process_state` | 47 | 144 | 3.064 | +97 | 1->1 | 10->26 |
| expanded | exact_name | `noise_reduction_fsm_process_event` | `noise_reduction_fsm_process_event` | 46 | 103 | 2.239 | +57 | 3->3 | 10->6 |
| expanded | exact_name | `apical_sbus_write_u8` | `apical_sbus_write_u8` | 45 | 132 | 2.933 | +87 | 0->1 | 10->11 |
| expanded | exact_name | `defect_pixel_initialize` | `defect_pixel_initialize` | 45 | 100 | 2.222 | +55 | 8->8 | 0->0 |
| expanded | exact_name | `color_matrix_fsm_process_event` | `color_matrix_fsm_process_event` | 45 | 91 | 2.022 | +46 | 2->5 | 10->12 |
| expanded | exact_name | `dis_fsm_process_event` | `dis_fsm_process_event` | 44 | 89 | 2.023 | +45 | 2->4 | 10->12 |
| expanded | exact_name | `flash_fsm_process_event` | `flash_fsm_process_event` | 44 | 89 | 2.023 | +45 | 2->4 | 10->12 |
| expanded | exact_name | `matrix_yuv_fsm_process_event` | `matrix_yuv_fsm_process_event` | 44 | 89 | 2.023 | +45 | 2->4 | 10->12 |
| expanded | exact_name | `vin_s_stream` | `vin_s_stream` | 43 | 103 | 2.395 | +60 | 1->1 | 12->19 |
| expanded | exact_name | `iridix_fsm_process_event` | `iridix_fsm_process_event` | 43 | 89 | 2.070 | +46 | 2->4 | 9->12 |
| expanded | exact_name | `calc_equidistant_modulation_u32` | `calc_equidistant_modulation_u32` | 42 | 96 | 2.286 | +54 | 0->0 | 4->8 |
| expanded | exact_name | `private_log2_int_to_fixed` | `private_log2_int_to_fixed` | 42 | 84 | 2.000 | +42 | 1->1 | 6->6 |
| expanded | exact_name | `ispcore_sync_sensor_attr` | `ispcore_sync_sensor_attr` | 41 | 84 | 2.049 | +43 | 3->4 | 7->8 |
| expanded | exact_name | `matrix_matrix_multiply` | `matrix_matrix_multiply` | 39 | 102 | 2.615 | +63 | 0->0 | 5->6 |
| expanded | exact_name | `ldc_core_ops_ioctl` | `ldc_core_ops_ioctl` | 38 | 129 | 3.395 | +91 | 1->1 | 11->17 |
| expanded | exact_name | `ncu_core_ops_ioctl` | `ncu_core_ops_ioctl` | 38 | 129 | 3.395 | +91 | 1->1 | 11->17 |
| expanded | exact_name | `cmos_fsm_clear` | `cmos_fsm_clear` | 38 | 123 | 3.237 | +85 | 1->1 | 0->0 |
| expanded | exact_name | `mscaler_core_ops_ioctl` | `mscaler_core_ops_ioctl` | 38 | 100 | 2.632 | +62 | 1->2 | 11->16 |
| expanded | exact_name | `apical_event_queue_push` | `apical_event_queue_push` | 38 | 83 | 2.184 | +45 | 1->2 | 4->6 |
| expanded | exact_name | `isp_irq_thread_handle` | `isp_irq_thread_handle` | 38 | 81 | 2.132 | +43 | 2->2 | 7->7 |
| expanded | exact_name | `get_gmv_gauss_init` | `get_gmv_gauss_init` | 38 | 80 | 2.105 | +42 | 0->0 | 11->13 |
| expanded | exact_name | `histogram_lum` | `histogram_lum` | 37 | 89 | 2.405 | +52 | 2->2 | 3->4 |
| expanded | exact_name | `__vb2_queue_free` | `__vb2_queue_free` | 36 | 73 | 2.028 | +37 | 1->1 | 2->2 |
| expanded | exact_name | `cmos_store_frame_exposure_set` | `cmos_store_frame_exposure_set` | 35 | 78 | 2.229 | +43 | 1->1 | 0->0 |
| expanded | exact_name | `get_quantised_integration_time` | `get_quantised_integration_time` | 35 | 73 | 2.086 | +38 | 1->1 | 3->7 |
| expanded | exact_name | `wdr_mode` | `wdr_mode` | 34 | 135 | 3.971 | +101 | 0->0 | 10->14 |
| expanded | exact_name | `ispcore_sensor_ops_release_all_sensor` | `ispcore_sensor_ops_release_all_sensor` | 33 | 88 | 2.667 | +55 | 1->1 | 6->12 |
| expanded | exact_name | `isp_core_tunning_open` | `isp_core_tunning_open` | 32 | 120 | 3.750 | +88 | 0->0 | 1->2 |
| expanded | exact_name | `system_chardev_init` | `system_chardev_init` | 32 | 101 | 3.156 | +69 | 1->1 | 0->2 |
| expanded | exact_name | `tx_isp_sinfo_init` | `tx_isp_sinfo_init` | 32 | 78 | 2.438 | +46 | 3->3 | 2->4 |
| expanded | exact_name | `_GET_HDR_TABLE_INDEX` | `_GET_HDR_TABLE_INDEX` | 32 | 67 | 2.094 | +35 | 0->0 | 9->10 |
| expanded | exact_name | `isp_core_tunning_release` | `isp_core_tunning_release` | 31 | 74 | 2.387 | +43 | 2->2 | 2->4 |
| expanded | exact_name | `leading_one_position` | `leading_one_position` | 31 | 63 | 2.032 | +32 | 0->0 | 5->9 |
| expanded | exact_name | `get_common_estimations` | `get_common_estimations` | 30 | 76 | 2.533 | +46 | 0->0 | 5->9 |
| expanded | exact_name | `dis_mode` | `dis_mode` | 29 | 73 | 2.517 | +44 | 0->0 | 7->9 |
| expanded | exact_name | `tx_isp_mscaler_remove` | `tx_isp_mscaler_remove` | 29 | 64 | 2.207 | +35 | 5->5 | 0->0 |
| expanded | exact_name | `private_leading_one_position_64` | `private_leading_one_position_64` | 28 | 117 | 4.179 | +89 | 0->0 | 0->10 |
| expanded | exact_name | `csi_sensor_ops_sync_sensor_attr` | `csi_sensor_ops_sync_sensor_attr` | 28 | 77 | 2.750 | +49 | 2->2 | 5->7 |
| expanded | exact_name | `awb_blue_gain` | `awb_blue_gain` | 28 | 69 | 2.464 | +41 | 0->0 | 7->9 |
| expanded | exact_name | `awb_red_gain` | `awb_red_gain` | 28 | 69 | 2.464 | +41 | 0->0 | 7->8 |
| expanded | exact_name | `private_leading_one_position` | `private_leading_one_position` | 28 | 68 | 2.429 | +40 | 0->0 | 2->8 |
| expanded | exact_name | `vic_sensor_ops_ioctl` | `vic_sensor_ops_ioctl` | 28 | 60 | 2.143 | +32 | 0->1 | 8->11 |
| expanded | exact_name | `APICAL_WRITE_32` | `APICAL_WRITE_32` | 28 | 59 | 2.107 | +31 | 0->1 | 4->4 |
| expanded | exact_name | `csi_sensor_ops_ioctl` | `csi_sensor_ops_ioctl` | 28 | 58 | 2.071 | +30 | 0->0 | 8->9 |
| expanded | exact_name | `isp_vic_frd_show` | `isp_vic_frd_show` | 27 | 94 | 3.481 | +67 | 1->2 | 5->10 |
| expanded | exact_name | `_process_fps_cnt` | `_process_fps_cnt` | 27 | 78 | 2.889 | +51 | 1->1 | 2->4 |
| expanded | exact_name | `tx_isp_vin_activate_subdev` | `tx_isp_vin_activate_subdev` | 27 | 71 | 2.630 | +44 | 2->2 | 1->2 |
| expanded | exact_name | `matrix_vector_multiply` | `matrix_vector_multiply` | 27 | 70 | 2.593 | +43 | 0->0 | 4->4 |
| expanded | exact_name | `msclaer_notify_front_module` | `msclaer_notify_front_module` | 27 | 59 | 2.185 | +32 | 1->1 | 1->2 |
| expanded | exact_name | `APICAL_WRITE_16` | `APICAL_WRITE_16` | 26 | 60 | 2.308 | +34 | 0->1 | 3->3 |
| expanded | exact_name | `contrast_strength` | `contrast_strength` | 26 | 60 | 2.308 | +34 | 1->1 | 4->6 |
| expanded | exact_name | `brightness_strength` | `brightness_strength` | 26 | 59 | 2.269 | +33 | 1->1 | 4->6 |
| expanded | exact_name | `saturation_strength` | `saturation_strength` | 26 | 59 | 2.269 | +33 | 1->1 | 4->6 |
| expanded | exact_name | `ae_split_preset` | `ae_split_preset` | 26 | 52 | 2.000 | +26 | 1->1 | 5->6 |
| expanded | exact_name | `private_driver_get_interface` | `private_driver_get_interface` | 26 | 52 | 2.000 | +26 | 2->2 | 4->6 |
| expanded | exact_name | `sharpening_fsm_clear` | `sharpening_fsm_clear` | 25 | 105 | 4.200 | +80 | 0->0 | 0->0 |
| expanded | exact_name | `af_roi` | `af_roi` | 25 | 102 | 4.080 | +77 | 0->0 | 3->9 |
| expanded | exact_name | `isp_lfb_config_default_dma` | `isp_lfb_config_default_dma` | 25 | 92 | 3.680 | +67 | 0->0 | 1->2 |
| expanded | exact_name | `dis_clip_gmv_vector` | `dis_clip_gmv_vector` | 25 | 67 | 2.680 | +42 | 0->0 | 5->5 |
| expanded | exact_name | `register_size` | `register_size` | 25 | 58 | 2.320 | +33 | 0->0 | 5->7 |
| expanded | exact_name | `sharpening_strength` | `sharpening_strength` | 24 | 78 | 3.250 | +54 | 0->0 | 4->8 |
| expanded | exact_name | `au_read_histogram` | `au_read_histogram` | 24 | 62 | 2.583 | +38 | 0->0 | 1->2 |
| expanded | exact_name | `tx_isp_reg_set` | `tx_isp_reg_set` | 24 | 61 | 2.542 | +37 | 0->0 | 2->2 |
| expanded | exact_name | `mem_read_u32` | `mem_read_u32` | 24 | 58 | 2.417 | +34 | 0->0 | 5->6 |
| expanded | exact_name | `div_fixed` | `div_fixed` | 24 | 54 | 2.250 | +30 | 2->1 | 1->2 |
| expanded | exact_name | `flash_mode` | `flash_mode` | 24 | 53 | 2.208 | +29 | 1->1 | 3->4 |
| expanded | exact_name | `log16` | `log16` | 23 | 62 | 2.696 | +39 | 0->0 | 2->3 |
| expanded | exact_name | `luts_fetch` | `luts_fetch` | 23 | 48 | 2.087 | +25 | 0->0 | 5->6 |
| expanded | exact_name | `color_matrix_fsm_switch_state` | `color_matrix_fsm_switch_state` | 22 | 54 | 2.455 | +32 | 0->3 | 7->8 |
| expanded | exact_name | `cmos_get_fps` | `cmos_get_fps` | 22 | 50 | 2.273 | +28 | 2->2 | 1->2 |
| expanded | exact_name | `cmos_convert_integration_time_ms2lines` | `cmos_convert_integration_time_ms2lines` | 22 | 44 | 2.000 | +22 | 1->1 | 0->0 |
| expanded | exact_name | `video_input_cmd_show` | `video_input_cmd_show` | 21 | 50 | 2.381 | +29 | 0->2 | 6->5 |
| expanded | exact_name | `mem_write_u32` | `mem_write_u32` | 20 | 89 | 4.450 | +69 | 0->0 | 3->6 |
| expanded | exact_name | `init_semaphore` | `init_semaphore` | 20 | 50 | 2.500 | +30 | 1->1 | 0->0 |
| expanded | exact_name | `APICAL_WRITE_8` | `APICAL_WRITE_8` | 20 | 43 | 2.150 | +23 | 0->1 | 3->3 |
| expanded | exact_name | `validate_borders` | `validate_borders` | 19 | 96 | 5.053 | +77 | 0->0 | 4->14 |
| expanded | exact_name | `isp_lfb_config_resolution` | `isp_lfb_config_resolution` | 19 | 64 | 3.368 | +45 | 0->0 | 0->0 |
| expanded | exact_name | `isp_lfb_ctrl_hw_recovery` | `isp_lfb_ctrl_hw_recovery` | 19 | 59 | 3.105 | +40 | 0->0 | 0->0 |
| expanded | exact_name | `isp_lfb_ctrl_vsync_wait` | `isp_lfb_ctrl_vsync_wait` | 19 | 58 | 3.053 | +39 | 0->0 | 0->0 |
| expanded | exact_name | `isp_lfb_ctrl_ncu_to_ddr` | `isp_lfb_ctrl_ncu_to_ddr` | 19 | 57 | 3.000 | +38 | 0->0 | 0->0 |
| expanded | exact_name | `flash_time` | `flash_time` | 19 | 38 | 2.000 | +19 | 1->1 | 2->4 |
| expanded | exact_name | `sensor_alloc_integration_time` | `sensor_alloc_integration_time` | 18 | 73 | 4.056 | +55 | 0->0 | 2->6 |
| expanded | exact_name | `gain_log2` | `gain_log2` | 18 | 67 | 3.722 | +49 | 0->0 | 2->4 |
| expanded | exact_name | `dump_isp_framesource_open` | `dump_isp_framesource_open` | 18 | 51 | 2.833 | +33 | 1->2 | 1->0 |
| expanded | exact_name | `dump_isp_mscaler_open` | `dump_isp_mscaler_open` | 18 | 51 | 2.833 | +33 | 1->2 | 1->0 |
| expanded | exact_name | `tx_isp_ldc_open` | `tx_isp_ldc_open` | 18 | 51 | 2.833 | +33 | 1->2 | 1->0 |
| expanded | exact_name | `tx_isp_ncu_open` | `tx_isp_ncu_open` | 18 | 51 | 2.833 | +33 | 1->2 | 1->0 |
| expanded | exact_name | `ae_compensation` | `ae_compensation` | 18 | 48 | 2.667 | +30 | 0->0 | 4->6 |
| expanded | exact_name | `antiflicker_mode` | `antiflicker_mode` | 18 | 47 | 2.611 | +29 | 0->0 | 4->5 |
| expanded | exact_name | `dis_get_default_settings` | `dis_get_default_settings` | 18 | 40 | 2.222 | +22 | 0->0 | 1->1 |
| expanded | exact_name | `apical_api_read_buffer` | `apical_api_read_buffer` | 18 | 37 | 2.056 | +19 | 0->0 | 4->3 |
| expanded | exact_name | `sensor_fps_control` | `sensor_fps_control` | 17 | 93 | 5.471 | +76 | 0->0 | 0->0 |

## Replacement Map

| OEM symbol | Recovered symbol(s) |
|---|---|
| `apical_isp_ae_weight_s_attr.isra.0` | `apical_isp_ae_weight_s_attr_isra_0` |
| `apical_isp_core_ops_s_ctrl` | `apical_isp_ae_weight_s_attr_isra_0`<br>`apical_isp_core_ops_s_ctrl`<br>`apical_isp_gamma_s_attr_isra_0`<br>`apical_isp_stab_s_attr_isra_0` |
| `apical_isp_gamma_s_attr.isra.0` | `apical_isp_gamma_s_attr_isra_0` |
| `apical_isp_stab_s_attr.isra.0` | `apical_isp_stab_s_attr_isra_0` |
