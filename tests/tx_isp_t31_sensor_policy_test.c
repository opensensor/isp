#include <assert.h>
#include <errno.h>
#include <stdio.h>

#include "../driver/t31/tx_isp_t31_sensor_policy.h"

static void test_active_dimensions_prefer_video_mode(void)
{
	const struct tx_isp_t31_sensor_policy sensor = {
		.mbus_width = 1280,
		.mbus_height = 720,
		.dbus_type = TX_ISP_T31_DBUS_MIPI,
		.mipi_width = 1920,
		.mipi_height = 1080,
	};
	u32 width = 0;
	u32 height = 0;

	assert(tx_isp_t31_sensor_active_dimensions(&sensor, &width, &height) == 0);
	assert(width == 1280);
	assert(height == 720);
}

static void test_mipi_dimensions_are_only_a_mipi_fallback(void)
{
	struct tx_isp_t31_sensor_policy sensor = {
		.dbus_type = TX_ISP_T31_DBUS_MIPI,
		.mipi_width = 2304,
		.mipi_height = 1296,
	};
	u32 width = 0;
	u32 height = 0;

	assert(tx_isp_t31_sensor_active_dimensions(&sensor, &width, &height) == 0);
	assert(width == 2304);
	assert(height == 1296);

	sensor.dbus_type = 2;
	assert(tx_isp_t31_sensor_active_dimensions(&sensor, &width, &height) ==
	       -EINVAL);

	sensor.dbus_type = TX_ISP_T31_DBUS_MIPI;
	sensor.mbus_width = 1280;
	assert(tx_isp_t31_sensor_active_dimensions(&sensor, &width, &height) == 0);
	assert(width == 2304);
	assert(height == 1296);
}

static void test_fps_uses_oem_q8_conversion(void)
{
	u32 fps_q8 = 0;

	assert(tx_isp_t31_sensor_fps_q8((25U << 16) | 1U, &fps_q8) == 0);
	assert(fps_q8 == (25U << 8));
	assert(tx_isp_t31_sensor_fps_q8((30000U << 16) | 1001U,
					&fps_q8) == 0);
	assert(fps_q8 == 7672U);
	assert(tx_isp_t31_sensor_fps_q8(25U << 16, &fps_q8) == -EINVAL);
}

static void test_frame_stitch_wdr_uses_active_geometry(void)
{
	const struct tx_isp_t31_sensor_policy sensor = {
		.mbus_width = 2560,
		.mbus_height = 1440,
		.data_type = TX_ISP_T31_DATA_TYPE_WDR_FS,
	};
	u32 size = 0;
	u32 stride = 0;
	u32 lines = 0;

	assert(tx_isp_t31_wdr_buffer_layout(&sensor, &size, &stride,
					     &lines) == 0);
	assert(size == 2560U * 1440U * 2U);
	assert(stride == 2560U * 2U);
	assert(lines == 1440U);
}

static void test_dol_wdr_uses_sensor_cache_size(void)
{
	struct tx_isp_t31_sensor_policy sensor = {
		.mbus_width = 1920,
		.mbus_height = 1080,
		.data_type = TX_ISP_T31_DATA_TYPE_WDR_DOL,
		.wdr_cache = 0x240000,
	};
	u32 size = 0;
	u32 stride = 0;
	u32 lines = 0;

	assert(tx_isp_t31_wdr_buffer_layout(&sensor, &size, &stride,
					     &lines) == 0);
	assert(size == sensor.wdr_cache);
	assert(stride == 3840U);
	assert(lines == sensor.wdr_cache / stride);

	sensor.data_type = TX_ISP_T31_DATA_TYPE_LINEAR;
	assert(tx_isp_t31_wdr_buffer_layout(&sensor, &size, &stride,
					     &lines) == -ENODATA);
}

int main(void)
{
	test_active_dimensions_prefer_video_mode();
	test_mipi_dimensions_are_only_a_mipi_fallback();
	test_fps_uses_oem_q8_conversion();
	test_frame_stitch_wdr_uses_active_geometry();
	test_dol_wdr_uses_sensor_cache_size();
	puts("tx_isp_t31_sensor_policy tests passed");
	return 0;
}
