#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#include "tx_isp/tx_isp_frame_format.h"

#define ABI_ASSERT(name, expression) \
	typedef char abi_assert_##name[(expression) ? 1 : -1]

#define WORD_OFFSET(type, member, word) \
	ABI_ASSERT(member##_offset, offsetof(type, member) == \
		   (word) * sizeof(uint32_t))

ABI_ASSERT(pixel_format_size,
	sizeof(struct tx_isp_pixel_format_wire) == TX_ISP_PIXEL_FORMAT_BYTES);
ABI_ASSERT(frame_format_size,
	sizeof(struct tx_isp_frame_format_wire) == TX_ISP_FRAME_FORMAT_BYTES);
ABI_ASSERT(t41_frame_format_size,
	sizeof(struct tx_isp_t41_frame_format_wire) ==
	TX_ISP_T41_FRAME_FORMAT_BYTES);
ABI_ASSERT(frame_word_count,
	TX_ISP_FRAME_FORMAT_WORD_COUNT * sizeof(uint32_t) ==
	TX_ISP_FRAME_FORMAT_BYTES);
ABI_ASSERT(t41_frame_word_count,
	TX_ISP_T41_FRAME_FORMAT_WORD_COUNT * sizeof(uint32_t) ==
	TX_ISP_T41_FRAME_FORMAT_BYTES);

WORD_OFFSET(struct tx_isp_frame_format_wire, type,
	    TX_ISP_FRAME_FORMAT_WORD_TYPE);
ABI_ASSERT(pixel_format_offset,
	offsetof(struct tx_isp_frame_format_wire, pix) == sizeof(uint32_t));
ABI_ASSERT(pixel_xfer_offset,
	offsetof(struct tx_isp_frame_format_wire, pix.xfer_func) ==
	TX_ISP_FRAME_FORMAT_WORD_XFER_FUNC * sizeof(uint32_t));
WORD_OFFSET(struct tx_isp_frame_format_wire, crop_enable,
	    TX_ISP_FRAME_FORMAT_WORD_CROP_ENABLE);
WORD_OFFSET(struct tx_isp_frame_format_wire, crop_top,
	    TX_ISP_FRAME_FORMAT_WORD_CROP_TOP);
WORD_OFFSET(struct tx_isp_frame_format_wire, scaler_enable,
	    TX_ISP_FRAME_FORMAT_WORD_SCALER_ENABLE);
WORD_OFFSET(struct tx_isp_frame_format_wire, scaler_out_width,
	    TX_ISP_FRAME_FORMAT_WORD_SCALER_WIDTH);
WORD_OFFSET(struct tx_isp_frame_format_wire, rate_bits,
	    TX_ISP_FRAME_FORMAT_WORD_RATE_BITS);
WORD_OFFSET(struct tx_isp_frame_format_wire, fcrop_enable,
	    TX_ISP_FRAME_FORMAT_WORD_FCROP_ENABLE);
WORD_OFFSET(struct tx_isp_frame_format_wire, fcrop_height,
	    TX_ISP_FRAME_FORMAT_WORD_FCROP_HEIGHT);
ABI_ASSERT(t41_flip_offset,
	offsetof(struct tx_isp_t41_frame_format_wire, flip_enable) ==
	TX_ISP_T41_FRAME_FORMAT_WORD_FLIP_ENABLE * sizeof(uint32_t));

int main(void)
{
	puts("tx_isp_frame_format tests passed");
	return 0;
}
