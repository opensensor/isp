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
	/* T23 SDK object offsets; report the sensor that actually bound. */
	.client_offset = 0xd4,
	.attr_offset = 0x270,
	.width_offset = 0x23c,
	.height_offset = 0x240,
	.fps_offset = 0x27c,
	.adapter_nr_offset = 0x190,
	.attr_name_offset = 0,
	.attr_chip_id_offset = 4,
	.driver_added = tx_isp_t23_sinfo_driver_added,
	.driver_removing = tx_isp_t23_sinfo_driver_removing,
	.sensor_bound = tx_isp_t23_sinfo_sensor_bound,
	.sensor_unbound = tx_isp_t23_sinfo_sensor_unbound,
};

#include "../common/tx_isp_sinfo.c"
