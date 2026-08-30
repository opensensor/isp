#include <assert.h>
#include <errno.h>
#include <stdint.h>
#include <stdio.h>

#include "tx_isp/tx_isp_frame_layout.h"

static void test_device_geometries(void)
{
	struct tx_isp_nv12_layout layout;

	assert(tx_isp_nv12_layout_build(1920, 1080, 1, 16, &layout) == 0);
	assert(layout.stride == 1920);
	assert(layout.aggregate_line_size == 2880);
	assert(layout.aligned_height == 1088);
	assert(layout.y_size == 2088960);
	assert(layout.sizeimage == 3133440);

	assert(tx_isp_nv12_layout_build(640, 360, 1, 16, &layout) == 0);
	assert(layout.stride == 640);
	assert(layout.aggregate_line_size == 960);
	assert(layout.aligned_height == 368);
	assert(layout.y_size == 235520);
	assert(layout.sizeimage == 353280);

	assert(tx_isp_nv12_layout_build(1919, 1079, 32, 16, &layout) == 0);
	assert(layout.stride == 1920);
	assert(layout.aggregate_line_size == 2880);
	assert(layout.aligned_height == 1088);
	assert(layout.y_size == 2088960);
	assert(layout.sizeimage == 3133440);

	assert(tx_isp_nv12_layout_build(1920, 1080, 8, 1, &layout) == 0);
	assert(layout.stride == 1920);
	assert(layout.aggregate_line_size == 2880);
	assert(layout.aligned_height == 1080);
	assert(layout.y_size == 2073600);
	assert(layout.sizeimage == 3110400);
}

static void test_mdns_device_geometries(void)
{
	struct tx_isp_mdns_layout layout;

	assert(tx_isp_mdns_layout_build(1920, 1080, 1, &layout) == 0);
	assert(layout.y_stride == 1920);
	assert(layout.y_size == 2073600);
	assert(layout.nv12_size == 3110400);
	assert(layout.reference_stride == 64);
	assert(layout.reference_height == 69);
	assert(layout.reference_size == 4416);
	assert(layout.reference_offset[0] == 3110400);
	assert(layout.reference_offset[1] == 3110400);
	assert(layout.reference_offset[2] == 3110400);
	assert(layout.reference_offset[3] == 3110400);
	assert(layout.uv_stride == 0);
	assert(layout.tiny_stride == 0);
	assert(layout.used_size == 3114816);

	assert(tx_isp_mdns_layout_build(1920, 1080, 0, &layout) == 0);
	assert(layout.reference_offset[0] == 3110400);
	assert(layout.reference_offset[1] == 3114816);
	assert(layout.reference_offset[2] == 3119232);
	assert(layout.reference_offset[3] == 3123648);
	assert(layout.uv_stride == 960);
	assert(layout.uv_size == 1036800);
	assert(layout.uv_offset[0] == 3128064);
	assert(layout.uv_offset[1] == 4164864);
	assert(layout.tiny_stride == 64);
	assert(layout.tiny_size == 2160);
	assert(layout.tiny_offset == 4683264);
	assert(layout.used_size == 4685424);

	assert(tx_isp_mdns_layout_build(640, 360, 1, &layout) == 0);
	assert(layout.y_stride == 640);
	assert(layout.nv12_size == 345600);
	assert(layout.reference_stride == 24);
	assert(layout.reference_height == 24);
	assert(layout.reference_size == 576);
	assert(layout.used_size == 346176);
}

static void test_nv12_dma_binding(void)
{
	struct tx_isp_nv12_buffer buffer;

	assert(tx_isp_nv12_buffer_build(1920, 1080, 1, 16,
					0x6300000U, 3133440,
					&buffer) == 0);
	assert(buffer.layout.stride == 1920);
	assert(buffer.layout.aligned_height == 1088);
	assert(buffer.layout.sizeimage == 3133440);
	assert(buffer.y_dma == 0x6300000U);
	assert(buffer.uv_dma == 0x64fe000U);

	assert(tx_isp_nv12_buffer_build(640, 360, 32, 16,
					0x6600000U, 353280,
					&buffer) == 0);
	assert(buffer.layout.stride == 640);
	assert(buffer.layout.aligned_height == 368);
	assert(buffer.y_dma == 0x6600000U);
	assert(buffer.uv_dma == 0x6639800U);
}

static void test_validation_and_overflow(void)
{
	struct tx_isp_nv12_layout layout = { 1, 2, 3, 4, 5 };
	struct tx_isp_nv12_buffer buffer = {
		.layout = { 6, 7, 8, 9, 10 },
		.y_dma = 11,
		.uv_dma = 12,
	};
	struct tx_isp_mdns_layout mdns = {
		.y_stride = 11,
		.used_size = 22,
	};

	assert(tx_isp_nv12_layout_build(0, 1080, 1, 16, &layout) ==
	       -EINVAL);
	assert(tx_isp_nv12_layout_build(1920, 0, 1, 16, &layout) ==
	       -EINVAL);
	assert(tx_isp_nv12_layout_build(1920, 1080, 0, 16, &layout) ==
	       -EINVAL);
	assert(tx_isp_nv12_layout_build(1920, 1080, 3, 16, &layout) ==
	       -EINVAL);
	assert(tx_isp_nv12_layout_build(0xffffffffU, 1, 32, 1, &layout) ==
	       -EOVERFLOW);
	assert(tx_isp_nv12_layout_build(0x80000000U, 3, 1, 1, &layout) ==
	       -EOVERFLOW);
	assert(tx_isp_nv12_layout_build(1, 1, 1, 1, NULL) == -EINVAL);

	/* Failed builds do not publish a partial layout. */
	assert(layout.stride == 1);
	assert(layout.aggregate_line_size == 2);
	assert(layout.aligned_height == 3);
	assert(layout.y_size == 4);
	assert(layout.sizeimage == 5);

	assert(tx_isp_nv12_buffer_build(1920, 1080, 1, 16,
					0x6300000U, 3133439,
					&buffer) == -ENOSPC);
	assert(tx_isp_nv12_buffer_build(1920, 1080, 1, 16,
					0xfff00000U, 3133440,
					&buffer) == -EOVERFLOW);
	assert(tx_isp_nv12_buffer_build(1920, 1080, 1, 16,
					0x6300000U, 3133440,
					NULL) == -EINVAL);
	assert(buffer.layout.stride == 6);
	assert(buffer.layout.aggregate_line_size == 7);
	assert(buffer.layout.aligned_height == 8);
	assert(buffer.layout.y_size == 9);
	assert(buffer.layout.sizeimage == 10);
	assert(buffer.y_dma == 11);
	assert(buffer.uv_dma == 12);

	assert(tx_isp_mdns_layout_build(0, 1080, 0, &mdns) == -EINVAL);
	assert(tx_isp_mdns_layout_build(1920, 0, 0, &mdns) == -EINVAL);
	assert(tx_isp_mdns_layout_build(0xffffffffU, 0xffffffffU, 0,
					&mdns) == -EOVERFLOW);
	assert(tx_isp_mdns_layout_build(1920, 1080, 0, NULL) == -EINVAL);
	assert(mdns.y_stride == 11);
	assert(mdns.used_size == 22);
}

static void test_dma_aperture_validation(void)
{
	const uint32_t t31_limit = 0x08000000U;

	assert(tx_isp_dma_range_validate(0x02cf9000U, 3133440U,
					 t31_limit) == 0);
	assert(tx_isp_dma_range_validate(0x07d03000U, 3133440U,
					 t31_limit) == 0);
	assert(tx_isp_dma_range_validate(0x07d04000U, 3133440U,
					 t31_limit) == -ERANGE);
	assert(tx_isp_dma_range_validate(0x10023000U, 4096U,
					 t31_limit) == -ERANGE);
	assert(tx_isp_dma_range_validate(0, 4096U, t31_limit) == -EINVAL);
	assert(tx_isp_dma_range_validate(0x02cf9000U, 0, t31_limit) ==
		       -EINVAL);
	assert(tx_isp_dma_range_validate(0x02cf9000U, 4096U, 0) ==
		       -EINVAL);
}

int main(void)
{
	test_device_geometries();
	test_mdns_device_geometries();
	test_nv12_dma_binding();
	test_validation_and_overflow();
	test_dma_aperture_validation();
	puts("tx_isp_frame_layout tests passed");
	return 0;
}
