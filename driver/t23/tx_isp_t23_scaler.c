/*
 * T23 polyphase scaler adapter.
 *
 * The generator and validation are shared; this file owns the exact T23
 * sinc table and four-tap rounding order recovered from the stock module.
 */
#ifdef __KERNEL__
#include <linux/types.h>
#endif

#include "tx_isp_t23_scaler.h"
#include "../include/tx_isp/tx_isp_scaler.h"

#ifdef __KERNEL__
#include "../common/tx_isp_scaler.c"
#endif

static const s16 tx_isp_t23_scaler_sinc_lut[257] = {
	1535, 1534, 1532, 1528, 1523, 1516, 1507, 1497,
	1486, 1473, 1459, 1444, 1427, 1409, 1389, 1369,
	1347, 1324, 1300, 1274, 1248, 1221, 1194, 1165,
	1135, 1105, 1074, 1043, 1011, 979, 946, 913,
	880, 847, 813, 779, 745, 711, 678, 645,
	612, 579, 546, 514, 483, 452, 421, 391,
	361, 332, 304, 277, 251, 225, 200, 175,
	152, 130, 109, 89, 69, 50, 32, 16,
	0, -15, -29, -41, -53, -64, -75, -84,
	-92, -99, -106, -112, -117, -121, -125, -128,
	-130, -131, -132, -132, -132, -131, -130, -128,
	-126, -124, -121, -118, -114, -110, -106, -102,
	-98, -94, -89, -84, -79, -75, -70, -65,
	-60, -55, -51, -47, -43, -39, -35, -31,
	-27, -24, -21, -18, -15, -12, -10, -8,
	-7, -5, -4, -3, -2, -1, 0, 0,
	0, 0, 0, 0, -1, -2, -3, -4,
	-5, -7, -8, -9, -10, -12, -14, -16,
	-17, -19, -20, -22, -23, -25, -26, -27,
	-28, -30, -31, -32, -33, -34, -34, -35,
	-35, -36, -36, -36, -36, -36, -36, -36,
	-35, -34, -33, -33, -32, -31, -29, -28,
	-27, -26, -24, -23, -21, -20, -18, -16,
	-14, -13, -11, -9, -7, -5, -3, -1,
	0, 2, 3, 5, 6, 8, 9, 11,
	12, 13, 14, 15, 16, 17, 18, 19,
	19, 20, 20, 21, 21, 21, 21, 21,
	21, 21, 21, 21, 20, 20, 19, 19,
	18, 18, 17, 16, 15, 15, 14, 13,
	12, 12, 11, 10, 9, 8, 7, 6,
	6, 6, 5, 4, 3, 2, 2, 2,
	2, 2, 1, 0, 0, 0, 0, 0,
	0,
};

static const u8 tx_isp_t23_scaler_add_order[4] = {
	1, 2, 0, 3,
};

static const u8 tx_isp_t23_scaler_subtract_order[4] = {
	3, 0, 2, 1,
};

int tx_isp_t23_scaler_curve_generate(u32 ratio_q14, s16 *coefficients,
				     u32 coefficient_capacity)
{
	const struct tx_isp_scaler_kernel kernel = {
		.sinc_lut = tx_isp_t23_scaler_sinc_lut,
		.sinc_lut_count = sizeof(tx_isp_t23_scaler_sinc_lut) /
				  sizeof(tx_isp_t23_scaler_sinc_lut[0]),
		.add_order = tx_isp_t23_scaler_add_order,
		.subtract_order = tx_isp_t23_scaler_subtract_order,
		.phase_count = 16,
		.phase_stride = 8,
		.tap_count = 4,
	};

	return tx_isp_scaler_curve_generate(&kernel, ratio_q14, coefficients,
					    coefficient_capacity);
}

const s16 *tx_isp_t23_scaler_sinc_lut_get(u32 *entry_count)
{
	if (entry_count)
		*entry_count = sizeof(tx_isp_t23_scaler_sinc_lut) /
			       sizeof(tx_isp_t23_scaler_sinc_lut[0]);
	return tx_isp_t23_scaler_sinc_lut;
}
