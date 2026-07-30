CC ?= cc
CFLAGS ?= -O2
CFLAGS += -std=c99 -Wall -Wextra -Werror

.PHONY: all check clean

all: tx_isp_math_test tx_isp_subdev_abi_test tx_isp_reg_profile_test \
	tx_isp_callback_plan_test

tx_isp_math_test: tx_isp_math_test.c ../driver/include/tx_isp/tx_isp_math.h
	$(CC) $(CFLAGS) -I../driver/include -o $@ $<

tx_isp_subdev_abi_test: tx_isp_subdev_abi_test.c ../driver/include/tx_isp/tx_isp_subdev_abi.h
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

check: tx_isp_math_test tx_isp_subdev_abi_test tx_isp_reg_profile_test \
		tx_isp_callback_plan_test
	./tx_isp_math_test
	./tx_isp_subdev_abi_test
	./tx_isp_reg_profile_test
	./tx_isp_callback_plan_test

clean:
	$(RM) tx_isp_math_test tx_isp_subdev_abi_test tx_isp_reg_profile_test \
		tx_isp_callback_plan_test
