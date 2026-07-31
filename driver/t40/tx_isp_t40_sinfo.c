/*
 * T40 adapter for the shared Thingino sensor registry.
 *
 * The offsets below are the T40 SDK's binary ABI.  They are derived from the
 * exact 4.4.94 Thingino SDK headers with the target MIPS toolchain.  Wiring is
 * stored in the sensor's inline register-info object; T40 video state does not
 * contain the min_fps and max_fps fields present on T41.
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
	.flags = TX_ISP_SINFO_REGINFO_WIRING,
	.client_offset = 0x10c,
	.attr_offset = 0x304,
	.width_offset = 0x2d0,
	.height_offset = 0x2d4,
	.fps_offset = 0x314,
	.adapter_nr_offset = 0x190,
	.attr_name_offset = 0,
	.attr_chip_id_offset = 4,
	.info_offset = 0x128,
	.info_mclk_offset = 0x5c,
	.info_boot_offset = 0x60,
	.info_interface_offset = 0x58,
	.info_rst_gpio_offset = 0x48,
	.info_pwdn_gpio_offset = 0x4c,
};

#include "../common/tx_isp_sinfo.c"

extern int tx_isp_t40_core_init(void);
extern void tx_isp_t40_core_exit(void);

int init_module(void)
{
	int ret;

	ret = tx_isp_t40_core_init();
	if (ret)
		return ret;

	ret = tx_isp_sinfo_init();
	if (ret)
		tx_isp_t40_core_exit();
	return ret;
}

void cleanup_module(void)
{
	tx_isp_sinfo_exit();
	tx_isp_t40_core_exit();
}
