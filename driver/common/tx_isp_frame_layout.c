#ifdef __KERNEL__
#include <linux/errno.h>
#else
#include <errno.h>
#include <stdint.h>
typedef uint64_t u64;
#endif

#include "../include/tx_isp/tx_isp_frame_layout.h"

static int tx_isp_layout_power_of_two(u32 value)
{
	return value && !(value & (value - 1));
}

int tx_isp_nv12_layout_build(u32 width, u32 height, u32 width_align,
			     u32 height_align,
			     struct tx_isp_nv12_layout *layout)
{
	struct tx_isp_nv12_layout result;
	u64 aligned_width;
	u64 aligned_height;
	u64 aggregate_line_size;
	u64 y_size;
	u64 sizeimage;

	if (!layout || !width || !height ||
	    !tx_isp_layout_power_of_two(width_align) ||
	    !tx_isp_layout_power_of_two(height_align))
		return -EINVAL;

	aligned_width = ((u64)width + width_align - 1) &
			~((u64)width_align - 1);
	aligned_height = ((u64)height + height_align - 1) &
			 ~((u64)height_align - 1);
	aggregate_line_size = (aligned_width * 12) >> 3;
	y_size = aligned_width * aligned_height;
	sizeimage = y_size + (y_size >> 1);

	if (aligned_width > 0xffffffffULL ||
	    aggregate_line_size > 0xffffffffULL ||
	    aligned_height > 0xffffffffULL ||
	    y_size > 0xffffffffULL ||
	    sizeimage > 0xffffffffULL)
		return -EOVERFLOW;

	result.stride = (u32)aligned_width;
	result.aggregate_line_size = (u32)aggregate_line_size;
	result.aligned_height = (u32)aligned_height;
	result.y_size = (u32)y_size;
	result.sizeimage = (u32)sizeimage;
	*layout = result;
	return 0;
}

int tx_isp_nv12_buffer_build(u32 width, u32 height, u32 width_align,
			     u32 height_align, u32 y_dma, u32 buffer_size,
			     struct tx_isp_nv12_buffer *buffer)
{
	struct tx_isp_nv12_buffer result;
	u64 last_dma;
	int ret;

	if (!buffer)
		return -EINVAL;

	ret = tx_isp_nv12_layout_build(width, height, width_align,
				       height_align, &result.layout);
	if (ret)
		return ret;
	if (buffer_size < result.layout.sizeimage)
		return -ENOSPC;

	last_dma = (u64)y_dma + result.layout.sizeimage - 1;
	if (last_dma > 0xffffffffULL)
		return -EOVERFLOW;

	result.y_dma = y_dma;
	result.uv_dma = y_dma + result.layout.y_size;
	*buffer = result;
	return 0;
}

int tx_isp_mdns_layout_build(u32 width, u32 height, u32 memopt,
			     struct tx_isp_mdns_layout *layout)
{
	struct tx_isp_mdns_layout result = { 0 };
	u64 y_stride;
	u64 y_size;
	u64 nv12_size;
	u64 reference_stride;
	u64 reference_height;
	u64 reference_size;
	u64 next_offset;
	u64 uv_stride;
	u64 uv_size;
	u64 tiny_stride;
	u64 tiny_size;
	unsigned int index;

	if (!layout || !width || !height)
		return -EINVAL;

	y_stride = ((u64)width + 7) & ~7ULL;
	y_size = y_stride * height;
	nv12_size = y_size + (y_size >> 1);

	reference_stride = ((((u64)width + 31) >> 5) + 7) & ~7ULL;
	reference_height = (((u64)height + 15) >> 4) + 1;
	reference_size = reference_stride * reference_height;

	if (y_stride > 0xffffffffULL ||
	    y_size > 0xffffffffULL ||
	    nv12_size > 0xffffffffULL ||
	    reference_stride > 0xffffffffULL ||
	    reference_height > 0xffffffffULL ||
	    reference_size > 0xffffffffULL)
		return -EOVERFLOW;

	result.y_stride = (u32)y_stride;
	result.y_size = (u32)y_size;
	result.nv12_size = (u32)nv12_size;
	result.reference_stride = (u32)reference_stride;
	result.reference_height = (u32)reference_height;
	result.reference_size = (u32)reference_size;

	next_offset = nv12_size;
	for (index = 0; index < TX_ISP_MDNS_REFERENCE_BANKS; index++) {
		result.reference_offset[index] = (u32)next_offset;
		if (!memopt)
			next_offset += reference_size;
	}

	if (memopt) {
		next_offset = nv12_size + reference_size;
		if (next_offset > 0xffffffffULL)
			return -EOVERFLOW;
		result.used_size = (u32)next_offset;
		*layout = result;
		return 0;
	}

	if (next_offset > 0xffffffffULL)
		return -EOVERFLOW;

	uv_stride = (((u64)width >> 1) + 7) & ~7ULL;
	uv_size = uv_stride * height;
	result.uv_offset[0] = (u32)next_offset;
	next_offset += uv_size;
	if (next_offset > 0xffffffffULL)
		return -EOVERFLOW;
	result.uv_offset[1] = (u32)next_offset;
	next_offset += uv_size >> 1;
	if (next_offset > 0xffffffffULL)
		return -EOVERFLOW;

	tiny_stride = (((u64)width >> 5) + 7) & ~7ULL;
	tiny_size = (tiny_stride * height) >> 5;
	result.tiny_offset = (u32)next_offset;
	next_offset += tiny_size;

	if (uv_stride > 0xffffffffULL ||
	    uv_size > 0xffffffffULL ||
	    tiny_stride > 0xffffffffULL ||
	    tiny_size > 0xffffffffULL ||
	    next_offset > 0xffffffffULL)
		return -EOVERFLOW;

	result.uv_stride = (u32)uv_stride;
	result.uv_size = (u32)uv_size;
	result.tiny_stride = (u32)tiny_stride;
	result.tiny_size = (u32)tiny_size;
	result.used_size = (u32)next_offset;
	*layout = result;
	return 0;
}
