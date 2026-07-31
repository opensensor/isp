CC ?= cc
CFLAGS ?= -O2
CFLAGS += -std=c99 -Wall -Wextra -Werror

.PHONY: all check clean

all: tx_isp_math_test tx_isp_subdev_abi_test tx_isp_subdev_test \
	tx_isp_remote_event_test \
	tx_isp_state_test \
	tx_isp_reg_profile_test \
	tx_isp_callback_plan_test tx_isp_tuning_abi_test \
	tx_isp_frame_abi_test tx_isp_frame_channel_test \
	tx_isp_frame_format_test \
	tx_isp_frame_layout_test \
	tx_isp_exposure_test tx_isp_scaler_test

tx_isp_math_test: tx_isp_math_test.c ../driver/include/tx_isp/tx_isp_math.h
	$(CC) $(CFLAGS) -I../driver/include -o $@ $<

tx_isp_subdev_abi_test: tx_isp_subdev_abi_test.c ../driver/include/tx_isp/tx_isp_subdev_abi.h
	$(CC) $(CFLAGS) -I../driver/include -o $@ $<

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

tx_isp_exposure_test: tx_isp_exposure_test.c \
		../driver/t31/tx_isp_t31_exposure.c \
		../driver/t31/tx_isp_t31_exposure.h \
		../driver/common/tx_isp_exposure.c \
		../driver/include/tx_isp/tx_isp_exposure.h
	$(CC) $(CFLAGS) -I../driver/include -o $@ \
		tx_isp_exposure_test.c ../driver/common/tx_isp_exposure.c \
		../driver/t31/tx_isp_t31_exposure.c

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

check: tx_isp_math_test tx_isp_subdev_abi_test tx_isp_subdev_test \
		tx_isp_remote_event_test \
		tx_isp_state_test \
		tx_isp_reg_profile_test \
		tx_isp_callback_plan_test tx_isp_tuning_abi_test \
		tx_isp_frame_abi_test tx_isp_frame_channel_test \
		tx_isp_frame_format_test \
		tx_isp_frame_layout_test \
		tx_isp_exposure_test tx_isp_scaler_test
	./tx_isp_math_test
	./tx_isp_subdev_abi_test
	./tx_isp_subdev_test
	./tx_isp_remote_event_test
	./tx_isp_state_test
	./tx_isp_reg_profile_test
	./tx_isp_callback_plan_test
	./tx_isp_tuning_abi_test
	./tx_isp_frame_abi_test
	./tx_isp_frame_channel_test
	./tx_isp_frame_format_test
	./tx_isp_frame_layout_test
	./tx_isp_exposure_test
	./tx_isp_scaler_test

clean:
	$(RM) tx_isp_math_test tx_isp_subdev_abi_test tx_isp_subdev_test \
		tx_isp_remote_event_test \
		tx_isp_state_test \
		tx_isp_reg_profile_test \
		tx_isp_callback_plan_test tx_isp_tuning_abi_test \
		tx_isp_frame_abi_test tx_isp_frame_channel_test \
		tx_isp_frame_format_test \
		tx_isp_frame_layout_test \
		tx_isp_exposure_test tx_isp_scaler_test
