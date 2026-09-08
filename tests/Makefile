CC ?= cc
.DEFAULT_GOAL := all
CFLAGS ?= -O2
CFLAGS += -std=c99 -Wall -Wextra -Werror

.PHONY: all check clean t30_storage_audit

all check: tx_isp_t41_ysp_test
all check: tx_isp_t41_cdns_test
all check: tx_isp_t41_mdns_test
all check: tx_isp_t41_lce_test
all check: tx_isp_t41_adr_test
all check: tx_isp_t41_lsc_test
all check: tx_isp_t41_lsc_runtime_test
all check: tx_isp_t41_awb_stats_test
all check: tx_isp_t41_awb_long_test
all check: tx_isp_t41_awb_detect_test
all check: tx_isp_t41_awb_frame_test
all check: tx_isp_t41_awb_control_test
tx_isp_t41_awb_control_test: tx_isp_t41_awb_control_test.c t41_awb_control_oracle_check.c ../driver/t41/tx_isp_t41_awb_control.h ../driver/t41/tx_isp_t41_awb.h ../driver/t41/tx_isp_t41_awb_gray.h ../driver/t41/tx_isp_t41_awb_ct.h ../driver/t41/tx_isp_t41_awb_prior.h ../driver/t41/tx_isp_t41_awb_stats.h ../driver/t41/tx_isp_t41_awb_gain.h ../driver/t41/tx_isp_t41_ae.h ../driver/t41/tx_isp_t41_tmo.h
	$(CC) $(CFLAGS) -o $@ $<

tx_isp_t41_awb_frame_test: tx_isp_t41_awb_frame_test.c t41_awb_frame_oracle_check.c ../driver/t41/tx_isp_t41_awb_long.h ../driver/t41/tx_isp_t41_awb_special.h ../driver/t41/tx_isp_t41_awb_detect.h ../driver/t41/tx_isp_t41_awb_cluster.h ../driver/t41/tx_isp_t41_awb_gray.h ../driver/t41/tx_isp_t41_awb_prior.h ../driver/t41/tx_isp_t41_awb_stats.h ../driver/t41/tx_isp_t41_awb_gain.h ../driver/t41/tx_isp_t41_awb_ct.h ../driver/t41/tx_isp_t41_ae.h ../driver/t41/tx_isp_t41_tmo.h
	$(CC) $(CFLAGS) -o $@ $<

tx_isp_t41_awb_detect_test: tx_isp_t41_awb_detect_test.c t41_awb_detect_oracle_check.c ../driver/t41/tx_isp_t41_awb_detect.h ../driver/t41/tx_isp_t41_awb_cluster.h ../driver/t41/tx_isp_t41_awb_gray.h ../driver/t41/tx_isp_t41_awb_prior.h ../driver/t41/tx_isp_t41_awb_stats.h ../driver/t41/tx_isp_t41_awb_gain.h ../driver/t41/tx_isp_t41_awb_ct.h ../driver/t41/tx_isp_t41_ae.h ../driver/t41/tx_isp_t41_tmo.h
	$(CC) $(CFLAGS) -o $@ $<

tx_isp_t41_awb_long_test: tx_isp_t41_awb_long_test.c ../driver/t41/tx_isp_t41_awb_long.h ../driver/t41/tx_isp_t41_awb_prior.h ../driver/t41/tx_isp_t41_awb_stats.h ../driver/t41/tx_isp_t41_awb_gain.h ../driver/t41/tx_isp_t41_ae.h ../driver/t41/tx_isp_t41_tmo.h
	$(CC) $(CFLAGS) -o $@ $<

tx_isp_t41_awb_stats_test: ../driver/t41/tx_isp_t41_awb_gray.h

tx_isp_t41_awb_stats_test: tx_isp_t41_awb_stats_test.c ../driver/t41/tx_isp_t41_awb_stats.h ../driver/t41/tx_isp_t41_awb_prior.h ../driver/t41/tx_isp_t41_awb_special.h ../driver/t41/tx_isp_t41_awb_ct.h ../driver/t41/tx_isp_t41_awb_gain.h ../driver/t41/tx_isp_t41_ae.h ../driver/t41/tx_isp_t41_tmo.h
	$(CC) $(CFLAGS) -o $@ $<

tx_isp_t41_lsc_runtime_test: tx_isp_t41_lsc_runtime_test.c ../driver/t41/tx_isp_t41_lsc_runtime.inc ../driver/t41/tx_isp_t41_lsc.h ../driver/t41/tx_isp_t41_dpc.h ../driver/t41/tx_isp_t41_tmo.h ../driver/include/tx_isp/tx_isp_math.h
	$(CC) $(CFLAGS) -no-pie -o $@ $<

tx_isp_t41_lsc_test: tx_isp_t41_lsc_test.c ../driver/t41/tx_isp_t41_lsc.h ../driver/t41/tx_isp_t41_dpc.h ../driver/t41/tx_isp_t41_tmo.h ../driver/include/tx_isp/tx_isp_math.h
	$(CC) $(CFLAGS) -o $@ $<

tx_isp_t41_adr_test: tx_isp_t41_adr_test.c ../driver/t41/tx_isp_t41_adr.h ../driver/t41/tx_isp_t41_ae.h ../driver/t41/tx_isp_t41_dpc.h ../driver/t41/tx_isp_t41_tmo.h ../driver/include/tx_isp/tx_isp_math.h
	$(CC) $(CFLAGS) -o $@ $<

tx_isp_t41_lce_test: tx_isp_t41_lce_test.c ../driver/t41/tx_isp_t41_lce.h ../driver/t41/tx_isp_t41_dpc.h ../driver/t41/tx_isp_t41_tmo.h ../driver/include/tx_isp/tx_isp_math.h
	$(CC) $(CFLAGS) -o $@ $<

tx_isp_t41_mdns_test: tx_isp_t41_mdns_test.c ../driver/t41/tx_isp_t41_mdns.h ../driver/t41/tx_isp_t41_dpc.h ../driver/t41/tx_isp_t41_tmo.h ../driver/include/tx_isp/tx_isp_math.h
	$(CC) $(CFLAGS) -o $@ $<

tx_isp_t41_cdns_test: tx_isp_t41_cdns_test.c ../driver/t41/tx_isp_t41_cdns.h ../driver/t41/tx_isp_t41_dpc.h ../driver/t41/tx_isp_t41_tmo.h ../driver/include/tx_isp/tx_isp_math.h
	$(CC) $(CFLAGS) -o $@ $<

tx_isp_t41_ysp_test: tx_isp_t41_ysp_test.c ../driver/t41/tx_isp_t41_ysp.h ../driver/t41/tx_isp_t41_dpc.h ../driver/t41/tx_isp_t41_tmo.h ../driver/include/tx_isp/tx_isp_math.h
	$(CC) $(CFLAGS) -o $@ $<

all check: tx_isp_t41_gib_test tx_isp_t41_awb_gain_test tx_isp_t41_sensor_gain_test tx_isp_t41_gamma_test tx_isp_t41_dpc_test tx_isp_t41_dmsc_test tx_isp_t41_sdns_test tx_isp_t41_ydns_test

tx_isp_t41_ydns_test: tx_isp_t41_ydns_test.c ../driver/t41/tx_isp_t41_ydns.h ../driver/t41/tx_isp_t41_dpc.h ../driver/t41/tx_isp_t41_tmo.h ../driver/include/tx_isp/tx_isp_math.h
	$(CC) $(CFLAGS) -o $@ $<

tx_isp_t41_sdns_test: tx_isp_t41_sdns_test.c ../driver/t41/tx_isp_t41_sdns.h ../driver/t41/tx_isp_t41_dpc.h ../driver/t41/tx_isp_t41_tmo.h ../driver/include/tx_isp/tx_isp_math.h
	$(CC) $(CFLAGS) -o $@ $<

tx_isp_t41_dmsc_test: tx_isp_t41_dmsc_test.c ../driver/t41/tx_isp_t41_dmsc.h ../driver/t41/tx_isp_t41_dpc.h ../driver/t41/tx_isp_t41_tmo.h ../driver/include/tx_isp/tx_isp_math.h
	$(CC) $(CFLAGS) -o $@ $<

tx_isp_t41_dpc_test: tx_isp_t41_dpc_test.c ../driver/t41/tx_isp_t41_dpc.h ../driver/t41/tx_isp_t41_tmo.h ../driver/include/tx_isp/tx_isp_math.h
	$(CC) $(CFLAGS) -o $@ $<

tx_isp_t41_gamma_test: tx_isp_t41_gamma_test.c ../driver/t41/tx_isp_t41_gamma.h ../driver/t41/tx_isp_t41_tmo.h
	$(CC) $(CFLAGS) -o $@ $<

tx_isp_t41_sensor_gain_test: tx_isp_t41_sensor_gain_test.c ../driver/t41/tx_isp_t41_sensor_gain.h ../driver/include/tx_isp/tx_isp_math.h
	$(CC) $(CFLAGS) -o $@ $<

tx_isp_t41_awb_gain_test: tx_isp_t41_awb_gain_test.c ../driver/t41/tx_isp_t41_awb_gain.h ../driver/t41/tx_isp_t41_ae.h
	$(CC) $(CFLAGS) -o $@ $<

tx_isp_t41_gib_test: tx_isp_t41_gib_test.c ../driver/t41/tx_isp_t41_gib.h
	$(CC) $(CFLAGS) -o $@ $<

tx_isp_t41_color_route_test: tx_isp_t41_color_route_test.c ../driver/t41/tx_isp_t41_exposure.c ../driver/t41/tx_isp_t41_exposure.h ../driver/include/tx_isp/tx_isp_top.h
	$(CC) $(CFLAGS) -o $@ $< ../driver/t41/tx_isp_t41_exposure.c

all: tx_isp_math_test tx_isp_modulation_test tx_isp_daynight_test tx_isp_awb_mesh_test tx_isp_t41_tmo_test tx_isp_t41_awb_test tx_isp_t41_tmo_map_test tx_isp_t41_ccm_test tx_isp_t41_bcsh_test tx_isp_t41_ae_test tx_isp_t41_color_route_test \
	tx_isp_subdev_abi_test tx_isp_subdev_test \
	tx_isp_remote_event_test \
	tx_isp_state_test \
	tx_isp_sinfo_abi_test \
	tx_isp_sensor_format_test \
	tx_isp_reg_profile_test \
	tx_isp_callback_plan_test tx_isp_tuning_abi_test \
	tx_isp_frame_abi_test tx_isp_frame_channel_test \
	tx_isp_frame_format_test \
		tx_isp_frame_layout_test \
		tx_isp_video_queue_test \
		tx_isp_region_allocator_test \
	tx_isp_exposure_test tx_isp_t31_adr_test tx_isp_t31_mdns_test \
	tx_isp_t31_sensor_policy_test \
	tx_isp_scaler_test

tx_isp_t31_mdns_test: tx_isp_t31_mdns_test.c \
		../driver/t31/tx_isp_t31_mdns.c \
		../driver/t31/tx_isp_t31_mdns.h
	$(CC) $(CFLAGS) -I../driver/include -o $@ \
		tx_isp_t31_mdns_test.c ../driver/t31/tx_isp_t31_mdns.c

tx_isp_t31_sensor_policy_test: tx_isp_t31_sensor_policy_test.c \
		../driver/t31/tx_isp_t31_sensor_policy.c \
		../driver/t31/tx_isp_t31_sensor_policy.h
	$(CC) $(CFLAGS) -I../driver/include -o $@ \
		tx_isp_t31_sensor_policy_test.c \
		../driver/t31/tx_isp_t31_sensor_policy.c

tx_isp_math_test: tx_isp_math_test.c ../driver/include/tx_isp/tx_isp_math.h
	$(CC) $(CFLAGS) -I../driver/include -o $@ $<

tx_isp_t41_tmo_test: tx_isp_t41_tmo_test.c ../driver/t41/tx_isp_t41_tmo.h
	$(CC) $(CFLAGS) -o $@ $<

tx_isp_t41_ccm_test: tx_isp_t41_ccm_test.c ../driver/t41/tx_isp_t41_ccm.h ../driver/t41/tx_isp_t41_tmo.h
	$(CC) $(CFLAGS) -o $@ $<

tx_isp_t41_bcsh_test: tx_isp_t41_bcsh_test.c ../driver/t41/tx_isp_t41_bcsh.h ../driver/t41/tx_isp_t41_ccm.h ../driver/t41/tx_isp_t41_tmo.h
	$(CC) $(CFLAGS) -o $@ $<

tx_isp_t41_ae_test: tx_isp_t41_ae_test.c ../driver/t41/tx_isp_t41_ae.h ../driver/t41/tx_isp_t41_tmo.h
	$(CC) $(CFLAGS) -o $@ $<

tx_isp_t41_tmo_map_test: tx_isp_t41_tmo_map_test.c ../driver/t41/tx_isp_t41_tmo_map.h \
		../driver/t41/tx_isp_t41_tmo.h ../driver/t41/tx_isp_t41_tmo_kernels.h
	$(CC) $(CFLAGS) -o $@ $<

tx_isp_t41_awb_test: tx_isp_t41_awb_test.c ../driver/t41/tx_isp_t41_awb.h ../driver/include/tx_isp/tx_isp_math.h
	$(CC) $(CFLAGS) -o $@ $<

tx_isp_awb_mesh_test: tx_isp_awb_mesh_test.c ../driver/common/tx_isp_awb_mesh.c \
		../driver/include/tx_isp/tx_isp_awb_mesh.h
	$(CC) $(CFLAGS) -I../driver/include -o $@ \
		tx_isp_awb_mesh_test.c ../driver/common/tx_isp_awb_mesh.c

tx_isp_modulation_test: tx_isp_modulation_test.c \
		../driver/include/tx_isp/tx_isp_modulation.h
	$(CC) $(CFLAGS) -I../driver/include -o $@ $<

tx_isp_daynight_test: tx_isp_daynight_test.c \
		../driver/common/tx_isp_daynight.c \
		../driver/include/tx_isp/tx_isp_daynight.h
	$(CC) $(CFLAGS) -I../driver/include -o $@ \
		tx_isp_daynight_test.c ../driver/common/tx_isp_daynight.c

tx_isp_subdev_abi_test: tx_isp_subdev_abi_test.c \
		../driver/common/tx_isp_subdev.c \
		../driver/include/tx_isp/tx_isp_subdev_abi.h \
		../driver/include/tx_isp/tx_isp_subdev.h
	$(CC) $(CFLAGS) -I../driver/include -o $@ \
		tx_isp_subdev_abi_test.c ../driver/common/tx_isp_subdev.c

tx_isp_subdev_test: tx_isp_subdev_test.c \
		../driver/common/tx_isp_subdev.c \
		../driver/include/tx_isp/tx_isp_subdev.h
	$(CC) $(CFLAGS) -I../driver/include -o $@ \
		tx_isp_subdev_test.c ../driver/common/tx_isp_subdev.c

tx_isp_remote_event_test: tx_isp_remote_event_test.c \
		../driver/common/tx_isp_remote_event.c \
		../driver/include/tx_isp/tx_isp_remote_event.h
	$(CC) $(CFLAGS) -I../driver/include -o $@ \
		tx_isp_remote_event_test.c ../driver/common/tx_isp_remote_event.c

tx_isp_state_test: tx_isp_state_test.c \
		../driver/common/tx_isp_state.c \
		../driver/include/tx_isp/tx_isp_state.h
	$(CC) $(CFLAGS) -I../driver/include -o $@ \
		tx_isp_state_test.c ../driver/common/tx_isp_state.c

tx_isp_sinfo_abi_test: tx_isp_sinfo_abi_test.c \
		../driver/include/tx_isp/tx_isp_sinfo.h
	$(CC) $(CFLAGS) -I../driver/include -o $@ $<

tx_isp_sensor_format_test: tx_isp_sensor_format_test.c \
		../driver/include/tx_isp/tx_isp_sensor_format.h
	$(CC) $(CFLAGS) -I../driver/include -o $@ $<

tx_isp_reg_profile_test: tx_isp_reg_profile_test.c \
		../driver/common/tx_isp_reg_profile.c \
		../driver/include/tx_isp/tx_isp_reg_profile.h
	$(CC) $(CFLAGS) -I../driver/include -o $@ \
		tx_isp_reg_profile_test.c ../driver/common/tx_isp_reg_profile.c

tx_isp_callback_plan_test: tx_isp_callback_plan_test.c \
		../driver/common/tx_isp_callback_plan.c \
		../driver/include/tx_isp/tx_isp_callback_plan.h
	$(CC) $(CFLAGS) -I../driver/include -o $@ \
		tx_isp_callback_plan_test.c ../driver/common/tx_isp_callback_plan.c

tx_isp_tuning_abi_test: tx_isp_tuning_abi_test.c \
		../driver/common/tx_isp_tuning_abi.c \
		../driver/include/tx_isp/tx_isp_tuning_abi.h
	$(CC) $(CFLAGS) -I../driver/include -o $@ \
		tx_isp_tuning_abi_test.c ../driver/common/tx_isp_tuning_abi.c

tx_isp_frame_abi_test: tx_isp_frame_abi_test.c \
		../driver/include/tx_isp/tx_isp_frame_abi.h
	$(CC) $(CFLAGS) -I../driver/include -o $@ $<

tx_isp_frame_channel_test: tx_isp_frame_channel_test.c \
		../driver/include/tx_isp/tx_isp_frame_channel.h \
		../driver/include/tx_isp/tx_isp_frame_abi.h \
		../driver/include/tx_isp/tx_isp_frame_format.h
	$(CC) $(CFLAGS) -I../driver/include -o $@ $<

tx_isp_frame_format_test: tx_isp_frame_format_test.c \
		../driver/include/tx_isp/tx_isp_frame_format.h
	$(CC) $(CFLAGS) -I../driver/include -o $@ $<

tx_isp_frame_layout_test: tx_isp_frame_layout_test.c \
		../driver/common/tx_isp_frame_layout.c \
		../driver/include/tx_isp/tx_isp_frame_layout.h \
		../driver/include/tx_isp/tx_isp_frame_abi.h \
		../driver/include/tx_isp/tx_isp_frame_format.h
	$(CC) $(CFLAGS) -I../driver/include -o $@ \
		tx_isp_frame_layout_test.c ../driver/common/tx_isp_frame_layout.c

tx_isp_video_queue_test: tx_isp_video_queue_test.c \
		../driver/common/tx_isp_video_queue.c \
		../driver/include/tx_isp/tx_isp_video_queue.h \
		../driver/include/tx_isp/tx_isp_frame_channel.h \
		../driver/include/tx_isp/tx_isp_frame_abi.h
	$(CC) $(CFLAGS) -I../driver/include -o $@ \
		tx_isp_video_queue_test.c ../driver/common/tx_isp_video_queue.c

tx_isp_region_allocator_test: tx_isp_region_allocator_test.c \
		../driver/common/tx_isp_region_allocator.c \
		../driver/include/tx_isp/tx_isp_region_allocator.h
	$(CC) $(CFLAGS) -I../driver/include -o $@ \
		tx_isp_region_allocator_test.c \
		../driver/common/tx_isp_region_allocator.c

tx_isp_exposure_test: tx_isp_exposure_test.c \
		../driver/t31/tx_isp_t31_exposure.c \
		../driver/t31/tx_isp_t31_exposure.h \
		../driver/common/tx_isp_exposure.c \
		../driver/include/tx_isp/tx_isp_exposure.h
	$(CC) $(CFLAGS) -I../driver/include -o $@ \
		tx_isp_exposure_test.c ../driver/common/tx_isp_exposure.c \
		../driver/t31/tx_isp_t31_exposure.c

tx_isp_t31_adr_test: tx_isp_t31_adr_test.c \
		../driver/t31/tx_isp_t31_adr.c \
		../driver/t31/tx_isp_t31_adr.h
	$(CC) $(CFLAGS) -I../driver/include -o $@ \
		tx_isp_t31_adr_test.c ../driver/t31/tx_isp_t31_adr.c

tx_isp_scaler_test: tx_isp_scaler_test.c \
		../driver/t23/tx_isp_t23_scaler.c \
		../driver/t23/tx_isp_t23_scaler.h \
		../driver/t41/tx_isp_t41_scaler.c \
		../driver/t41/tx_isp_t41_scaler.h \
		../driver/common/tx_isp_scaler.c \
		../driver/include/tx_isp/tx_isp_scaler.h
	$(CC) $(CFLAGS) -I../driver/include -o $@ \
		tx_isp_scaler_test.c ../driver/common/tx_isp_scaler.c \
		../driver/t23/tx_isp_t23_scaler.c \
		../driver/t41/tx_isp_t41_scaler.c

check: tx_isp_math_test tx_isp_modulation_test tx_isp_daynight_test tx_isp_awb_mesh_test tx_isp_t41_tmo_test tx_isp_t41_awb_test tx_isp_t41_tmo_map_test tx_isp_t41_ccm_test tx_isp_t41_bcsh_test tx_isp_t41_ae_test tx_isp_t41_color_route_test \
		tx_isp_subdev_abi_test tx_isp_subdev_test \
		tx_isp_remote_event_test \
		tx_isp_state_test \
		tx_isp_sinfo_abi_test \
		tx_isp_sensor_format_test \
		tx_isp_reg_profile_test \
		tx_isp_callback_plan_test tx_isp_tuning_abi_test \
		tx_isp_frame_abi_test tx_isp_frame_channel_test \
		tx_isp_frame_format_test \
		tx_isp_frame_layout_test \
		tx_isp_video_queue_test \
		tx_isp_region_allocator_test \
		tx_isp_exposure_test tx_isp_t31_adr_test tx_isp_scaler_test \
		tx_isp_t31_mdns_test tx_isp_t31_sensor_policy_test
	./tx_isp_math_test
	./tx_isp_awb_mesh_test
	./tx_isp_t41_tmo_test
	./tx_isp_t41_tmo_map_test
	./tx_isp_t41_ccm_test
	./tx_isp_t41_bcsh_test
	./tx_isp_t41_ae_test
	./tx_isp_t41_gib_test
	./tx_isp_t41_awb_gain_test
	./tx_isp_t41_sensor_gain_test
	./tx_isp_t41_gamma_test
	./tx_isp_t41_dpc_test
	./tx_isp_t41_dmsc_test
	./tx_isp_t41_sdns_test
	./tx_isp_t41_ydns_test
	./tx_isp_t41_ysp_test
	./tx_isp_t41_cdns_test
	./tx_isp_t41_mdns_test
	./tx_isp_t41_lce_test
	./tx_isp_t41_adr_test
	./tx_isp_t41_lsc_test
	./tx_isp_t41_lsc_runtime_test
	./tx_isp_t41_awb_stats_test
	./tx_isp_t41_awb_long_test
	./tx_isp_t41_awb_detect_test
	./tx_isp_t41_awb_frame_test
	./tx_isp_t41_awb_control_test
	./tx_isp_t41_color_route_test
	./tx_isp_t41_awb_test
	./tx_isp_modulation_test
	./tx_isp_daynight_test
	./tx_isp_subdev_abi_test
	./tx_isp_subdev_test
	./tx_isp_remote_event_test
	./tx_isp_state_test
	./tx_isp_sinfo_abi_test
	./tx_isp_sensor_format_test
	./tx_isp_reg_profile_test
	./tx_isp_callback_plan_test
	./tx_isp_tuning_abi_test
	./tx_isp_frame_abi_test
	./tx_isp_frame_channel_test
	./tx_isp_frame_format_test
	./tx_isp_frame_layout_test
	./tx_isp_video_queue_test
	./tx_isp_region_allocator_test
	./tx_isp_exposure_test
	./tx_isp_t31_adr_test
	./tx_isp_t31_mdns_test
	./tx_isp_t31_sensor_policy_test
	./tx_isp_scaler_test
	$(MAKE) t30_storage_audit

t30_storage_audit:
	../driver/t30/check_storage.sh

clean:
	$(RM) tx_isp_t41_awb_frame_test
	$(RM) tx_isp_t41_awb_control_test
	$(RM) tx_isp_t41_awb_detect_test
	$(RM) tx_isp_t41_awb_long_test
	$(RM) tx_isp_t41_awb_stats_test
	$(RM) tx_isp_math_test tx_isp_modulation_test \
		tx_isp_awb_mesh_test \
		tx_isp_t41_tmo_test \
		tx_isp_t41_tmo_map_test \
		tx_isp_t41_awb_test \
		tx_isp_t41_ccm_test tx_isp_t41_bcsh_test tx_isp_t41_ae_test tx_isp_t41_gib_test tx_isp_t41_awb_gain_test tx_isp_t41_sensor_gain_test \
		tx_isp_t41_color_route_test \
		tx_isp_daynight_test \
		tx_isp_subdev_abi_test tx_isp_subdev_test \
		tx_isp_remote_event_test \
		tx_isp_state_test \
		tx_isp_sinfo_abi_test \
		tx_isp_sensor_format_test \
		tx_isp_reg_profile_test \
		tx_isp_callback_plan_test tx_isp_tuning_abi_test \
		tx_isp_frame_abi_test tx_isp_frame_channel_test \
		tx_isp_frame_format_test \
		tx_isp_frame_layout_test \
		tx_isp_video_queue_test \
		tx_isp_region_allocator_test \
		tx_isp_exposure_test tx_isp_t31_adr_test tx_isp_scaler_test \
		tx_isp_t31_mdns_test tx_isp_t31_sensor_policy_test
