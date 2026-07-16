# Binary Assembly Audit

- schema: `regtrace-binary-audit-v1`
- OEM: `/home/matteius/re-framework/tx-isp-t23.ko`
- recovered: `/home/matteius/re-framework/open-tx-isp/driver/t23/tx_isp_t23_recovered.ko`
- objdump counts exclude relocation records
- thresholds: min_oem_insns=24 stub_insns=8 collapse=0.50 similar=0.80..1.25 expansion=2.00

## Summary

| Metric | OEM | Recovered |
|---|---:|---:|
| Function symbols | 1043 | 1331 |
| Functions with disassembly | 1043 | 1331 |
| Executable section bytes | 443376 | 383456 |
| Initialized writable bytes | 216768 | 196736 |
| Uninitialized writable bytes | 164688 | 929888 |

- direct matches: 947
- replacement matches: 35 (missing=0)
- unmatched: OEM-only=61 recovered-only=348
- matched instructions: OEM=106302 recovered=77540 ratio=0.729
- classes: stub=33 collapsed=77 shorter=186 same_count=199 similar=397 larger=47 expanded=43

## Allocated Section Delta

| Section | OEM bytes | Recovered bytes | Delta |
|---|---:|---:|---:|
| `.MIPS.abiflags` | 24 | 24 | +0 |
| `.bss` | 164688 | 929888 | +765200 |
| `.data` | 216480 | 196448 | -20032 |
| `.exit.text` | 1184 | 0 | -1184 |
| `.gnu.linkonce.this_module` | 288 | 288 | +0 |
| `.init.text` | 16 | 0 | -16 |
| `.modinfo` | 1260 | 6232 | +4972 |
| `.note.gnu.build-id` | 36 | 36 | +0 |
| `.reginfo` | 24 | 24 | +0 |
| `.rodata` | 6720 | 75696 | +68976 |
| `.rodata.str1.4` | 22728 | 21924 | -804 |
| `.text` | 442176 | 382128 | -60048 |
| `.text.unlikely` | 0 | 1328 | +1328 |
| `__ksymtab` | 208 | 176 | -32 |
| `__ksymtab_strings` | 537 | 477 | -60 |
| `__param` | 224 | 2448 | +2224 |

## Function Outliers

Showing 250 of 795 outliers. JSON and CSV contain every comparison row.

| Class | Match | OEM symbol | Recovered symbol(s) | OEM insns | Recovered insns | Ratio | Delta | Calls | Branches |
|---|---|---|---|---:|---:|---:|---:|---:|---:|
| stub | exact_name | `Tiziano_adr_fpga` | `Tiziano_adr_fpga` | 3210 | 2 | 0.001 | -3208 | 148->0 | 263->0 |
| stub | exact_name | `tisp_defog_soft_process` | `tisp_defog_soft_process` | 1121 | 2 | 0.002 | -1119 | 22->0 | 134->0 |
| stub | exact_name | `frame_channel_unlocked_ioctl` | `frame_channel_unlocked_ioctl` | 899 | 2 | 0.002 | -897 | 56->0 | 171->0 |
| stub | exact_name | `tx_isp_vic_start` | `tx_isp_vic_start` | 606 | 5 | 0.008 | -601 | 13->0 | 75->1 |
| stub | exact_name | `ispcore_pad_event_handle` | `ispcore_pad_event_handle` | 548 | 4 | 0.007 | -544 | 26->0 | 87->1 |
| stub | exact_name | `ispcore_core_ops_init` | `ispcore_core_ops_init` | 501 | 8 | 0.016 | -493 | 18->0 | 129->2 |
| stub | exact_name | `dump_ivdc_regs` | `dump_ivdc_regs` | 433 | 2 | 0.005 | -431 | 46->0 | 1->0 |
| stub | exact_name | `tisp_ae1_process_impl` | `tisp_ae1_process_impl` | 273 | 2 | 0.007 | -271 | 11->0 | 26->0 |
| stub | exact_name | `tisp_msca_api_set_line` | `tisp_msca_api_set_line` | 273 | 2 | 0.007 | -271 | 22->0 | 23->0 |
| stub | exact_name | `tx_isp_probe` | `tx_isp_probe` | 235 | 2 | 0.009 | -233 | 23->0 | 25->0 |
| stub | exact_name | `csi_core_ops_init` | `csi_core_ops_init` | 188 | 5 | 0.027 | -183 | 6->0 | 22->1 |
| stub | exact_name | `ispcore_slake_module` | `ispcore_slake_module` | 114 | 2 | 0.018 | -112 | 6->0 | 18->0 |
| stub | exact_name | `dump_csi_reg` | `dump_csi_reg` | 100 | 2 | 0.020 | -98 | 14->0 | 1->0 |
| stub | exact_name | `private_log2_int_to_fixed_64` | `private_log2_int_to_fixed_64` | 96 | 4 | 0.042 | -92 | 4->0 | 8->1 |
| stub | exact_name | `isp_frame_done_wait` | `isp_frame_done_wait` | 82 | 4 | 0.049 | -78 | 3->0 | 8->0 |
| stub | exact_name | `ispcore_irq_fs_work` | `ispcore_irq_fs_work` | 79 | 6 | 0.076 | -73 | 1->0 | 12->0 |
| stub | exact_name | `isp_malloc_buffer` | `isp_malloc_buffer` | 72 | 2 | 0.028 | -70 | 4->0 | 10->0 |
| stub | exact_name | `tisp_stream_on` | `tisp_stream_on` | 71 | 2 | 0.028 | -69 | 6->0 | 1->0 |
| stub | exact_name | `tx_isp_notify` | `tx_isp_notify` | 69 | 2 | 0.029 | -67 | 1->0 | 12->0 |
| stub | exact_name | `__vb2_queue_cancel` | `__vb2_queue_cancel` | 67 | 2 | 0.030 | -65 | 5->0 | 5->0 |
| stub | exact_name | `tx_isp_video_link_stream` | `tx_isp_video_link_stream` | 64 | 2 | 0.031 | -62 | 2->0 | 12->0 |
| stub | exact_name | `ivdc_slake_module` | `ivdc_slake_module` | 63 | 6 | 0.095 | -57 | 4->0 | 9->1 |
| stub | exact_name | `tx_isp_subdev_deinit` | `tx_isp_subdev_deinit` | 59 | 5 | 0.085 | -54 | 8->0 | 6->1 |
| stub | exact_name | `vic_core_s_stream` | `vic_core_s_stream` | 50 | 5 | 0.100 | -45 | 3->0 | 8->1 |
| stub | exact_name | `ispcore_sensor_ops_ioctl` | `ispcore_sensor_ops_ioctl` | 46 | 2 | 0.043 | -44 | 1->0 | 8->0 |
| stub | exact_name | `tx_vic_disable_irq` | `tx_vic_disable_irq` | 45 | 2 | 0.044 | -43 | 3->0 | 6->0 |
| stub | exact_name | `fs_activate_module` | `fs_activate_module` | 43 | 2 | 0.047 | -41 | 1->0 | 7->0 |
| stub | exact_name | `tx_vic_enable_irq` | `tx_vic_enable_irq` | 42 | 2 | 0.048 | -40 | 3->0 | 5->0 |
| stub | exact_name | `isp_irq_thread_handle` | `isp_irq_thread_handle` | 39 | 2 | 0.051 | -37 | 2->0 | 7->0 |
| stub | exact_name | `tx_isp_vin_remove` | `tx_isp_vin_remove` | 35 | 4 | 0.114 | -31 | 4->0 | 1->1 |
| stub | exact_name | `__enqueue_in_driver` | `__enqueue_in_driver` | 32 | 5 | 0.156 | -27 | 2->0 | 2->0 |
| stub | exact_name | `tx_isp_frame_chan_deinit` | `tx_isp_frame_chan_deinit` | 29 | 2 | 0.069 | -27 | 3->0 | 2->0 |
| stub | exact_name | `csi_video_s_stream` | `csi_video_s_stream` | 26 | 5 | 0.192 | -21 | 1->0 | 5->1 |
| collapsed | exact_name | `tiziano_adr_algorithm` | `tiziano_adr_algorithm` | 3274 | 702 | 0.214 | -2572 | 1->0 | 207->83 |
| collapsed | exact_name | `Tiziano_Awb_Ct_Detect` | `Tiziano_Awb_Ct_Detect` | 1845 | 235 | 0.127 | -1610 | 35->6 | 197->25 |
| collapsed | exact_name | `ae0_tune2` | `ae0_tune2` | 1535 | 31 | 0.020 | -1504 | 91->2 | 172->1 |
| collapsed | exact_name | `apical_isp_core_ops_s_ctrl` | `apical_isp_core_ops_s_ctrl` | 1295 | 616 | 0.476 | -679 | 59->30 | 274->133 |
| collapsed | exact_name | `tiziano_defog_init` | `tiziano_defog_init` | 1181 | 429 | 0.363 | -752 | 127->50 | 53->15 |
| collapsed | exact_name | `tiziano_adr_params_init` | `tiziano_adr_params_init` | 1158 | 242 | 0.209 | -916 | 126->41 | 2->0 |
| collapsed | exact_name | `tiziano_adr_init` | `tiziano_adr_init` | 981 | 257 | 0.262 | -724 | 62->31 | 61->12 |
| collapsed | exact_name | `tisp_msca_ch_curve_write` | `tisp_msca_ch_curve_write` | 762 | 102 | 0.134 | -660 | 120->3 | 15->4 |
| collapsed | exact_name | `Tiziano_awb_fpga` | `Tiziano_awb_fpga` | 724 | 208 | 0.287 | -516 | 10->9 | 70->16 |
| collapsed | exact_name | `isp_vic_cmd_set` | `isp_vic_cmd_set` | 678 | 22 | 0.032 | -656 | 64->1 | 78->1 |
| collapsed | exact_name | `tisp_code_tuning_ioctl` | `tisp_code_tuning_ioctl` | 643 | 37 | 0.058 | -606 | 34->1 | 89->5 |
| collapsed | exact_name | `tisp_init` | `tisp_init` | 605 | 297 | 0.491 | -308 | 95->40 | 33->24 |
| collapsed | exact_name | `ispcore_interrupt_service_routine` | `ispcore_interrupt_service_routine` | 471 | 204 | 0.433 | -267 | 24->11 | 54->29 |
| collapsed | exact_name | `tiziano_load_parameters` | `tiziano_load_parameters` | 463 | 128 | 0.276 | -335 | 51->17 | 46->8 |
| collapsed | exact_name | `frame_chan_event` | `frame_chan_event` | 453 | 91 | 0.201 | -362 | 21->5 | 50->9 |
| collapsed | exact_name | `ivdc_pad_event_handle` | `ivdc_pad_event_handle` | 424 | 13 | 0.031 | -411 | 11->0 | 62->1 |
| collapsed | exact_name | `tisp_ae0_process_impl` | `tisp_ae0_process_impl` | 421 | 49 | 0.116 | -372 | 18->1 | 36->6 |
| collapsed | exact_name | `proc_ivdc_writel` | `proc_ivdc_writel` | 353 | 67 | 0.190 | -286 | 29->4 | 40->7 |
| collapsed | exact_name | `ae0_weight_mean2` | `ae0_weight_mean2` | 326 | 111 | 0.340 | -215 | 7->5 | 6->6 |
| collapsed | exact_name | `tisp_lsc_write_lut_datas` | `tisp_lsc_write_lut_datas` | 325 | 93 | 0.286 | -232 | 7->13 | 25->2 |
| collapsed | exact_name | `tisp_adr_param_array_set` | `tisp_adr_param_array_set` | 293 | 139 | 0.474 | -154 | 50->25 | 3->1 |
| collapsed | exact_name | `tisp_core_switch_bin` | `tisp_core_switch_bin` | 274 | 115 | 0.420 | -159 | 33->14 | 24->7 |
| collapsed | exact_name | `isp_framesource_show` | `isp_framesource_show` | 269 | 48 | 0.178 | -221 | 27->3 | 23->5 |
| collapsed | exact_name | `tiziano_ct_ccm_interpolation` | `tiziano_ct_ccm_interpolation` | 257 | 108 | 0.420 | -149 | 1->1 | 40->16 |
| collapsed | exact_name | `tiziano_ae_init` | `tiziano_ae_init` | 249 | 30 | 0.120 | -219 | 27->3 | 9->1 |
| collapsed | exact_name | `Tiziano_ae0_fpga` | `Tiziano_ae0_fpga` | 241 | 103 | 0.427 | -138 | 6->4 | 21->9 |
| collapsed | exact_name | `tx_isp_fs_probe` | `tx_isp_fs_probe` | 233 | 54 | 0.232 | -179 | 21->6 | 21->2 |
| collapsed | exact_name | `tisp_ae_manual_set` | `tisp_ae_manual_set` | 227 | 89 | 0.392 | -138 | 10->8 | 12->12 |
| collapsed | exact_name | `tisp_vic_ctrl_ioctl` | `tisp_vic_ctrl_ioctl` | 206 | 10 | 0.049 | -196 | 16->1 | 31->0 |
| collapsed | replacement | `tisp_sdns_gaussian_k_cfg` | `regtrace_t23_sdns_gaussian_k_cfg` | 202 | 16 | 0.079 | -186 | 64->1 | 0->1 |
| collapsed | exact_name | `tisp_s_osd_block_attr` | `tisp_s_osd_block_attr` | 168 | 59 | 0.351 | -109 | 3->1 | 10->1 |
| collapsed | exact_name | `tiziano_ae_init_exp_th` | `tiziano_ae_init_exp_th` | 166 | 30 | 0.181 | -136 | 4->1 | 6->2 |
| collapsed | exact_name | `ispcore_irq_thread_handle` | `ispcore_irq_thread_handle` | 164 | 34 | 0.207 | -130 | 9->2 | 25->5 |
| collapsed | exact_name | `subdev_sensor_ops_set_input` | `subdev_sensor_ops_set_input` | 147 | 10 | 0.068 | -137 | 10->0 | 29->3 |
| collapsed | exact_name | `printf_func0` | `printf_func0` | 139 | 55 | 0.396 | -84 | 12->3 | 14->4 |
| collapsed | replacement | `tisp_sdns_gaussian_x_cfg` | `regtrace_t23_sdns_gaussian_x_cfg` | 138 | 16 | 0.116 | -122 | 32->1 | 0->1 |
| collapsed | exact_name | `tx_isp_ivdc_probe` | `tx_isp_ivdc_probe` | 135 | 17 | 0.126 | -118 | 13->2 | 9->0 |
| collapsed | exact_name | `tx_isp_csi_probe` | `tx_isp_csi_probe` | 134 | 17 | 0.127 | -117 | 14->2 | 9->0 |
| collapsed | exact_name | `isp_core_cmd_set` | `isp_core_cmd_set` | 131 | 42 | 0.321 | -89 | 8->4 | 22->3 |
| collapsed | exact_name | `vic_sensor_ops_ioctl` | `vic_sensor_ops_ioctl` | 118 | 16 | 0.136 | -102 | 6->0 | 21->2 |
| collapsed | exact_name | `tisp_ae_target` | `tisp_ae_target` | 117 | 43 | 0.368 | -74 | 4->3 | 13->3 |
| collapsed | exact_name | `ispcore_core_ops_ioctl` | `ispcore_core_ops_ioctl` | 116 | 37 | 0.319 | -79 | 3->2 | 27->7 |
| collapsed | exact_name | `tisp_af_set_attr_refresh` | `tisp_af_set_attr_refresh` | 112 | 45 | 0.402 | -67 | 0->5 | 1->0 |
| collapsed | replacement | `tisp_sdns_r_s_mv_cfg` | `regtrace_t23_sdns_r_s_mv_cfg` | 106 | 38 | 0.358 | -68 | 16->3 | 0->3 |
| collapsed | exact_name | `tisp_ae_g_scene_luma` | `tisp_ae_g_scene_luma` | 106 | 42 | 0.396 | -64 | 4->0 | 4->5 |
| collapsed | exact_name | `fix_point_mult2_64` | `fix_point_mult2_64` | 104 | 41 | 0.394 | -63 | 5->5 | 0->0 |
| collapsed | exact_name | `tiziano_lsc_init` | `tiziano_lsc_init` | 101 | 42 | 0.416 | -59 | 8->4 | 5->3 |
| collapsed | exact_name | `frame_channel_vidioc_set_fmt` | `frame_channel_vidioc_set_fmt` | 100 | 23 | 0.230 | -77 | 7->3 | 17->3 |
| collapsed | exact_name | `tx_isp_vic_probe` | `tx_isp_vic_probe` | 96 | 17 | 0.177 | -79 | 11->2 | 4->0 |
| collapsed | exact_name | `tisp_defog_wdr_en` | `tisp_defog_wdr_en` | 81 | 17 | 0.210 | -64 | 2->2 | 3->1 |
| collapsed | replacement | `tisp_sdns_sp_d_b_wei_np_array_cfg` | `regtrace_t23_sdns_sp_d_b_wei_np_array_cfg` | 80 | 31 | 0.388 | -49 | 6->1 | 0->2 |
| collapsed | replacement | `tisp_sdns_sp_d_w_wei_np_array_cfg` | `regtrace_t23_sdns_sp_d_w_wei_np_array_cfg` | 80 | 31 | 0.388 | -49 | 6->1 | 0->2 |
| collapsed | replacement | `tisp_sdns_sp_ud_b_wei_np_array_cfg` | `regtrace_t23_sdns_sp_ud_b_wei_np_array_cfg` | 80 | 31 | 0.388 | -49 | 6->1 | 0->2 |
| collapsed | replacement | `tisp_sdns_sp_ud_w_wei_np_array_cfg` | `regtrace_t23_sdns_sp_ud_w_wei_np_array_cfg` | 80 | 31 | 0.388 | -49 | 6->1 | 0->2 |
| collapsed | exact_name | `tisp_msca_set_omi_api` | `tisp_msca_set_omi_api` | 79 | 37 | 0.468 | -42 | 9->5 | 5->1 |
| collapsed | exact_name | `tiziano_ae_dn_params_refresh` | `tiziano_ae_dn_params_refresh` | 73 | 29 | 0.397 | -44 | 7->3 | 0->1 |
| collapsed | exact_name | `frame_channel_vidioc_get_fmt` | `frame_channel_vidioc_get_fmt` | 69 | 30 | 0.435 | -39 | 5->2 | 7->2 |
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
| collapsed | exact_name | `dump_vic_reg` | `dump_vic_reg` | 32 | 15 | 0.469 | -17 | 1->1 | 1->0 |
| collapsed | exact_name | `tx_isp_vin_activate_subdev` | `tx_isp_vin_activate_subdev` | 29 | 10 | 0.345 | -19 | 2->0 | 1->1 |
| collapsed | exact_name | `tisp_ae0_process` | `tisp_ae0_process` | 29 | 13 | 0.448 | -16 | 3->2 | 2->1 |
| collapsed | exact_name | `tx_isp_disable_irq` | `tx_isp_disable_irq` | 28 | 10 | 0.357 | -18 | 3->1 | 1->0 |
| collapsed | exact_name | `tx_isp_enable_irq` | `tx_isp_enable_irq` | 27 | 10 | 0.370 | -17 | 3->1 | 1->0 |
| collapsed | exact_name | `tisp_ae_trig` | `tisp_ae_trig` | 27 | 12 | 0.444 | -15 | 1->1 | 1->0 |
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
| shorter | exact_name | `apical_isp_core_ops_g_ctrl` | `apical_isp_core_ops_g_ctrl` | 975 | 543 | 0.557 | -432 | 52->26 | 219->134 |
| shorter | exact_name | `JZ_Isp_Awb` | `JZ_Isp_Awb` | 483 | 256 | 0.530 | -227 | 6->5 | 57->23 |
| shorter | exact_name | `tiziano_bcsh_update` | `tiziano_bcsh_update` | 455 | 347 | 0.763 | -108 | 6->16 | 26->17 |
| shorter | replacement | `tisp_mdns_c_2d_param_cfg` | `regtrace_t23_mdns_c_2d_param_cfg.isra.0` | 422 | 334 | 0.791 | -88 | 31->30 | 2->1 |
| shorter | exact_name | `tiziano_bcsh_lut_parameter` | `tiziano_bcsh_lut_parameter` | 373 | 238 | 0.638 | -135 | 29->29 | 13->4 |
| shorter | exact_name | `tisp_ae1_expt` | `tisp_ae1_expt` | 351 | 183 | 0.521 | -168 | 23->21 | 30->14 |
| shorter | exact_name | `tisp_ae_algo_handle` | `tisp_ae_algo_handle` | 322 | 240 | 0.745 | -82 | 40->32 | 8->13 |
| shorter | exact_name | `video_input_cmd_set` | `video_input_cmd_set` | 308 | 236 | 0.766 | -72 | 20->13 | 50->39 |
| shorter | exact_name | `tisp_defog_param_array_set` | `tisp_defog_param_array_set` | 260 | 203 | 0.781 | -57 | 46->38 | 1->1 |
| shorter | exact_name | `tisp_clm_interp_by_ct` | `tisp_clm_interp_by_ct` | 254 | 145 | 0.571 | -109 | 8->6 | 35->17 |
| shorter | exact_name | `cm_control` | `cm_control` | 229 | 135 | 0.590 | -94 | 9->1 | 15->9 |
| shorter | exact_name | `Tiziano_af_fpga` | `Tiziano_af_fpga` | 229 | 156 | 0.681 | -73 | 13->11 | 5->5 |
| shorter | exact_name | `tisp_ae0_get_hist` | `tisp_ae0_get_hist` | 206 | 158 | 0.767 | -48 | 5->5 | 22->17 |
| shorter | exact_name | `tisp_mdns_set_malloc_cfg` | `tisp_mdns_set_malloc_cfg` | 200 | 145 | 0.725 | -55 | 32->32 | 4->4 |
| shorter | exact_name | `tisp_msca_init_chx_cfg` | `tisp_msca_init_chx_cfg` | 199 | 147 | 0.739 | -52 | 17->26 | 8->9 |
| shorter | exact_name | `tisp_msca_normalized` | `tisp_msca_normalized` | 196 | 138 | 0.704 | -58 | 4->1 | 22->15 |
| shorter | exact_name | `tisp_af_param_array_get` | `tisp_af_param_array_get` | 169 | 114 | 0.675 | -55 | 19->19 | 0->0 |
| shorter | exact_name | `tiziano_bcsh_Toffset_RGBYUV` | `tiziano_bcsh_Toffset_RGBYUV` | 154 | 107 | 0.695 | -47 | 9->3 | 9->8 |
| shorter | exact_name | `tisp_s_adr_str_internal` | `tisp_s_adr_str_internal` | 146 | 75 | 0.514 | -71 | 1->0 | 7->6 |
| shorter | exact_name | `tisp_msca_write_reg` | `tisp_msca_write_reg` | 143 | 113 | 0.790 | -30 | 4->11 | 16->6 |
| shorter | exact_name | `tisp_msca_api_set_scaler_level_control` | `tisp_msca_api_set_scaler_level_control` | 131 | 91 | 0.695 | -40 | 7->6 | 7->6 |
| shorter | exact_name | `tisp_log2_int_to_fixed_64` | `tisp_log2_int_to_fixed_64` | 129 | 93 | 0.721 | -36 | 3->1 | 16->14 |
| shorter | exact_name | `tisp_bcsh_set_attr` | `tisp_bcsh_set_attr` | 128 | 92 | 0.719 | -36 | 17->11 | 4->5 |
| shorter | exact_name | `jz_isp_ccm` | `jz_isp_ccm` | 128 | 99 | 0.773 | -29 | 5->5 | 13->10 |
| shorter | exact_name | `tiziano_af_params_refresh` | `tiziano_af_params_refresh` | 124 | 93 | 0.750 | -31 | 19->19 | 0->0 |
| shorter | exact_name | `tisp_msca_api_set_mask` | `tisp_msca_api_set_mask` | 123 | 75 | 0.610 | -48 | 8->8 | 4->2 |
| shorter | exact_name | `tisp_vic_mmap` | `tisp_vic_mmap` | 123 | 95 | 0.772 | -28 | 9->8 | 14->8 |
| shorter | exact_name | `tisp_ae0_ctrls_update` | `tisp_ae0_ctrls_update` | 123 | 97 | 0.789 | -26 | 1->1 | 12->8 |
| shorter | exact_name | `tisp_msca_api_set_osd` | `tisp_msca_api_set_osd` | 117 | 69 | 0.590 | -48 | 14->14 | 2->0 |
| shorter | exact_name | `tisp_set_ae0_ag` | `tisp_set_ae0_ag` | 113 | 82 | 0.726 | -31 | 6->6 | 7->3 |
| shorter | exact_name | `fix_point_intp` | `fix_point_intp` | 102 | 54 | 0.529 | -48 | 4->4 | 12->4 |
| shorter | exact_name | `tisp_lsc_lut_valid_judge` | `tisp_lsc_lut_valid_judge` | 102 | 70 | 0.686 | -32 | 2->1 | 14->10 |
| shorter | exact_name | `vic_mdma_enable` | `vic_mdma_enable` | 100 | 79 | 0.790 | -21 | 0->0 | 6->3 |
| shorter | exact_name | `Tiziano_awb_set_gain` | `Tiziano_awb_set_gain` | 99 | 71 | 0.717 | -28 | 7->6 | 4->2 |
| shorter | exact_name | `fix_point_div_64` | `fix_point_div_64` | 94 | 51 | 0.543 | -43 | 3->3 | 9->3 |
| shorter | exact_name | `tiziano_set_parameter_clm` | `tiziano_set_parameter_clm` | 91 | 49 | 0.538 | -42 | 6->6 | 4->1 |
| shorter | exact_name | `ispcore_sync_sensor_attr` | `ispcore_sync_sensor_attr` | 91 | 60 | 0.659 | -31 | 4->4 | 7->7 |
| shorter | exact_name | `tisp_s_wb_mode` | `tisp_s_wb_mode` | 90 | 54 | 0.600 | -36 | 1->2 | 12->3 |
| shorter | exact_name | `tisp_msca_para_calc` | `tisp_msca_para_calc` | 86 | 62 | 0.721 | -24 | 0->6 | 17->10 |
| shorter | exact_name | `tisp_s_dpc_str_internal` | `tisp_s_dpc_str_internal` | 85 | 49 | 0.576 | -36 | 0->0 | 6->2 |
| shorter | exact_name | `tisp_msca_api_set_fcrop` | `tisp_msca_api_set_fcrop` | 82 | 61 | 0.744 | -21 | 6->5 | 3->3 |
| shorter | exact_name | `tisp_csccr_update_para` | `tisp_csccr_update_para` | 80 | 58 | 0.725 | -22 | 0->0 | 6->7 |
| shorter | exact_name | `tisp_dpc_s_par_cfg` | `tisp_dpc_s_par_cfg` | 79 | 62 | 0.785 | -17 | 5->5 | 4->4 |
| shorter | exact_name | `isp_vic_frd_show` | `isp_vic_frd_show` | 78 | 48 | 0.615 | -30 | 3->3 | 6->5 |
| shorter | exact_name | `tisp_defog_max_filter3` | `tisp_defog_max_filter3` | 78 | 62 | 0.795 | -16 | 0->1 | 14->7 |
| shorter | exact_name | `tisp_ae_s_min` | `tisp_ae_s_min` | 72 | 46 | 0.639 | -26 | 3->3 | 6->1 |
| shorter | exact_name | `tx_isp_subdev_pipo` | `tx_isp_subdev_pipo` | 70 | 45 | 0.643 | -25 | 1->0 | 5->3 |
| shorter | exact_name | `tx_isp_vin_slake_subdev` | `tx_isp_vin_slake_subdev` | 68 | 54 | 0.794 | -14 | 6->5 | 7->6 |
| shorter | exact_name | `tisp_adr_set_params` | `tisp_adr_set_params` | 67 | 50 | 0.746 | -17 | 5->5 | 3->3 |
| shorter | exact_name | `tiziano_defog_set_reg_params` | `tiziano_defog_set_reg_params` | 66 | 49 | 0.742 | -17 | 1->1 | 2->2 |
| shorter | exact_name | `subdev_sensor_ops_enum_input` | `subdev_sensor_ops_enum_input` | 64 | 38 | 0.594 | -26 | 2->0 | 10->8 |
| shorter | exact_name | `tiziano_gamma_lut_parameter` | `tiziano_gamma_lut_parameter` | 63 | 45 | 0.714 | -18 | 3->3 | 1->2 |
| shorter | exact_name | `tiziano_csccr_init` | `tiziano_csccr_init` | 62 | 45 | 0.726 | -17 | 4->2 | 0->0 |
| shorter | exact_name | `tisp_csccr_para_refresh_by_mode` | `tisp_csccr_para_refresh_by_mode` | 58 | 45 | 0.776 | -13 | 0->0 | 9->6 |
| shorter | exact_name | `private_math_exp2` | `private_math_exp2` | 57 | 42 | 0.737 | -15 | 1->1 | 2->2 |
| shorter | exact_name | `tisp_math_exp2` | `tisp_math_exp2` | 57 | 42 | 0.737 | -15 | 1->1 | 2->2 |
| shorter | exact_name | `frame_channel_open` | `frame_channel_open` | 57 | 44 | 0.772 | -13 | 7->4 | 4->3 |
| shorter | exact_name | `tisp_lsc_judge_ct_update_flag` | `tisp_lsc_judge_ct_update_flag` | 56 | 37 | 0.661 | -19 | 0->0 | 14->8 |
| shorter | exact_name | `tx_isp_vic_slake_subdev` | `tx_isp_vic_slake_subdev` | 56 | 42 | 0.750 | -14 | 4->2 | 7->7 |
| shorter | exact_name | `tisp_hldc_strength_adjust_k` | `tisp_hldc_strength_adjust_k` | 54 | 35 | 0.648 | -19 | 2->0 | 0->0 |
| shorter | exact_name | `func_zone_ct_weight` | `func_zone_ct_weight` | 54 | 42 | 0.778 | -12 | 2->2 | 7->7 |
| shorter | exact_name | `tiziano_gib_deir_reg` | `tiziano_gib_deir_reg` | 49 | 33 | 0.673 | -16 | 3->1 | 1->2 |
| shorter | exact_name | `tisp_dpc_d_m3_par_cfg` | `tisp_dpc_d_m3_par_cfg` | 49 | 39 | 0.796 | -10 | 3->3 | 0->0 |
| shorter | exact_name | `tx_isp_core_remove` | `tx_isp_core_remove` | 48 | 37 | 0.771 | -11 | 7->5 | 2->1 |
| shorter | exact_name | `isp_tunning_read` | `isp_tunning_read` | 45 | 34 | 0.756 | -11 | 2->2 | 4->4 |
| shorter | exact_name | `tisp_s_ae_attr` | `tisp_s_ae_attr` | 45 | 35 | 0.778 | -10 | 3->3 | 2->1 |
| shorter | exact_name | `vic_core_ops_init` | `vic_core_ops_init` | 43 | 28 | 0.651 | -15 | 2->1 | 7->4 |
| shorter | exact_name | `vin_s_stream` | `vin_s_stream` | 42 | 26 | 0.619 | -16 | 1->1 | 11->5 |
| shorter | exact_name | `tisp_hv_flip_enable` | `tisp_hv_flip_enable` | 41 | 27 | 0.659 | -14 | 2->1 | 2->1 |
| shorter | exact_name | `tisp_gib_deir_ir_update` | `tisp_gib_deir_ir_update` | 41 | 30 | 0.732 | -11 | 1->1 | 4->4 |
| shorter | exact_name | `tisp_s_BacklightComp` | `tisp_s_BacklightComp` | 41 | 31 | 0.756 | -10 | 4->3 | 0->0 |
| shorter | exact_name | `tisp_s_Hilightdepress` | `tisp_s_Hilightdepress` | 41 | 31 | 0.756 | -10 | 4->3 | 0->0 |
| shorter | exact_name | `tisp_csccr_api_set` | `tisp_csccr_api_set` | 40 | 29 | 0.725 | -11 | 5->4 | 0->0 |
| shorter | exact_name | `table_intp` | `table_intp` | 39 | 28 | 0.718 | -11 | 1->1 | 4->3 |
| shorter | exact_name | `mbus_to_bayer_write` | `mbus_to_bayer_write` | 38 | 25 | 0.658 | -13 | 2->2 | 6->2 |
| shorter | exact_name | `tisp_g_BacklightComp` | `tisp_g_BacklightComp` | 38 | 30 | 0.789 | -8 | 2->1 | 5->5 |
| shorter | exact_name | `tisp_g_Hilightdepress` | `tisp_g_Hilightdepress` | 38 | 30 | 0.789 | -8 | 2->1 | 5->5 |

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
| `tiziano_defog_params_refresh` | `regtrace_t23_source_defog_load_tuning`<br>`tiziano_defog_params_refresh` |
| `tiziano_sdns_params_refresh` | `regtrace_t23_source_sdns_load_tuning` |
