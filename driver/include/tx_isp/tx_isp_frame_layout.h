#ifndef TX_ISP_FRAME_LAYOUT_H
#define TX_ISP_FRAME_LAYOUT_H

#include "tx_isp_frame_abi.h"
#include "tx_isp_frame_channel.h"
#include "tx_isp_frame_format.h"

struct tx_isp_nv12_layout {
	u32 stride;
	u32 aggregate_line_size;
	u32 aligned_height;
	u32 y_size;
	u32 sizeimage;
};

struct tx_isp_nv12_buffer {
	struct tx_isp_nv12_layout layout;
	u32 y_dma;
	u32 uv_dma;
};

/*
 * The temporal denoise unit keeps an NV12 reference followed by up to four
 * compressed reference banks and, in full-memory mode, two chroma auxiliary
 * banks plus a tiny summary plane.  Offsets are relative to the allocation
 * base.  Memory-optimized layouts alias every inactive bank at offset zero.
 */
#define TX_ISP_MDNS_REFERENCE_BANKS 4
#define TX_ISP_MDNS_UV_BANKS 2

struct tx_isp_mdns_layout {
	u32 y_stride;
	u32 y_size;
	u32 nv12_size;
	u32 reference_stride;
	u32 reference_height;
	u32 reference_size;
	u32 reference_offset[TX_ISP_MDNS_REFERENCE_BANKS];
	u32 uv_stride;
	u32 uv_size;
	u32 uv_offset[TX_ISP_MDNS_UV_BANKS];
	u32 tiny_stride;
	u32 tiny_size;
	u32 tiny_offset;
	u32 used_size;
};

/*
 * Build the single-plane NV12 layout used by frame-channel userspace:
 *
 *   Y  = aligned width * aligned height
 *   UV = Y / 2
 *
 * aggregate_line_size is the vendor-private 12-bits-per-pixel line value
 * used by some TX-ISP generations.  It is not the per-plane DMA stride.
 *
 * Alignment values must be non-zero powers of two.  The helper rejects every
 * intermediate result that cannot be represented by the 32-bit vendor ABI.
 */
int tx_isp_nv12_layout_build(u32 width, u32 height, u32 width_align,
			     u32 height_align,
			     struct tx_isp_nv12_layout *layout);

/*
 * Bind checked NV12 geometry to one physical buffer.  The complete image must
 * fit both the supplied allocation and the vendor ABI's 32-bit DMA space.
 * Results are published only after every geometry, length, and address check
 * succeeds.
 */
int tx_isp_nv12_buffer_build(u32 width, u32 height, u32 width_align,
			     u32 height_align, u32 y_dma, u32 buffer_size,
			     struct tx_isp_nv12_buffer *buffer);

/*
 * Validate one complete DMA allocation against an exclusive physical-memory
 * ceiling.  This is deliberately separate from the 32-bit ABI check above:
 * a numerically valid address can still target an unmapped SoC bus window.
 */
int tx_isp_dma_range_validate(u32 dma, u32 size, u32 limit);

/*
 * Build the T23/T31 MDNS auxiliary allocation.  memopt == 0 returns the full
 * four-reference/two-UV/tiny-plane layout; any non-zero value returns the
 * single-reference layout used by the memory-optimized firmware mode.
 */
int tx_isp_mdns_layout_build(u32 width, u32 height, u32 memopt,
			     struct tx_isp_mdns_layout *layout);

#endif /* TX_ISP_FRAME_LAYOUT_H */
