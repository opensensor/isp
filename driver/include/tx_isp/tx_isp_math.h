#ifndef TX_ISP_MATH_H
#define TX_ISP_MATH_H

/*
 * Small, SoC-independent arithmetic primitives shared by the ISP tuning
 * implementations. Keep this header independent of Linux headers: recovered
 * sources are also compiled by the reconstruction/audit tooling.
 *
 * The interpolation functions deliberately use unsigned int arithmetic for
 * the multiply. The target ABIs use 32-bit ints, and the OEM implementation
 * relies on that 32-bit wraparound before applying its Q16 shift. The signed
 * variant also preserves T23's arithmetic post-multiply shift.
 */

static inline unsigned int
tx_isp_lerp_pair_u32(unsigned int base_value, unsigned int next,
		     unsigned int fraction)
{
	unsigned int difference;
	unsigned int product;
	unsigned int step;
	int subtract;

	if (base_value == next || fraction == 0)
		return base_value;

	if (base_value >= next) {
		difference = base_value - next;
		subtract = 1;
	} else {
		difference = next - base_value;
		subtract = 0;
	}

	product = difference * fraction;
	step = (product >> 16) + ((product >> 15) & 1U);

	return subtract ? base_value - step : base_value + step;
}

static inline unsigned int
tx_isp_lerp_u32(unsigned int index, unsigned int fraction,
		const unsigned int *values, unsigned int last_index)
{
	if (index >= last_index)
		return values[last_index];

	return tx_isp_lerp_pair_u32(values[index], values[index + 1],
				    fraction);
}

static inline unsigned int
tx_isp_lerp_u16(unsigned int index, unsigned int fraction,
		const unsigned short *values, unsigned int last_index)
{
	if (index >= last_index)
		return values[last_index];

	return tx_isp_lerp_pair_u32(values[index], values[index + 1],
				    fraction);
}

static inline unsigned int
tx_isp_lerp_u8(unsigned int index, unsigned int fraction,
	       const unsigned char *values, unsigned int last_index)
{
	if (index >= last_index)
		return values[last_index];

	return tx_isp_lerp_pair_u32(values[index], values[index + 1],
				    fraction);
}

static inline int
tx_isp_lerp_s32(unsigned int index, unsigned int fraction,
		const int *values, unsigned int last_index)
{
	int base_value;
	int next;
	int difference;
	int step;
	unsigned int product;

	if (index >= last_index)
		return values[last_index];

	base_value = values[index];
	next = values[index + 1];
	if (base_value == next || fraction == 0)
		return base_value;

	difference = base_value >= next ? base_value - next : next - base_value;
	product = (unsigned int)difference * fraction;
	step = ((int)product >> 16) + (int)((product >> 15) & 1U);

	return base_value >= next ? base_value - step : base_value + step;
}

/*
 * Base-2 logarithm in the Ingenic tuning fixed-point format.
 *
 * The input is an unsigned integer. `precision` selects the number of
 * fractional log2 bits and `output_shift` retains additional mantissa bits
 * from the final normalization step. The 32- and 64-bit variants deliberately
 * use the same bit-31 decision as the recovered OEM implementations.
 */
static inline int
tx_isp_log2_int_u32(unsigned int value, unsigned int precision,
		    unsigned int output_shift)
{
	unsigned int normalized;
	unsigned int scan;
	unsigned int square;
	unsigned int bit_position = 0;
	unsigned int fraction = 0;
	unsigned int i;

	if (!value)
		return 0;

	if (value < 0x10000U) {
		scan = value;
	} else {
		scan = value >> 16;
		bit_position = 16;
	}

	if (scan >= 0x100U) {
		scan >>= 8;
		bit_position += 8;
	}
	if (scan >= 0x10U) {
		scan >>= 4;
		bit_position += 4;
	}
	if (scan >= 4U) {
		scan >>= 2;
		bit_position += 2;
	}
	if (scan != 1U)
		bit_position++;

	if (bit_position >= 16U)
		normalized = value >> ((bit_position - 15U) & 31U);
	else
		normalized = value << ((15U - bit_position) & 31U);

	for (i = 0; i < precision; i++) {
		square = normalized * normalized;
		fraction <<= 1;
		if (square & 0x80000000U) {
			fraction++;
			normalized = square >> 16;
		} else {
			normalized = square >> 15;
		}
	}

	return (int)((((bit_position << (precision & 31U)) + fraction)
		      << (output_shift & 31U)) |
		     ((normalized & 0x7fffU) >>
		      ((15U - output_shift) & 31U)));
}

static inline int
tx_isp_log2_fixed_u32(unsigned int value, unsigned int input_precision,
		      unsigned int output_precision)
{
	unsigned int integer_log;
	unsigned int input_scale;

	integer_log = (unsigned int)tx_isp_log2_int_u32(
		value, output_precision, 0);
	input_scale = input_precision << (output_precision & 31U);
	return (int)(integer_log - input_scale);
}

static inline int
tx_isp_log2_int_u64(unsigned long long value, unsigned int precision,
		    unsigned int output_shift)
{
	unsigned long long normalized_value;
	unsigned long long square;
	unsigned int normalized;
	unsigned int scan;
	unsigned int bit_position;
	unsigned int fraction = 0;
	unsigned int i;

	if (!value)
		return 0;

	if (value >> 32) {
		scan = (unsigned int)(value >> 32);
		bit_position = 32;
	} else {
		scan = (unsigned int)value;
		bit_position = 0;
	}

	if (scan >= 0x10000U) {
		scan >>= 16;
		bit_position += 16;
	}
	if (scan >= 0x100U) {
		scan >>= 8;
		bit_position += 8;
	}
	if (scan >= 0x10U) {
		scan >>= 4;
		bit_position += 4;
	}
	if (scan >= 4U) {
		scan >>= 2;
		bit_position += 2;
	}
	if (scan != 1U)
		bit_position++;

	normalized_value = value;
	if (bit_position >= 16U)
		normalized_value >>= bit_position - 15U;
	else
		normalized_value <<= 15U - bit_position;
	normalized = (unsigned int)normalized_value;

	for (i = 0; i < precision; i++) {
		square = (unsigned long long)normalized * normalized;
		fraction <<= 1;
		if (square & (1ULL << 31)) {
			fraction++;
			normalized = (unsigned int)(square >> 16);
		} else {
			normalized = (unsigned int)(square >> 15);
		}
	}

	return (int)((((bit_position << (precision & 31U)) + fraction)
		      << (output_shift & 31U)) |
		     ((normalized & 0x7fffU) >>
		      ((15U - output_shift) & 31U)));
}

static inline int
tx_isp_log2_fixed_u64(unsigned long long value,
		      unsigned int input_precision,
		      unsigned int output_precision)
{
	unsigned int integer_log;
	unsigned int input_scale;

	integer_log = (unsigned int)tx_isp_log2_int_u64(
		value, output_precision, 0);
	input_scale = input_precision << (output_precision & 31U);
	return (int)(integer_log - input_scale);
}

/*
 * Fixed-point 2^x using the 33-entry Q30 Ingenic lookup table. `value` is
 * Q(input_precision), and the result is Q(output_precision). Five fractional
 * bits address the table directly; additional bits linearly interpolate
 * between adjacent entries.
 */
static inline int
tx_isp_exp2_args_valid_u32(unsigned int value,
			   unsigned int input_precision,
			   unsigned int output_precision)
{
	unsigned int integer_part;

	/*
	 * The implementation uses native 32-bit shifts and a Q30 table. Reject
	 * impossible Q formats instead of inheriting MIPS' masked-shift behavior
	 * or allowing a stale ABI argument to index beyond the table.
	 */
	if (input_precision > 31U || output_precision > 30U)
		return 0;

	integer_part = value >> input_precision;
	return integer_part <= 30U - output_precision;
}

static inline unsigned int
tx_isp_exp2_u32(unsigned int value, unsigned int input_precision,
		unsigned int output_precision)
{
	static const unsigned int pow2_lut_q30[33] = {
		0x40000000U, 0x4166c34cU, 0x42d561b4U, 0x444c0740U,
		0x45cae0f2U, 0x47521cc6U, 0x48e1e9baU, 0x4a7a77d4U,
		0x4c1bf829U, 0x4dc69cddU, 0x4f7a9930U, 0x51382182U,
		0x52ff6b55U, 0x54d0ad5aU, 0x56ac1f75U, 0x5891fac1U,
		0x5a82799aU, 0x5c7dd7a4U, 0x5e8451d0U, 0x60962665U,
		0x62b39509U, 0x64dcdec3U, 0x6712460bU, 0x69540ec9U,
		0x6ba27e65U, 0x6dfddbccU, 0x70666f76U, 0x72dc8374U,
		0x75606374U, 0x77f25cceU, 0x7a92be8bU, 0x7d41d96eU,
		0x80000000U,
	};
	unsigned int fractional_mask;
	unsigned int fractional;
	unsigned int integer_part;
	unsigned int right_shift;
	unsigned int index;

	if (!tx_isp_exp2_args_valid_u32(value, input_precision,
					output_precision))
		return 0;

	fractional_mask = (1U << (input_precision & 31U)) - 1U;
	fractional = value & fractional_mask;
	integer_part = value >> (input_precision & 31U);
	right_shift = 30U - output_precision - integer_part;

	if (input_precision < 6U) {
		index = fractional << ((5U - input_precision) & 31U);
		return pow2_lut_q30[index] >> (right_shift & 31U);
	}

	index = fractional >> ((input_precision - 5U) & 31U);
	{
		unsigned int lower = pow2_lut_q30[index];
		unsigned int upper = pow2_lut_q30[index + 1U];
		unsigned int interpolation_bits =
			(input_precision - 5U) & 31U;
		unsigned int interpolation_mask =
			(1U << interpolation_bits) - 1U;
		unsigned int remainder = fractional & interpolation_mask;
		unsigned long long delta =
			(unsigned long long)(upper - lower) * remainder;

		return (lower + (unsigned int)(delta >>
			interpolation_bits)) >> (right_shift & 31U);
	}
}

/*
 * Fixed-point multiplication shared by the T31 and T41 tuning engines.
 * Splitting each operand avoids target-side helper calls while remaining
 * equal to ((unsigned long long)a * b) >> point_pos for Q0 through Q31.
 */
static inline unsigned int
tx_isp_fixmul_u32(unsigned int point_pos, unsigned int first,
		  unsigned int second)
{
	unsigned int mask;
	unsigned int first_integer;
	unsigned int first_fraction;
	unsigned int second_integer;
	unsigned int second_fraction;
	unsigned long long result;

	if (point_pos > 31)
		return 0;

	mask = point_pos ? (1U << point_pos) - 1U : 0;
	first_integer = first >> point_pos;
	first_fraction = first & mask;
	second_integer = second >> point_pos;
	second_fraction = second & mask;

	result = ((unsigned long long)first_integer * second_integer
		  << point_pos);
	result += (unsigned long long)first_integer * second_fraction;
	result += (unsigned long long)first_fraction * second_integer;
	result += ((unsigned long long)first_fraction * second_fraction)
		  >> point_pos;

	return (unsigned int)result;
}

static inline unsigned int
tx_isp_fixmul3_u32(unsigned int point_pos, unsigned int first,
		   unsigned int second, unsigned int third)
{
	return tx_isp_fixmul_u32(point_pos,
		tx_isp_fixmul_u32(point_pos, first, second), third);
}

static inline unsigned int
tx_isp_fixadd_u32(unsigned int left, unsigned int right)
{
	return left + right;
}

static inline unsigned int
tx_isp_fixsub_u32(unsigned int left, unsigned int right)
{
	return left - right;
}

static inline unsigned long long
tx_isp_fixadd_u64(unsigned long long left, unsigned long long right)
{
	return left + right;
}

static inline unsigned long long
tx_isp_fixsub_u64(unsigned long long left, unsigned long long right)
{
	return left - right;
}

/*
 * Round a signed fixed-point value while removing `precision` fractional
 * bits. Ingenic returns the input unchanged for Q0 and Q63+, and for Q1..Q62
 * adds the highest discarded bit to the arithmetic-shifted result.
 */
static inline long long
tx_isp_round_s64(long long value, unsigned int precision)
{
	unsigned int rounding_shift = precision - 1U;

	if (rounding_shift >= 62U)
		return value;

	return (value >> precision) +
		((value >> rounding_shift) & 1LL);
}

/*
 * T23's generation-local split multiply keeps every partial product in 32
 * bits, including the fractional product before its right shift. Preserve
 * that wraparound separately from the full-range T31/T41 primitive above.
 * The vendor routine is only defined for the tuning engine's Q1 through Q31.
 */
static inline unsigned int
tx_isp_fixmul_wrapped_u32_unchecked(unsigned int point_pos,
				    unsigned int first,
				    unsigned int second)
{
	unsigned int mask;
	unsigned int first_integer;
	unsigned int first_fraction;
	unsigned int second_integer;
	unsigned int second_fraction;
	unsigned int result;

	mask = 0xffffffffU >> ((0U - point_pos) & 31U);
	first_integer = first >> point_pos;
	first_fraction = first & mask;
	second_integer = second >> point_pos;
	second_fraction = second & mask;

	result = first_fraction * second_integer;
	result += first_integer * second_fraction;
	result += first_integer * second_integer << point_pos;
	result += (first_fraction * second_fraction) >> point_pos;

	return result;
}

static inline unsigned int
tx_isp_fixmul_wrapped_u32(unsigned int point_pos, unsigned int first,
			  unsigned int second)
{
	if (!point_pos || point_pos > 31U)
		return 0;

	return tx_isp_fixmul_wrapped_u32_unchecked(point_pos, first, second);
}

/*
 * Native-width counterpart used by the tuning paths. Build the full 128-bit
 * product from 32-bit limbs so MIPS32 does not require compiler 128-bit
 * support. The returned low word of (first * second) >> point_pos matches the
 * OEM implementation for the complete unsigned 64-bit input domain.
 */
static inline unsigned long long
tx_isp_fixmul_u64(unsigned int point_pos, unsigned long long first,
		  unsigned long long second)
{
	unsigned long long first_low;
	unsigned long long first_high;
	unsigned long long second_low;
	unsigned long long second_high;
	unsigned long long product_low_low;
	unsigned long long product_low_high;
	unsigned long long product_high_low;
	unsigned long long product_high_high;
	unsigned long long low;
	unsigned long long high;
	unsigned long long addend;
	unsigned long long previous;
	unsigned int carry;

	if (point_pos >= 64U)
		return 0;

	first_low = (unsigned int)first;
	first_high = first >> 32;
	second_low = (unsigned int)second;
	second_high = second >> 32;

	product_low_low = first_low * second_low;
	product_low_high = first_low * second_high;
	product_high_low = first_high * second_low;
	product_high_high = first_high * second_high;

	low = product_low_low;
	high = product_high_high +
		(product_low_high >> 32) +
		(product_high_low >> 32);

	addend = product_low_high << 32;
	previous = low;
	low += addend;
	carry = low < previous;

	addend = product_high_low << 32;
	previous = low;
	low += addend;
	high += carry + (low < previous);

	if (!point_pos)
		return low;

	return (low >> point_pos) | (high << (64U - point_pos));
}

static inline unsigned long long
tx_isp_fixmul3_u64(unsigned int point_pos, unsigned long long first,
		   unsigned long long second, unsigned long long third)
{
	unsigned long long pair =
		tx_isp_fixmul_u64(point_pos, first, second);

	return tx_isp_fixmul_u64(point_pos, pair, third);
}

/*
 * Unsigned Q-format division without a 64-bit divide helper. The remainder is
 * widened before each fractional-bit shift so large denominators retain exact
 * behavior across the full 32-bit input domain.
 */
static inline unsigned int
tx_isp_fixdiv_u32(unsigned int point_pos, unsigned int numerator,
		  unsigned int denominator)
{
	unsigned long long remainder;
	unsigned int quotient;
	unsigned int fractional = 0;
	unsigned int bit;

	if (!denominator || point_pos > 31U)
		return 0;

	quotient = numerator / denominator;
	remainder = numerator % denominator;
	for (bit = 0; bit < point_pos; bit++) {
		remainder <<= 1;
		fractional <<= 1;
		if (remainder >= denominator) {
			fractional |= 1U;
			remainder -= denominator;
		}
	}

	return (quotient << point_pos) | fractional;
}

/*
 * Full-width counterpart for the native tuning ABI. Compute both the integer
 * quotient and the requested fractional bits with shift/subtract division so
 * MIPS32 builds require neither compiler 128-bit support nor a libgcc 64-bit
 * divide helper. The result is the low word of (numerator << point_pos) /
 * denominator across the complete unsigned 64-bit input domain.
 */
static inline unsigned long long
tx_isp_fixdiv_u64(unsigned int point_pos, unsigned long long numerator,
		  unsigned long long denominator)
{
	unsigned long long quotient = 0;
	unsigned long long remainder = 0;
	unsigned long long fractional = 0;
	unsigned long long shifted;
	unsigned int overflow;
	unsigned int bit;

	if (!denominator || point_pos >= 64U)
		return 0;

	for (bit = 64U; bit > 0; bit--) {
		overflow = (unsigned int)(remainder >> 63);
		shifted = (remainder << 1) |
			((numerator >> (bit - 1U)) & 1ULL);
		if (overflow || shifted >= denominator) {
			shifted -= denominator;
			quotient |= 1ULL << (bit - 1U);
		}
		remainder = shifted;
	}

	for (bit = 0; bit < point_pos; bit++) {
		overflow = (unsigned int)(remainder >> 63);
		shifted = remainder << 1;
		fractional <<= 1;
		if (overflow || shifted >= denominator) {
			shifted -= denominator;
			fractional |= 1ULL;
		}
		remainder = shifted;
	}

	return (quotient << point_pos) | fractional;
}

/*
 * Ingenic's generation-local implementation performs the fractional
 * remainder shift in 32 bits. Keep that wrapped behavior as an explicit
 * compatibility primitive alongside the full-range implementation above.
 */
static inline unsigned int
tx_isp_fixdiv_oem_u32(unsigned int point_pos, unsigned int numerator,
		      unsigned int denominator)
{
	unsigned int quotient;
	unsigned int remainder;
	unsigned int fractional = 0;
	unsigned int bit = 0;

	if (!denominator || point_pos > 31U)
		return 0;

	quotient = numerator / denominator;
	remainder = numerator % denominator;
	while (bit != point_pos) {
		remainder <<= 1;
		fractional <<= 1;
		if (denominator < remainder) {
			fractional |= 1U;
			remainder -= denominator;
		} else if (denominator == remainder) {
			fractional |= 1U;
			fractional <<= point_pos - 1U - bit;
			break;
		}
		bit++;
	}

	return (quotient << point_pos) | fractional;
}

/*
 * Legacy Apical scalar helpers used by the T20/T30-era firmware.  These are
 * kept independent of Linux types so their arithmetic can be locked down by
 * host tests and reused by any generation adapter with the same ABI.
 */
static inline unsigned char tx_isp_leading_one_u32(unsigned int value)
{
	unsigned char position = 0;

	if (value >= (1U << 16)) {
		value >>= 16;
		position += 16;
	}
	if (value >= (1U << 8)) {
		value >>= 8;
		position += 8;
	}
	if (value >= (1U << 4)) {
		value >>= 4;
		position += 4;
	}
	if (value >= (1U << 2)) {
		value >>= 2;
		position += 2;
	}
	if (value >= (1U << 1))
		position++;

	return position;
}

static inline int tx_isp_leading_one_u64(unsigned long long value)
{
	int position = 0;

	if (value >= (1ULL << 32)) {
		value >>= 32;
		position += 32;
	}
	if (value >= (1ULL << 16)) {
		value >>= 16;
		position += 16;
	}
	if (value >= (1ULL << 8)) {
		value >>= 8;
		position += 8;
	}
	if (value >= (1ULL << 4)) {
		value >>= 4;
		position += 4;
	}
	if (value >= (1ULL << 2)) {
		value >>= 2;
		position += 2;
	}
	if (value >= (1ULL << 1))
		position++;

	return position;
}

static inline unsigned short tx_isp_isqrt_u32(unsigned int value)
{
	unsigned int result = 0;
	unsigned int bit;

	for (bit = 0; bit < 16U; bit++) {
		unsigned int candidate = result + (0x8000U >> bit);

		if (candidate * candidate <= value)
			result = candidate;
	}

	return (unsigned short)result;
}

static inline unsigned char tx_isp_isqrt_u16(unsigned short value)
{
	unsigned int result = 0;
	unsigned int bit;

	for (bit = 0; bit < 8U; bit++) {
		unsigned int candidate = result + (0x80U >> bit);

		if (candidate * candidate <= value)
			result = candidate;
	}

	return (unsigned char)result;
}

static inline unsigned char tx_isp_log_u16_q4(unsigned short value)
{
	unsigned int result = 0;
	unsigned int bit;

	for (bit = 0; bit < 16U; bit++) {
		unsigned int base = 1U << bit;

		if (base < value)
			result = (bit << 4) - 16U +
				 ((unsigned int)value << 4) / base;
	}

	return (unsigned char)result;
}

static inline unsigned int
tx_isp_multiply_fixed_u32(unsigned int first, unsigned int second,
			  unsigned int output_fraction)
{
	if (output_fraction >= 64U)
		return 0;

	return (unsigned int)(((unsigned long long)first * second) >>
			      output_fraction);
}

static inline int
tx_isp_solve_linear_a_s32(int y1, int y2, int x1, int x2,
			   int fraction_size)
{
	unsigned int numerator;
	int denominator;

	if (x1 == x2)
		return x1;
	numerator = ((unsigned int)y1 - (unsigned int)y2) <<
		    ((unsigned int)fraction_size & 31U);
	denominator = (int)((unsigned int)x1 - (unsigned int)x2);
	return (int)numerator / denominator;
}

static inline int
tx_isp_solve_linear_b_s32(int y1, int slope, int x1,
			   int fraction_size)
{
	unsigned int intercept;

	intercept = (unsigned int)y1 <<
		    ((unsigned int)fraction_size & 31U);
	intercept -= (unsigned int)slope * (unsigned int)x1;
	return (int)intercept;
}

static inline unsigned int
tx_isp_div_fixed_u32(unsigned int numerator, unsigned int denominator,
		     int fraction_size)
{
	unsigned int shift = (unsigned int)fraction_size & 31U;

	if (!denominator)
		return numerator << shift;
	return tx_isp_fixdiv_u32(shift, numerator, denominator);
}

static inline int tx_isp_nth_root_045_s32(int value, int fraction_size)
{
	unsigned int shift = (unsigned int)fraction_size & 31U;
	int scaled = (int)((unsigned int)value << shift);
	unsigned int result = 1U << shift;

	result += (unsigned int)(scaled >> 2);
	result -= (unsigned int)(scaled >> 4);
	return (int)result;
}

static inline unsigned short
tx_isp_line_offset_u16(unsigned short line_length,
		       unsigned char bytes_per_pixel)
{
	return (unsigned short)(((unsigned int)line_length * bytes_per_pixel +
				 127U) & 0xff80U);
}

/*
 * Three-segment Q7 strength blend used by the newer tuning engines.
 *
 * ratio 0..128 fades from zero to `strength`; ratio 128..255 fades from
 * `strength` toward `maximum`.  The recovered ABI passes the ratio in a full
 * word, but the OEM branch decision is made from its low byte.  Keep that
 * narrowing explicit here so adapters cannot accidentally depend on host
 * `char` signedness.
 */
#define TX_ISP_RATIO_Q7_RETURN(ratio, strength, maximum)		\
	do {								\
		if ((ratio) == 0x80)					\
			return (strength);				\
		if ((signed char)((ratio) & 0xff) < 0)			\
			return ((((ratio) - 0x80) *			\
				 ((maximum) - (strength))) >> 7) +	\
				(strength);				\
		return ((ratio) * (strength)) >> 7;			\
	} while (0)

static inline int
tx_isp_ratio_q7_s32(int ratio, int strength, int maximum)
{
	TX_ISP_RATIO_Q7_RETURN(ratio, strength, maximum);
}

#endif /* TX_ISP_MATH_H */
