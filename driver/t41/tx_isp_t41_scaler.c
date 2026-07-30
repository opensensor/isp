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

int tx_isp_t41_scaler_curve_generate(u32 target, u32 source,
				     s16 *coefficients,
				     u32 coefficient_capacity)
{
	const struct tx_isp_scaler_kernel kernel = {
		.sinc_lut = tx_isp_t41_scaler_sinc_lut,
		.sinc_lut_count = sizeof(tx_isp_t41_scaler_sinc_lut) /
				  sizeof(tx_isp_t41_scaler_sinc_lut[0]),
		.add_order = tx_isp_t41_scaler_add_order,
		.subtract_order = tx_isp_t41_scaler_subtract_order,
		.phase_count = 32,
		.phase_stride = 8,
		.tap_count = 8,
	};
	u32 ratio_q14;

	if (!target || !source)
		return -EINVAL;
	ratio_q14 = ((target << 14) + (source >> 1)) / source;
	if (ratio_q14 >= 27307U)
		ratio_q14 = 27306U;
	return tx_isp_scaler_curve_generate(&kernel, ratio_q14, coefficients,
					    coefficient_capacity);
}
