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

static int t31_sinfo_read_module_param_int(struct module *owner,
					   const char *name, int *value)
{
	unsigned int i;

	if (!owner || !name || !value)
		return -EINVAL;

	for (i = 0; i < owner->num_kp; ++i) {
		const struct kernel_param *param = &owner->kp[i];
		const char *param_name;
		char text[32];
		int len;

		if (!param->name || !param->ops || !param->ops->get)
			continue;
		param_name = strrchr(param->name, '.');
		param_name = param_name ? param_name + 1 : param->name;
		if (strcmp(param_name, name))
			continue;

		len = param->ops->get(text, param);
		if (len < 0)
			return len;
		if (len >= (int)sizeof(text))
			return -EOVERFLOW;
		text[len] = '\0';
		return kstrtoint(text, 0, value);
	}

	return -ENOENT;
}

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
	.read_module_param_int = t31_sinfo_read_module_param_int,
};

#include "../common/tx_isp_sinfo.c"
