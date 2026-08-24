#ifndef TX_ISP_FRAME_FORMAT_H
#define TX_ISP_FRAME_FORMAT_H

#include "tx_isp_frame_abi.h"

/*
 * Private frame-channel format shared by the 32-bit T23/T30/T31 userspace ABI.
 * T41 retains the same base and appends one low-byte flip-enable word.
 *
 * Enable fields are one byte followed by three explicit padding bytes.  This
 * matches the vendor kernel's bool layout without depending on a host bool.
 */
struct tx_isp_pixel_format_wire {
	u32 width;
	u32 height;
	u32 pixelformat;
	u32 field;
	u32 bytesperline;
	u32 sizeimage;
	u32 colorspace;
	u32 priv;
	u32 flags;
	u32 ycbcr_enc;
	u32 quantization;
	u32 xfer_func;
};

struct tx_isp_frame_format_wire {
	u32 type;
	struct tx_isp_pixel_format_wire pix;
	unsigned char crop_enable;
	unsigned char crop_padding[3];
	u32 crop_top;
	u32 crop_left;
	u32 crop_width;
	u32 crop_height;
	unsigned char scaler_enable;
	unsigned char scaler_padding[3];
	u32 scaler_out_width;
	u32 scaler_out_height;
	u32 rate_bits;
	u32 rate_mask;
	unsigned char fcrop_enable;
	unsigned char fcrop_padding[3];
	u32 fcrop_top;
	u32 fcrop_left;
	u32 fcrop_width;
	u32 fcrop_height;
};

struct tx_isp_t41_frame_format_wire {
	struct tx_isp_frame_format_wire base;
	unsigned char flip_enable;
	unsigned char flip_padding[3];
};

enum tx_isp_frame_format_word {
	TX_ISP_FRAME_FORMAT_WORD_TYPE = 0,
	TX_ISP_FRAME_FORMAT_WORD_WIDTH,
	TX_ISP_FRAME_FORMAT_WORD_HEIGHT,
	TX_ISP_FRAME_FORMAT_WORD_PIXELFORMAT,
	TX_ISP_FRAME_FORMAT_WORD_FIELD,
	TX_ISP_FRAME_FORMAT_WORD_BYTESPERLINE,
	TX_ISP_FRAME_FORMAT_WORD_SIZEIMAGE,
	TX_ISP_FRAME_FORMAT_WORD_COLORSPACE,
	TX_ISP_FRAME_FORMAT_WORD_PRIV,
	TX_ISP_FRAME_FORMAT_WORD_FLAGS,
	TX_ISP_FRAME_FORMAT_WORD_YCBCR_ENC,
	TX_ISP_FRAME_FORMAT_WORD_QUANTIZATION,
	TX_ISP_FRAME_FORMAT_WORD_XFER_FUNC,
	TX_ISP_FRAME_FORMAT_WORD_CROP_ENABLE,
	TX_ISP_FRAME_FORMAT_WORD_CROP_TOP,
	TX_ISP_FRAME_FORMAT_WORD_CROP_LEFT,
	TX_ISP_FRAME_FORMAT_WORD_CROP_WIDTH,
	TX_ISP_FRAME_FORMAT_WORD_CROP_HEIGHT,
	TX_ISP_FRAME_FORMAT_WORD_SCALER_ENABLE,
	TX_ISP_FRAME_FORMAT_WORD_SCALER_WIDTH,
	TX_ISP_FRAME_FORMAT_WORD_SCALER_HEIGHT,
	TX_ISP_FRAME_FORMAT_WORD_RATE_BITS,
	TX_ISP_FRAME_FORMAT_WORD_RATE_MASK,
	TX_ISP_FRAME_FORMAT_WORD_FCROP_ENABLE,
	TX_ISP_FRAME_FORMAT_WORD_FCROP_TOP,
	TX_ISP_FRAME_FORMAT_WORD_FCROP_LEFT,
	TX_ISP_FRAME_FORMAT_WORD_FCROP_WIDTH,
	TX_ISP_FRAME_FORMAT_WORD_FCROP_HEIGHT,
	TX_ISP_FRAME_FORMAT_WORD_COUNT,
	TX_ISP_T41_FRAME_FORMAT_WORD_FLIP_ENABLE =
		TX_ISP_FRAME_FORMAT_WORD_COUNT,
	TX_ISP_T41_FRAME_FORMAT_WORD_COUNT,
};

#define TX_ISP_PIXEL_FORMAT_BYTES		0x30U
#define TX_ISP_FRAME_FORMAT_BYTES		0x70U
#define TX_ISP_T41_FRAME_FORMAT_BYTES		0x74U

#endif /* TX_ISP_FRAME_FORMAT_H */
