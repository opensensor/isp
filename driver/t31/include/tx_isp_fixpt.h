#ifndef TX_ISP_FIXPT_H
#define TX_ISP_FIXPT_H

#include <linux/types.h>
#include <linux/math64.h>

/* 32-bit Q-format fixed-point helpers used by AE1 exposure code.
 * q: number of fractional bits (0..31)
 * All operations saturate naturally in 32-bit; callers must choose q to avoid overflow.
 */

static inline u32 fix_point_mult2_32(u32 q, u32 a, u32 b)
{
    /* Decomp-inspired split multiply:
     * (ai<<q + af) * (bi<<q + bf) / (1<<q)
     * = (ai*bi<<q) + (ai*bf) + (af*bi) + (af*bf>>q)
     */
    const u32 mask = (q >= 32) ? 0xffffffffu : ((1u << q) - 1u);
    const u32 ai = (q >= 32) ? 0 : (a >> q);
    const u32 bi = (q >= 32) ? 0 : (b >> q);
    const u32 af = a & mask;
    const u32 bf = b & mask;

    /* Use 64-bit to avoid overflow on partial products */
    u64 term0 = ((u64)ai * (u64)bi) << q;
    u64 term1 = (u64)ai * (u64)bf;
    u64 term2 = (u64)af * (u64)bi;
    u64 term3 = (q >= 32) ? 0 : (((u64)af * (u64)bf) >> q);

    return (u32)(term0 + term1 + term2 + term3);
}

static inline u32 fix_point_mult3_32(u32 q, u32 a, u32 b, u32 c)
{
    /* Multiply a*b, then by c, keeping the same Q scaling */
    u32 ab = fix_point_mult2_32(q, a, b);
    return fix_point_mult2_32(q, ab, c);
}

static inline u32 fix_point_div_32(u32 q, u32 num, u32 den)
{
    if (den == 0)
        return 0; /* caller should guard; safe default */

    /* ((num << q) / den) using do_div to avoid libgcc 64-bit division */
    u64 scaled = ((u64)num) << (q & 31);
    do_div(scaled, den); /* scaled = scaled / den; remainder discarded */
    return (u32)scaled;
}

/* 64-bit Q-format fixed-point helpers.
 * OEM uses MIPS (lo,hi) register pairs; we use native uint64_t.
 * q for 64-bit ops is typically 2*_AePointPos (e.g., 20 for Q10 base).
 */

static inline u64 fix_point_mult2_64_native(u32 q, u64 a, u64 b)
{
	/* Same split-multiply algorithm as mult2_32 but for 64-bit operands.
	 * OEM at 0x10bd0. For the values used in ae0_tune2 (EV ratios near
	 * 1.0 in Q20), the partial products fit in 64 bits. */
	u64 mask, ai, bi, af, bf;

	if (q >= 64)
		return 0;

	mask = (1ULL << q) - 1;
	ai = a >> q;
	bi = b >> q;
	af = a & mask;
	bf = b & mask;

	/* (ai*bi << q) + ai*bf + af*bi + (af*bf >> q)
	 * For AE values (bounded to ~30 bits), no 128-bit overflow. */
	return (ai * bi << q) + ai * bf + af * bi + ((af * bf) >> q);
}

static inline u64 fix_point_mult3_64_native(u32 q, u64 a, u64 b, u64 c)
{
	/* OEM at 0x10d70: mult3 = mult2(mult2(a,b), c) */
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

