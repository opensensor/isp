/*
 * T41 recovered-object adapter for the shared sensor registry.
 *
 * These offsets are part of the prebuilt T41 sensor-module ABI. Registry
 * ownership and procfs behavior live in driver/common/tx_isp_sinfo.c.
 */

#include <linux/module.h>
#include <linux/i2c.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/mutex.h>
#include <linux/string.h>
#include <linux/errno.h>

#include "../include/tx_isp/tx_isp_sinfo.h"

static const struct tx_isp_sinfo_config tx_isp_sinfo_config = {
	.flags = TX_ISP_SINFO_EXTENDED_ATTRS,
	.client_offset = 0x10c,
	.attr_offset = 0x308,
	.width_offset = 0x2d4,
	.height_offset = 0x2d8,
	.fps_offset = 0x318,
	.min_fps_offset = 0x31c,
	.max_fps_offset = 0x320,
	.adapter_nr_offset = 0x190,
	.attr_name_offset = 0,
	.attr_chip_id_offset = 4,
	.attr_mclk_offset = 0x184,
	.attr_boot_offset = 0x188,
	.attr_interface_offset = 0x180,
	.attr_rst_gpio_offset = 0x170,
	.attr_pwdn_gpio_offset = 0x174,
};

#include "../common/tx_isp_sinfo.c"
