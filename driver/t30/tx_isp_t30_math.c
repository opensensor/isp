/*
 * T30 ABI adapters for the shared fixed-point and modulation libraries.
 *
 * Function names and parameter widths follow the Ingenic 3.10.14 SDK.  The
 * arithmetic is generation-neutral and lives in the host-tested common
 * headers; this file only preserves the T30/Apical-facing contract.
 */

#include <linux/module.h>
#include <linux/types.h>

#include "../include/tx_isp/tx_isp_math.h"
#include "../include/tx_isp/tx_isp_modulation.h"
#include "tx_isp_t30_math.h"

u32 private_math_exp2(u32 value, u8 input_precision, u8 output_precision)
{
	return tx_isp_exp2_u32(value, input_precision, output_precision);
}

u8 private_leading_one_position(u32 value)
{
	return tx_isp_leading_one_u32(value);
}

int private_leading_one_position_64(u64 value)
{
	return tx_isp_leading_one_u64(value);
}

u32 private_log2_int_to_fixed(u32 value, u8 output_precision,
			      u8 output_shift)
{
	return (u32)tx_isp_log2_int_u32(value, output_precision, output_shift);
}

u32 private_log2_int_to_fixed_64(u64 value, u8 output_precision,
				 u8 output_shift)
{
	return (u32)tx_isp_log2_int_u64(value, output_precision, output_shift);
}

u32 private_log2_fixed_to_fixed(u32 value, int input_precision,
				u8 output_precision)
{
	return (u32)tx_isp_log2_fixed_u32(value, (u32)input_precision,
					 output_precision);
}

s32 private_log2_fixed_to_fixed_64(u64 value, s32 input_precision,
				   u8 output_precision)
{
	return (s32)tx_isp_log2_fixed_u64(value, (u32)input_precision,
					 output_precision);
}

u16 calc_modulation_u16(u16 x, const u16 *pairs, int pair_count)
{
	return (u16)tx_isp_modulate_pairs_u16(x, pairs, pair_count);
}

u32 calc_modulation_u32(u32 x, const u32 *pairs, int pair_count)
{
	return tx_isp_modulate_pairs_u32(x, pairs, pair_count);
}

u16 calc_scaled_modulation_u16(u16 x, u16 target_min, u16 target_max,
			       const u16 *pairs, int pair_count)
{
	return (u16)tx_isp_modulate_pairs_scaled_u16(
		x, target_min, target_max, pairs, pair_count);
}

u16 calc_equidistant_modulation_u16(u16 x, const u16 *table,
				    u16 table_len)
{
	return (u16)tx_isp_modulate_equidistant_u16(x, table, table_len);
}

u32 calc_equidistant_modulation_u32(u32 x, const u32 *table,
				    u32 table_len)
{
	return tx_isp_modulate_equidistant_u32(x, table, table_len);
}

u16 calc_inv_equidistant_modulation_u16(u16 x, const u16 *table,
					u16 table_len)
{
	return (u16)tx_isp_modulate_inverse_equidistant_u16(
		x, table, table_len);
}

u32 calc_inv_equidistant_modulation_u32(u32 x, const u32 *table,
					u32 table_len)
{
	return tx_isp_modulate_inverse_equidistant_u32(x, table, table_len);
}

u8 leading_one_position(u32 value)
{
	return tx_isp_leading_one_u32(value);
}

u16 sqrt32(u32 value)
{
	return tx_isp_isqrt_u32(value);
}

u8 sqrt16(u16 value)
{
	return tx_isp_isqrt_u16(value);
}

u8 log16(u16 value)
{
	return tx_isp_log_u16_q4(value);
}

u32 log2_int_to_fixed(u32 value, u8 output_precision, u8 output_shift)
{
	return (u32)tx_isp_log2_int_u32(value, output_precision, output_shift);
}

u32 log2_fixed_to_fixed(u32 value, int input_precision,
				u8 output_precision)
{
	return (u32)tx_isp_log2_fixed_u32(value, (u32)input_precision,
					 output_precision);
}

u32 math_exp2(u32 value, u8 input_precision, u8 output_precision)
{
	return tx_isp_exp2_u32(value, input_precision, output_precision);
}

u32 math_log2(u32 value, u8 output_precision, u8 output_shift)
{
	return (u32)tx_isp_log2_int_u32(value, output_precision, output_shift);
}

u32 multiplication_fixed_to_fixed(u32 first, u32 second,
				   int input_fraction,
				   int output_fraction)
{
	(void)input_fraction;
	return tx_isp_multiply_fixed_u32(first, second,
					(unsigned int)output_fraction);
}

s32 solving_lin_equation_a(s32 y1, s32 y2, s32 x1, s32 x2,
			   s16 fraction_size)
{
	return tx_isp_solve_linear_a_s32(y1, y2, x1, x2, fraction_size);
}

s32 solving_lin_equation_b(s32 y1, s32 slope, s32 x1,
			   s16 fraction_size)
{
	return tx_isp_solve_linear_b_s32(y1, slope, x1, fraction_size);
}

u32 div_fixed(u32 numerator, u32 denominator, s16 fraction_size)
{
	return tx_isp_div_fixed_u32(numerator, denominator, fraction_size);
}

s32 solving_nth_root_045(s32 value, s16 fraction_size)
{
	return tx_isp_nth_root_045_s32(value, fraction_size);
}

u16 line_offset(u16 line_length, u8 bytes_per_pixel)
{
	return tx_isp_line_offset_u16(line_length, bytes_per_pixel);
}

EXPORT_SYMBOL(private_math_exp2);
EXPORT_SYMBOL(private_log2_int_to_fixed);
EXPORT_SYMBOL(private_log2_fixed_to_fixed);
EXPORT_SYMBOL(math_exp2);
EXPORT_SYMBOL(log2_fixed_to_fixed);
