/* Exact T20/T30 OEM matches implemented by the shared fixed-point core. */

#include <linux/types.h>

#include "../include/tx_isp/tx_isp_math.h"

u32 log2_fixed_to_fixed(u32 value, int input_precision, u8 output_precision)
{
	return (u32)tx_isp_log2_fixed_u32(value, (u32)input_precision,
					 output_precision);
}

u32 multiplication_fixed_to_fixed(u32 first, u32 second,
				   int input_fraction, int output_fraction)
{
	(void)input_fraction;
	return tx_isp_multiply_fixed_u32(first, second,
					(unsigned int)output_fraction);
}

s32 solving_lin_equation_b(s32 y1, s32 slope, s32 x1,
			   s16 fraction_size)
{
	return tx_isp_solve_linear_b_s32(y1, slope, x1, fraction_size);
}

s32 solving_nth_root_045(s32 value, s16 fraction_size)
{
	return tx_isp_nth_root_045_s32(value, fraction_size);
}

u16 line_offset(u16 line_length, u8 bytes_per_pixel)
{
	return tx_isp_line_offset_u16(line_length, bytes_per_pixel);
}
