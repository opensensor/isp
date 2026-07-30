/*
 * T41 compatibility entry points for the shared TX-ISP math library.
 *
 * Keep the recovered function names and MIPS32 ABI here while the algorithms
 * live in the kernel-independent common header. Table depth remains an
 * explicit T41 policy rather than hidden global state in the shared code.
 *
 * Only layout-neutral entry points belong here. The recovered T41 core is
 * sensitive to both its text and BSS tail layout, so additional helpers must
 * first move behind a separately loaded common module or a size-neutral
 * trampoline before their generation-local bodies can be removed.
 */

#include <linux/types.h>

#include "../include/tx_isp/tx_isp_math.h"

#define TX_ISP_T41_INTERPOLATION_LAST_INDEX	10U

extern s32 isp_printf(u32 level, const char *fmt, ...);

u32 fix_point_add_32(u32 point_pos, u32 left, u32 right)
{
	(void)point_pos;
	return tx_isp_fixadd_u32(left, right);
}

u32 fix_point_sub_32(u32 point_pos, u32 left, u32 right)
{
	(void)point_pos;
	if (left < right)
		isp_printf(2, "fix_point_sub_32: unsigned underflow\n");

	return tx_isp_fixsub_u32(left, right);
}

u64 fix_point_add(u32 point_pos, u64 left, u64 right)
{
	(void)point_pos;
	return tx_isp_fixadd_u64(left, right);
}

u64 fix_point_sub(u32 point_pos, u64 left, u64 right)
{
	(void)point_pos;
	if (left < right)
		isp_printf(2, "fix_point_sub: unsigned underflow\n");

	return tx_isp_fixsub_u64(left, right);
}

u64 fix_point_add_64(u32 point_pos, u64 left, u64 right)
{
	(void)point_pos;
	return tx_isp_fixadd_u64(left, right);
}

u64 fix_point_sub_64(u32 point_pos, u64 left, u64 right)
{
	(void)point_pos;
	if (left < right)
		isp_printf(2, "fix_point_sub_64: unsigned underflow\n");

	return tx_isp_fixsub_u64(left, right);
}

s64 tisp_round_int64(s64 value, s32 precision)
{
	return tx_isp_round_s64(value, (u32)precision);
}

u64 __attribute__((__noinline__))
fix_point_mult2(u32 point_pos, u64 first, u64 second)
{
	return tx_isp_fixmul_u64(point_pos, first, second);
}

u64 fix_point_mult3(u32 point_pos, u64 first, u64 second, u64 third)
{
	u64 pair = fix_point_mult2(point_pos, first, second);

	return fix_point_mult2(point_pos, pair, third);
}

u64 fix_point_mult2_64(u32 point_pos, u64 first, u64 second)
{
	return fix_point_mult2(point_pos, first, second);
}

u64 fix_point_mult3_64(u32 point_pos, u64 first, u64 second, u64 third)
{
	return fix_point_mult3(point_pos, first, second, third);
}

s32 fix_point_mult2_32(s32 point_pos, s32 first, s32 second)
{
	if (point_pos < 0 || point_pos > 31)
		return 0;

	return (s32)tx_isp_fixmul_u32((u32)point_pos, (u32)first,
				      (u32)second);
}

u32 fix_point_mult3_32(u32 point_pos, u32 first, u32 second, u32 third)
{
	return tx_isp_fixmul3_u32(point_pos, first, second, third);
}

s32 fix_point_div_32(u32 point_pos, u32 numerator, u32 denominator)
{
	return (s32)tx_isp_fixdiv_oem_u32(point_pos, numerator,
					  denominator);
}

s64 tisp_simple_intp(u32 index, u32 fraction, unsigned long table_address)
{
	const u32 *table = (const u32 *)table_address;

	if (!table)
		return 0;

	return tx_isp_lerp_u32(index, fraction, table,
			       TX_ISP_T41_INTERPOLATION_LAST_INDEX);
}

u32 tisp_simple_intp_int8(s32 index, s32 fraction, void *table_address)
{
	const u8 *table = table_address;

	if (!table)
		return 0;

	return tx_isp_lerp_u8((u32)index, (u32)fraction, table,
			      TX_ISP_T41_INTERPOLATION_LAST_INDEX);
}

s64 tisp_simple_intp_int16(u32 index, u32 fraction,
			   unsigned long table_address)
{
	const u16 *table = (const u16 *)table_address;

	if (!table)
		return 0;

	return tx_isp_lerp_u16(index, fraction, table,
			       TX_ISP_T41_INTERPOLATION_LAST_INDEX);
}
