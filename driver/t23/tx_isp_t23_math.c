/*
 * T23 compatibility entry points for the shared TX-ISP math library.
 *
 * T23 uses signed table values and an eight-step endpoint. The common helper
 * owns the arithmetic and OEM rounding behavior; this file owns the recovered
 * function name and generation policy.
 */

#include <linux/types.h>

#include "../include/tx_isp/tx_isp_math.h"

#define TX_ISP_T23_INTERPOLATION_LAST_INDEX	8U

s32 tisp_simple_intp(s32 index, s32 fraction, void *table_address)
{
	const s32 *table = table_address;

	if (!table)
		return 0;

	return tx_isp_lerp_s32((u32)index, (u32)fraction, table,
			       TX_ISP_T23_INTERPOLATION_LAST_INDEX);
}
