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

#endif /* TX_ISP_MATH_H */
