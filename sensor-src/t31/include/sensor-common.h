#ifndef __TX_SENSOR_COMMON_H__
#define __TX_SENSOR_COMMON_H__

#include <linux/version.h>
#include <txx_funcs.h>

#if LINUX_VERSION_CODE < KERNEL_VERSION(5, 0, 0)
#include <soc/gpio.h>
#else
#define GPIO_PA(pin) (pin)
#endif

#define SENSOR_R_BLACK_LEVEL 0
#define SENSOR_GR_BLACK_LEVEL 1
#define SENSOR_GB_BLACK_LEVEL 2
#define SENSOR_B_BLACK_LEVEL 3

#define V4L2_I2C_REG_MAX 150
#define V4L2_I2C_ADDR_16BIT 0x0002
#define V4L2_I2C_DATA_16BIT 0x0004
#define V4L2_SBUS_MASK_SAMPLE_8BITS 0x01
#define V4L2_SBUS_MASK_SAMPLE_16BITS 0x02
#define V4L2_SBUS_MASK_SAMPLE_32BITS 0x04
#define V4L2_SBUS_MASK_ADDR_8BITS 0x08
#define V4L2_SBUS_MASK_ADDR_16BITS 0x10
#define V4L2_SBUS_MASK_ADDR_32BITS 0x20
#define V4L2_SBUS_MASK_ADDR_STEP_16BITS 0x40
#define V4L2_SBUS_MASK_ADDR_STEP_32BITS 0x80
#define V4L2_SBUS_MASK_SAMPLE_SWAP_BYTES 0x100
#define V4L2_SBUS_MASK_SAMPLE_SWAP_WORDS 0x200
#define V4L2_SBUS_MASK_ADDR_SWAP_BYTES 0x400
#define V4L2_SBUS_MASK_ADDR_SWAP_WORDS 0x800
#define V4L2_SBUS_MASK_ADDR_SKIP 0x1000
#define V4L2_SBUS_MASK_SPI_READ_MSB_SET 0x2000
#define V4L2_SBUS_MASK_SPI_INVERSE_DATA 0x4000
#define V4L2_SBUS_MASK_SPI_HALF_ADDR 0x8000
#define V4L2_SBUS_MASK_SPI_LSB 0x10000

#ifndef V4L2_MBUS_FMT_SRGGB10_1X10
#define V4L2_MBUS_FMT_SRGGB10_1X10 MEDIA_BUS_FMT_SRGGB10_1X10
#endif

static inline int set_sensor_gpio_function(int func_set)
{
	unsigned long pins;

	switch (func_set) {
	case DVP_PA_LOW_8BIT:
		pins = 0x000340ff;
		break;
	case DVP_PA_HIGH_8BIT:
		pins = 0x00034ff0;
		break;
	case DVP_PA_LOW_10BIT:
		pins = 0x000343ff;
		break;
	case DVP_PA_HIGH_10BIT:
		pins = 0x00034ffc;
		break;
	case DVP_PA_12BIT:
		pins = 0x00034fff;
		break;
	default:
		return -EINVAL;
	}

	return private_jzgpio_set_func(GPIO_PORT_A, GPIO_FUNC_1, pins);
}

#endif
