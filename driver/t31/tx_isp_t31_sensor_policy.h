#ifndef TX_ISP_T31_SENSOR_POLICY_H
#define TX_ISP_T31_SENSOR_POLICY_H

#ifdef __KERNEL__
#include <linux/types.h>
#else
#include <stdint.h>
typedef uint32_t u32;
typedef uint64_t u64;
#endif

/* Values are part of the public T31 sensor-module ABI. */
#define TX_ISP_T31_DBUS_MIPI             1U
#define TX_ISP_T31_DATA_TYPE_LINEAR       0U
#define TX_ISP_T31_DATA_TYPE_WDR_FS       1U
#define TX_ISP_T31_DATA_TYPE_WDR_DOL      2U

struct tx_isp_t31_sensor_policy {
	u32 mbus_width;
	u32 mbus_height;
	u32 dbus_type;
	u32 mipi_width;
	u32 mipi_height;
	u32 raw_fps;
	u32 data_type;
	u32 wdr_cache;
};

int tx_isp_t31_sensor_active_dimensions(
	const struct tx_isp_t31_sensor_policy *sensor, u32 *width, u32 *height);
int tx_isp_t31_sensor_fps_q8(u32 raw_fps, u32 *fps_q8);
int tx_isp_t31_wdr_buffer_layout(
	const struct tx_isp_t31_sensor_policy *sensor,
	u32 *size, u32 *stride, u32 *lines);
u32 tx_isp_t31_ae_scene_strength(u32 calibrated_strength,
				 int requested_level);

#endif /* TX_ISP_T31_SENSOR_POLICY_H */
