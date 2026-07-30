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
#include <linux/slab.h>
#include <linux/string.h>
#include <linux/errno.h>

#include "../include/tx_isp/tx_isp_sinfo.h"

static const struct tx_isp_sinfo_config tx_isp_sinfo_config = {
	.flags = TX_ISP_SINFO_EXTENDED_ATTRS,
	.static_chip_id = 0x530444,
	.static_i2c_adapter = 0,
	/*
	 * Report the sensor's native mode, not the configured stream size.
	 * Raptor uses these values to decide whether the frame-source scaler is
	 * required. Treating the native 2560x1440 input as 1920x1080 makes it
	 * consume the ISP buffers with the wrong geometry.
	 */
	.static_width = 2560,
	.static_height = 1440,
	.static_fps = 25,
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

/*
 * The original four-slot array occupied 0x280 bytes at the recovered core's
 * BSS tail. Keep that anonymous tail overlay in place for recovered lifecycle
 * writes, but give procfs a stable heap snapshot because the core later
 * repurposes the overlay. The common implementation is kept size-neutral for
 * T41 so all allocatable linked-section boundaries remain unchanged.
 */
#define TX_ISP_SINFO_BSS_COMPAT_SLOTS	1
#define TX_ISP_SINFO_STABLE_PROC_SNAPSHOT	1
/*
 * Keep the static-metadata choice as a runtime read. Constant-folding it
 * removes the recovered dynamic branch and shifts T41's linked sections.
 */
#define TX_ISP_SINFO_CONFIG_FLAGS \
	(TX_ISP_SINFO_EXTENDED_ATTRS | \
	 ((*(volatile unsigned int *)&tx_isp_sinfo_stats.magic) ? \
	  TX_ISP_SINFO_STATIC_METADATA : 0U))
#include "../common/tx_isp_sinfo.c"
