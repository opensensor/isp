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

int fix_point_mult2_32(int point_pos, int first, int second)
{
	return (int)tx_isp_fixmul_wrapped_u32_unchecked((u32)point_pos,
							(u32)first,
							(u32)second);
}

int fix_point_div_32(int point_pos, int numerator, int denominator)
{
	return (int)tx_isp_fixdiv_oem_u32((u32)point_pos, (u32)numerator,
					  (u32)denominator);
}

u32 tisp_math_exp2(u32 value, u32 input_precision, u32 output_precision)
{
	return tx_isp_exp2_u32(value, (u8)input_precision,
			       (u8)output_precision);
}

s32 tisp_simple_intp(s32 index, s32 fraction, void *table_address)
{
	const s32 *table = table_address;

	if (!table)
		return 0;

	return tx_isp_lerp_s32((u32)index, (u32)fraction, table,
			       TX_ISP_T23_INTERPOLATION_LAST_INDEX);
}

s32 tisp_log2_int_to_fixed(u32 value, char precision, char output_shift)
{
	return (s32)tx_isp_log2_int_u32(value, (u8)precision,
					(u8)output_shift);
}

s32 tisp_log2_fixed_to_fixed(u32 value, u32 input_precision,
			     u32 output_precision)
{
	return (s32)tx_isp_log2_fixed_u32(value, input_precision,
					  (u8)output_precision);
}

s32 tisp_log2_int_to_fixed_64(u32 value_low, u32 value_high,
			      char precision, char output_shift)
{
	u64 value = ((u64)value_high << 32) | value_low;

	return (s32)tx_isp_log2_int_u64(value, (u8)precision,
					(u8)output_shift);
}

s32 tisp_log2_fixed_to_fixed_64(u32 value_low, u32 value_high,
				s32 input_precision, char output_precision)
{
	u64 value = ((u64)value_high << 32) | value_low;

	return (s32)tx_isp_log2_fixed_u64(value, (u32)input_precision,
					  (u8)output_precision);
}
