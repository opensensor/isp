#ifdef __KERNEL__
#include <linux/errno.h>
#else
#include <errno.h>
#endif

#include "tx_isp_t31_sensor_policy.h"

int tx_isp_t31_sensor_active_dimensions(
	const struct tx_isp_t31_sensor_policy *sensor, u32 *width, u32 *height)
{
	u32 active_width;
	u32 active_height;

	if (!sensor || !width || !height)
		return -EINVAL;

	active_width = sensor->mbus_width;
	active_height = sensor->mbus_height;
	if (!active_width || !active_height) {
		if (sensor->dbus_type != TX_ISP_T31_DBUS_MIPI ||
		    !sensor->mipi_width || !sensor->mipi_height)
			return -EINVAL;
		active_width = sensor->mipi_width;
		active_height = sensor->mipi_height;
	}

	if (!active_width || !active_height)
		return -EINVAL;

	*width = active_width;
	*height = active_height;
	return 0;
}

int tx_isp_t31_sensor_fps_q8(u32 raw_fps, u32 *fps_q8)
{
	u32 numerator;
	u32 denominator;
	u32 integer;
	u32 remainder;

	if (!fps_q8)
		return -EINVAL;

	numerator = raw_fps >> 16;
	denominator = raw_fps & 0xffffU;
	if (!numerator || !denominator)
		return -EINVAL;

	integer = numerator / denominator;
	remainder = numerator % denominator;
	*fps_q8 = (integer << 8) + ((remainder << 8) / denominator);
	return 0;
}

int tx_isp_t31_wdr_buffer_layout(
	const struct tx_isp_t31_sensor_policy *sensor,
	u32 *size, u32 *stride, u32 *lines)
{
	u32 active_width;
	u32 active_height;
	u64 required;
	int ret;

	if (!sensor || !size || !stride || !lines)
		return -EINVAL;
	if (sensor->data_type != TX_ISP_T31_DATA_TYPE_WDR_FS &&
	    sensor->data_type != TX_ISP_T31_DATA_TYPE_WDR_DOL)
		return -ENODATA;

	ret = tx_isp_t31_sensor_active_dimensions(sensor, &active_width,
						     &active_height);
	if (ret)
		return ret;
	if (active_width > (~(u32)0) / 2U)
		return -EOVERFLOW;

	*stride = active_width << 1;
	if (sensor->data_type == TX_ISP_T31_DATA_TYPE_WDR_FS) {
		required = (u64)(*stride) * active_height;
		if (required > ~(u32)0)
			return -EOVERFLOW;
		*size = (u32)required;
		*lines = active_height;
		return 0;
	}

	if (sensor->data_type == TX_ISP_T31_DATA_TYPE_WDR_DOL) {
		if (!sensor->wdr_cache)
			return -EINVAL;
		*size = sensor->wdr_cache;
		*lines = sensor->wdr_cache / *stride;
		if (!*lines)
			return -EINVAL;
		return 0;
	}

	return -ENODATA;
}
