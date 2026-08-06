#include <linux/module.h>
#include <linux/i2c.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/mutex.h>
#include <linux/slab.h>
#include <linux/string.h>
#include <linux/errno.h>

#include "../include/tx_isp/tx_isp_sinfo.h"
#include "include/tx_isp_common.h"

#define T31_SENSOR_VIDEO_OFFSET(member) \
	(offsetof(struct tx_isp_sensor, video) + \
	 offsetof(struct tx_isp_video_in, member))

static const struct tx_isp_sinfo_config tx_isp_sinfo_config = {
	.client_offset = offsetof(struct tx_isp_sensor, sd) +
			 offsetof(struct tx_isp_subdev, dev_priv),
	.attr_offset = T31_SENSOR_VIDEO_OFFSET(attr),
	.width_offset = T31_SENSOR_VIDEO_OFFSET(mbus) +
			offsetof(struct v4l2_mbus_framefmt, width),
	.height_offset = T31_SENSOR_VIDEO_OFFSET(mbus) +
			 offsetof(struct v4l2_mbus_framefmt, height),
	.fps_offset = T31_SENSOR_VIDEO_OFFSET(fps),
	.adapter_nr_offset = offsetof(struct i2c_adapter, nr),
	.attr_name_offset = offsetof(struct tx_isp_sensor_attribute, name),
	.attr_chip_id_offset =
		offsetof(struct tx_isp_sensor_attribute, chip_id),
};

#include "../common/tx_isp_sinfo.c"
