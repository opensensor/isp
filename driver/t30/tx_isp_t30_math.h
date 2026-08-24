#ifndef TX_ISP_T30_MATH_H
#define TX_ISP_T30_MATH_H

#include <linux/types.h>

u32 private_math_exp2(u32 value, u8 input_precision,
		      u8 output_precision);
u8 private_leading_one_position(u32 value);
int private_leading_one_position_64(u64 value);
u32 private_log2_int_to_fixed(u32 value, u8 output_precision,
			      u8 output_shift);
u32 private_log2_int_to_fixed_64(u64 value, u8 output_precision,
				 u8 output_shift);
u32 private_log2_fixed_to_fixed(u32 value, int input_precision,
				u8 output_precision);
s32 private_log2_fixed_to_fixed_64(u64 value, s32 input_precision,
				   u8 output_precision);

u16 calc_modulation_u16(u16 x, const u16 *pairs, int pair_count);
u32 calc_modulation_u32(u32 x, const u32 *pairs, int pair_count);
u16 calc_scaled_modulation_u16(u16 x, u16 target_min, u16 target_max,
			       const u16 *pairs, int pair_count);
u16 calc_equidistant_modulation_u16(u16 x, const u16 *table,
				    u16 table_len);
u32 calc_equidistant_modulation_u32(u32 x, const u32 *table,
				    u32 table_len);
u16 calc_inv_equidistant_modulation_u16(u16 x, const u16 *table,
					u16 table_len);
u32 calc_inv_equidistant_modulation_u32(u32 x, const u32 *table,
					u32 table_len);

u8 leading_one_position(u32 value);
u16 sqrt32(u32 value);
u8 sqrt16(u16 value);
u8 log16(u16 value);
u32 log2_int_to_fixed(u32 value, u8 output_precision, u8 output_shift);
u32 log2_fixed_to_fixed(u32 value, int input_precision,
				u8 output_precision);
u32 math_exp2(u32 value, u8 input_precision, u8 output_precision);
u32 math_log2(u32 value, u8 output_precision, u8 output_shift);
u32 multiplication_fixed_to_fixed(u32 first, u32 second,
				   int input_fraction,
				   int output_fraction);
s32 solving_lin_equation_a(s32 y1, s32 y2, s32 x1, s32 x2,
			   s16 fraction_size);
s32 solving_lin_equation_b(s32 y1, s32 slope, s32 x1,
			   s16 fraction_size);
u32 div_fixed(u32 numerator, u32 denominator, s16 fraction_size);
s32 solving_nth_root_045(s32 value, s16 fraction_size);
u16 line_offset(u16 line_length, u8 bytes_per_pixel);

#endif /* TX_ISP_T30_MATH_H */
