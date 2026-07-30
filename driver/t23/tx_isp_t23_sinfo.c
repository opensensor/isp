/*
 * T23 adapter for the shared sensor registry.
 *
 * The recovered T23 pipeline still owns sensor-client creation and cached
 * sensor state. Lifecycle callbacks preserve those side effects while common
 * code owns slot management, exported ABI functions, and procfs publication.
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

void tx_isp_t23_sinfo_driver_added(struct i2c_driver *drv,
				   int default_i2c_addr,
				   struct module *owner);
void tx_isp_t23_sinfo_driver_removing(struct i2c_driver *drv);
void tx_isp_t23_sinfo_sensor_bound(void *subdev, struct module *owner);
void tx_isp_t23_sinfo_sensor_unbound(void *subdev, struct module *owner);

static const struct tx_isp_sinfo_config tx_isp_sinfo_config = {
	.flags = TX_ISP_SINFO_STATIC_METADATA,
	.static_chip_id = 0x2336,
	.static_i2c_adapter = 0,
	.static_width = 1920,
	.static_height = 1080,
	.static_fps = 25,
	.driver_added = tx_isp_t23_sinfo_driver_added,
	.driver_removing = tx_isp_t23_sinfo_driver_removing,
	.sensor_bound = tx_isp_t23_sinfo_sensor_bound,
	.sensor_unbound = tx_isp_t23_sinfo_sensor_unbound,
};

#include "../common/tx_isp_sinfo.c"
