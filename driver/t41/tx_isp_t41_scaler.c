/*
 * T41 polyphase scaler adapter.
 *
 * The generator and validation are shared; this file owns the exact
 * H20250310a sinc table and deterministic rounding orders recovered from the
 * stock T41 module.
 */
#ifdef __KERNEL__
#include <linux/errno.h>
#include <linux/types.h>
#else
#include <errno.h>
#define ARRAY_SIZE(array) (sizeof(array) / sizeof((array)[0]))
#endif

#include "tx_isp_t41_scaler.h"
#include "../include/tx_isp/tx_isp_scaler.h"

#ifdef __KERNEL__
#include "../common/tx_isp_scaler.c"
#endif

static const s16 tx_isp_t41_scaler_sinc_lut[257] = {
	1535, 1532, 1523, 1507, 1486, 1459, 1427, 1389,
	1347, 1300, 1248, 1194, 1135, 1074, 1011, 946,
	880, 813, 745, 678, 612, 546, 483, 421,
	361, 304, 251, 200, 152, 109, 69, 32,
	0, -29, -53, -75, -92, -106, -117, -125,
	-130, -132, -132, -130, -126, -121, -114, -106,
	-98, -89, -79, -70, -60, -51, -43, -35,
	-27, -21, -15, -10, -7, -4, -2, 0,
	0, 0, -1, -3, -5, -8, -10, -14,
	-17, -20, -23, -26, -28, -31, -33, -34,
	-35, -36, -36, -36, -35, -33, -32, -29,
	-27, -24, -21, -18, -14, -11, -7, -3,
	0, 3, 6, 9, 12, 14, 16, 18,
	19, 20, 21, 21, 21, 21, 20, 19,
	18, 17, 15, 14, 12, 11, 9, 7,
	6, 5, 3, 2, 2, 1, 0, 0,
	0, 0, 0, 1, 1, 2, 3, 4,
	5, 6, 7, 7, 8, 9, 10, 10,
	11, 11, 11, 11, 11, 11, 10, 10,
	9, 8, 7, 6, 5, 4, 2, 1,
	0, -1, -2, -3, -4, -5, -6, -7,
	-7, -8, -8, -8, -8, -8, -8, -8,
	-7, -7, -6, -6, -5, -4, -4, -3,
	-3, -2, -1, -1, -1, 0, 0, 0,
	0, 0, 0, 0, -1, -1, -1, -2,
	-2, -3, -3, -4, -4, -4, -5, -5,
	-5, -5, -5, -5, -5, -5, -5, -5,
	-4, -4, -4, -3, -2, -2, -1, -1,
	0, 1, 1, 2, 2, 3, 3, 4,
	4, 4, 4, 4, 4, 4, 4, 4,
	4, 4, 3, 3, 3, 2, 2, 2,
	1, 1, 1, 1, 0, 0, 0, 0,
	0,
};

static const u8 tx_isp_t41_scaler_add_order[8] = {
	3, 4, 2, 5, 1, 6, 0, 7,
};

static const u8 tx_isp_t41_scaler_subtract_order[8] = {
	7, 0, 6, 1, 5, 2, 4, 3,
};

int tx_isp_t41_scaler_channel_curve_generate(u32 channel, u32 target,
					   u32 source, s16 *coefficients,
					   u32 coefficient_capacity)
{
	const struct tx_isp_scaler_kernel kernel = {
		.sinc_lut = tx_isp_t41_scaler_sinc_lut,
		.sinc_lut_count = sizeof(tx_isp_t41_scaler_sinc_lut) /
				  sizeof(tx_isp_t41_scaler_sinc_lut[0]),
		.add_order = tx_isp_t41_scaler_add_order,
		.subtract_order = tx_isp_t41_scaler_subtract_order,
		.phase_count = channel == 1 ? 256 : 32,
		.phase_stride = channel == 1 ? 64 : 8,
		.tap_count = 8,
	};
	u32 ratio_q14;

	/* Geometry enters the ISP in unsigned 16-bit fields. Bound the Q14 math. */
	if (channel > 2 || !target || !source || target > 65535 || source > 65535)
		return -EINVAL;
	ratio_q14 = ((target << 14) + (source >> 1)) / source;
	if (ratio_q14 >= 27307U)
		ratio_q14 = 27306U;
	return tx_isp_scaler_curve_generate(&kernel, ratio_q14, coefficients,
					    coefficient_capacity);
}

int tx_isp_t41_scaler_curve_generate(u32 target, u32 source,
				     s16 *coefficients,
				     u32 coefficient_capacity)
{
	return tx_isp_t41_scaler_channel_curve_generate(
		0, target, source, coefficients, coefficient_capacity);
}

static u32 tx_isp_t41_scaler_fixed_coefficient(const u8 *p, u32 tap)
{
	return ((u32)p[2 * tap] | ((u32)p[2 * tap + 1] << 8)) & 0x1fffU;
}

static int tx_isp_t41_scaler_pair(tx_isp_t41_scaler_write_fn write,
				void *context, u32 address, u32 value)
{
	int ret = write(context, 0xf8004U, value);

	return ret ? ret : write(context, 0xf8004U, address);
}

int tx_isp_t41_scaler_curve_write(u32 channel, const u8 *params,
				u32 params_bytes, const s16 *vertical,
				const s16 *horizontal, u32 curve_capacity,
				tx_isp_t41_scaler_write_fn write, void *context)
{
	u32 count = channel == 1 ? 257U : 33U;
	u32 axis;
	int ret;

	if (channel > 2 || !params || params_bytes < TX_ISP_T41_SCALER_PARAMS_BYTES ||
	    !vertical || !horizontal || curve_capacity < count || !write)
		return -EINVAL;

	/* H20250310a tisp_msca_ch_curve_write: channel 1 uses two FIFO
	 * coefficient RAMs. Channels 0/2 use linear packed coefficient registers.
	 * Both transports store signed 13-bit taps, two per 32-bit value.
	 */
	for (axis = 0; axis < 2; ++axis) {
		const s16 *curve = axis ? horizontal : vertical;
		const u8 *fixed = params + 0x70 + channel * 0x20 + axis * 0x10;
		u32 fixed_phase = params[0xdf + 2 * channel + axis] == 1;
		u32 i;

		if (channel == 1) {
			u32 phase;

			ret = tx_isp_t41_scaler_pair(write, context,
						    0xf1100U + axis * 8, 1);
			if (ret)
				return ret;
			for (phase = 0; phase <= 32; ++phase) {
				for (i = 0; i < 8; i += 2) {
					u32 value = 0;
					u32 half;

					for (half = 0; half < 2; ++half) {
						u32 tap = i + half;
						u32 index = tap < 4 ?
							256 - tap * 64 - phase :
							(tap - 4) * 64 + phase;
						u32 coefficient = fixed_phase && !phase ?
							tx_isp_t41_scaler_fixed_coefficient(fixed, tap) :
							((u32)curve[index] & 0x1fffU);

						value |= coefficient << (16 * half);
					}
					ret = tx_isp_t41_scaler_pair(write, context,
								    0xf1104U + axis * 8, value);
					if (ret)
						return ret;
				}
			}
		} else {
			u32 base = 0xf0740U + channel * 0x80 + axis * 0x50;

			for (i = 0; i < count; i += 2) {
				u32 value = 0;
				u32 half;

				for (half = 0; half < 2 && i + half < count; ++half) {
					u32 tap = i + half;
					u32 coefficient = fixed_phase && tap < 8 ?
						tx_isp_t41_scaler_fixed_coefficient(fixed, tap) :
						((u32)curve[tap] & 0x1fffU);

					value |= coefficient << (16 * half);
				}
				ret = tx_isp_t41_scaler_pair(write, context,
							    base + (i / 2) * 4, value);
				if (ret)
					return ret;
			}
		}
	}
	return channel == 1 ? 266 : 34;
}
