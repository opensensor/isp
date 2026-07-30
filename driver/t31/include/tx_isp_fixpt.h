#ifndef TX_ISP_FIXPT_H
#define TX_ISP_FIXPT_H

#include <linux/types.h>
#include <linux/math64.h>
#include "../../include/tx_isp/tx_isp_math.h"

/* 32-bit Q-format fixed-point helpers used by AE1 exposure code.
 * q: number of fractional bits (0..31)
 * All operations saturate naturally in 32-bit; callers must choose q to avoid overflow.
 */

static inline u32 fix_point_mult2_32(u32 q, u32 a, u32 b)
{
    return tx_isp_fixmul_u32(q, a, b);
}

static inline u32 fix_point_mult3_32(u32 q, u32 a, u32 b, u32 c)
{
    return tx_isp_fixmul3_u32(q, a, b, c);
}

static inline u32 fix_point_div_32(u32 q, u32 num, u32 den)
{
    return tx_isp_fixdiv_u32(q, num, den);
}

/* 64-bit Q-format fixed-point helpers.
 * OEM uses MIPS (lo,hi) register pairs; we use native uint64_t.
 * q for 64-bit ops is typically 2*_AePointPos (e.g., 20 for Q10 base).
 */

static inline u64 fix_point_mult2_64_native(u32 q, u64 a, u64 b)
{
	return tx_isp_fixmul_u64(q, a, b);
}

static inline u64 fix_point_mult3_64_native(u32 q, u64 a, u64 b, u64 c)
{
	u64 ab = fix_point_mult2_64_native(q, a, b);

	return fix_point_mult2_64_native(q, ab, c);
}

static inline u64 fix_point_div_64_native(u32 q, u64 num, u64 den)
{
	/* OEM at 0x10de4: (num << q) / den */
	if (den == 0)
		return 0;
	num <<= (q & 63);
	return div64_u64(num, den);
}

#endif /* TX_ISP_FIXPT_H */
