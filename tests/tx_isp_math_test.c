#include <assert.h>
#include <stdio.h>

#include "tx_isp/tx_isp_math.h"

static unsigned int reference_lerp_u32(unsigned int index,
		unsigned int fraction, const unsigned int *values,
		unsigned int last_index)
{
	unsigned int current;
	unsigned int next;
	unsigned int difference;
	unsigned int product;
	unsigned int step;

	if (index >= last_index)
		return values[last_index];
	current = values[index];
	next = values[index + 1];
	if (current == next || fraction == 0)
		return current;
	difference = current >= next ? current - next : next - current;
	product = difference * fraction;
	step = (product >> 16) + ((product >> 15) & 1U);
	return current >= next ? current - step : current + step;
}

static int reference_lerp_s32(unsigned int index, unsigned int fraction,
		const int *values, unsigned int last_index)
{
	int current;
	int next;
	int difference;
	int step;
	unsigned int product;

	if (index >= last_index)
		return values[last_index];
	current = values[index];
	next = values[index + 1];
	if (current == next || fraction == 0)
		return current;
	difference = current >= next ? current - next : next - current;
	product = (unsigned int)difference * fraction;
	step = ((int)product >> 16) + (int)((product >> 15) & 1U);
	return current >= next ? current - step : current + step;
}

static int reference_log2_int_u32(unsigned int value,
		unsigned int precision, unsigned int output_shift)
{
	unsigned int bit_position = 0;
	unsigned int fraction = 0;
	unsigned int normalized;
	unsigned int scan;
	unsigned int i;

	if (!value)
		return 0;
	for (scan = value; scan > 1; scan >>= 1)
		bit_position++;
	if (bit_position >= 16)
		normalized = value >> (bit_position - 15);
	else
		normalized = value << (15 - bit_position);

	for (i = 0; i < precision; i++) {
		unsigned int square = normalized * normalized;

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

static int reference_log2_int_u64(unsigned long long value,
		unsigned int precision, unsigned int output_shift)
{
	unsigned int bit_position = 0;
	unsigned int fraction = 0;
	unsigned int normalized;
	unsigned long long scan;
	unsigned int i;

	if (!value)
		return 0;
	for (scan = value; scan > 1; scan >>= 1)
		bit_position++;
	if (bit_position >= 16)
		value >>= bit_position - 15;
	else
		value <<= 15 - bit_position;
	normalized = (unsigned int)value;

	for (i = 0; i < precision; i++) {
		unsigned long long square =
			(unsigned long long)normalized * normalized;

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

static void test_unsigned_interpolation(void)
{
	const unsigned int rising[] = { 100, 200, 300 };
	const unsigned int falling[] = { 300, 200, 100 };
	const unsigned int equal[] = { 42, 42 };

	assert(tx_isp_lerp_u32(0, 0, rising, 2) == 100);
	assert(tx_isp_lerp_u32(0, 0x8000, rising, 2) == 150);
	assert(tx_isp_lerp_u32(0, 0x8000, falling, 2) == 250);
	assert(tx_isp_lerp_u32(0, 0xffff, equal, 1) == 42);
	assert(tx_isp_lerp_u32(2, 0x8000, rising, 2) == 300);
	assert(tx_isp_lerp_u32(20, 0x8000, rising, 2) == 300);
}

static void test_oem_rounding(void)
{
	const unsigned int rising[] = { 10, 11 };
	const unsigned int falling[] = { 11, 10 };

	assert(tx_isp_lerp_u32(0, 0x7fff, rising, 1) == 10);
	assert(tx_isp_lerp_u32(0, 0x8000, rising, 1) == 11);
	assert(tx_isp_lerp_u32(0, 0x8000, falling, 1) == 10);
}

static void test_signed_interpolation(void)
{
	const int rising[] = { -200, -100, 0 };
	const int falling[] = { 100, -100, -300 };
	const int wrapped_product[] = { 0, 65535 };

	assert(tx_isp_lerp_s32(0, 0x8000, rising, 2) == -150);
	assert(tx_isp_lerp_s32(0, 0x8000, falling, 2) == 0);
	assert(tx_isp_lerp_s32(2, 0x8000, rising, 2) == 0);
	assert(tx_isp_lerp_s32(0, 0xffff, wrapped_product, 1) == -2);
}

static void test_typed_interpolation(void)
{
	const unsigned char rising_u8[] = { 10, 20, 30 };
	const unsigned char falling_u8[] = { 30, 20, 10 };
	const unsigned short rising_u16[] = { 1000, 2000, 3000 };
	const unsigned short falling_u16[] = { 3000, 2000, 1000 };

	assert(tx_isp_lerp_u8(0, 0x8000, rising_u8, 2) == 15);
	assert(tx_isp_lerp_u8(0, 0x8000, falling_u8, 2) == 25);
	assert(tx_isp_lerp_u8(20, 0x8000, rising_u8, 2) == 30);
	assert(tx_isp_lerp_u16(0, 0x8000, rising_u16, 2) == 1500);
	assert(tx_isp_lerp_u16(0, 0x8000, falling_u16, 2) == 2500);
	assert(tx_isp_lerp_u16(20, 0x8000, rising_u16, 2) == 3000);
}

static void test_fixed_point_multiply(void)
{
	unsigned int seed = 0x41f31a23U;
	unsigned int iteration;
	unsigned int point_pos;

	assert(tx_isp_fixmul_u32(10, 0x400, 0x400) == 0x400);
	assert(tx_isp_fixmul_u32(10, 0x600, 0x800) == 0xc00);
	assert(tx_isp_fixmul_u32(32, 1, 1) == 0);
	assert(tx_isp_fixmul3_u32(10, 0x400, 0x800, 0xc00) == 0x1800);

	for (iteration = 0; iteration < 10000; iteration++) {
		unsigned int first;
		unsigned int second;
		unsigned int third;

		seed = seed * 1664525U + 1013904223U;
		first = seed;
		seed = seed * 1664525U + 1013904223U;
		second = seed;
		seed = seed * 1664525U + 1013904223U;
		third = seed;

		for (point_pos = 0; point_pos < 32; point_pos++) {
			unsigned int pair = (unsigned int)
				(((unsigned long long)first * second) >> point_pos);
			unsigned int triple = (unsigned int)
				(((unsigned long long)pair * third) >> point_pos);

			assert(tx_isp_fixmul_u32(point_pos, first, second) ==
			       pair);
			assert(tx_isp_fixmul3_u32(point_pos, first, second,
						 third) == triple);
		}
	}
}

static void test_fixed_point_add_subtract(void)
{
	assert(tx_isp_fixadd_u32(0x400, 0x800) == 0xc00);
	assert(tx_isp_fixadd_u32(0xffffffffU, 1) == 0);
	assert(tx_isp_fixsub_u32(0xc00, 0x800) == 0x400);
	assert(tx_isp_fixsub_u32(0, 1) == 0xffffffffU);
	assert(tx_isp_fixadd_u64(0x100000000ULL, 0x200000000ULL) ==
	       0x300000000ULL);
	assert(tx_isp_fixadd_u64(0xffffffffffffffffULL, 1) == 0);
	assert(tx_isp_fixsub_u64(0x300000000ULL, 0x200000000ULL) ==
	       0x100000000ULL);
	assert(tx_isp_fixsub_u64(0, 1) == 0xffffffffffffffffULL);
}

static long long reference_round_s64(long long value,
				      unsigned int precision)
{
	if ((precision - 1U) >= 62U)
		return value;

	return (value >> precision) +
		((value >> (precision - 1U)) & 1LL);
}

static void test_fixed_point_round_s64(void)
{
	unsigned int seed = 0x6400d123U;
	unsigned int iteration;

	assert(tx_isp_round_s64(0x180, 8) == 2);
	assert(tx_isp_round_s64(0x17f, 8) == 1);
	assert(tx_isp_round_s64(-0x180, 8) == -1);
	assert(tx_isp_round_s64(-0x181, 8) == -2);
	assert(tx_isp_round_s64(123, 0) == 123);
	assert(tx_isp_round_s64(123, 63) == 123);

	for (iteration = 0; iteration < 100000; iteration++) {
		unsigned long long bits;
		unsigned int precision = iteration % 64U;

		seed = seed * 1664525U + 1013904223U;
		bits = (unsigned long long)seed << 32;
		seed = seed * 1664525U + 1013904223U;
		bits |= seed;
		assert(tx_isp_round_s64((long long)bits, precision) ==
		       reference_round_s64((long long)bits, precision));
	}
}

static unsigned int
reference_t23_fixmul_u32(unsigned int point_pos, unsigned int first,
			 unsigned int second)
{
	unsigned int mask = 0xffffffffU >> ((0U - point_pos) & 31U);
	unsigned int first_integer = first >> point_pos;
	unsigned int second_integer = second >> point_pos;
	unsigned int first_fraction = first & mask;
	unsigned int second_fraction = second & mask;
	unsigned int result =
		first_fraction * second_integer +
		first_integer * second_fraction +
		(first_integer * second_integer << point_pos);

	return result + (first_fraction * second_fraction >> point_pos);
}

static void test_t23_fixed_point_multiply_equivalence(void)
{
	unsigned int seed = 0x23f17a2bU;
	unsigned int iteration;
	unsigned int point_pos;

	for (iteration = 0; iteration < 10000; iteration++) {
		unsigned int first;
		unsigned int second;

		seed = seed * 1664525U + 1013904223U;
		first = seed;
		seed = seed * 1664525U + 1013904223U;
		second = seed;

		for (point_pos = 1; point_pos < 32; point_pos++)
			assert(tx_isp_fixmul_wrapped_u32(point_pos, first,
							 second) ==
			       reference_t23_fixmul_u32(point_pos, first,
							second));
	}

	assert(tx_isp_fixmul_wrapped_u32(0, 1, 1) == 0);
	assert(tx_isp_fixmul_wrapped_u32(32, 1, 1) == 0);
}

static void test_fixed_point_multiply_64(void)
{
	unsigned int seed = 0x64f17a2bU;
	unsigned int iteration;

	assert(tx_isp_fixmul_u64(10, 0x400, 0x400) == 0x400);
	assert(tx_isp_fixmul_u64(20, 0x180000, 0x200000) == 0x300000);
	assert(tx_isp_fixmul_u64(64, 1, 1) == 0);
	assert(tx_isp_fixmul3_u64(10, 0x400, 0x800, 0xc00) == 0x1800);

	for (iteration = 0; iteration < 10000; iteration++) {
		unsigned long long first;
		unsigned long long second;
		unsigned long long third;
		unsigned long long pair;
		unsigned long long triple;
		unsigned int point_pos = iteration % 64U;

		seed = seed * 1664525U + 1013904223U;
		first = (unsigned long long)seed << 32;
		seed = seed * 1664525U + 1013904223U;
		first |= seed;
		seed = seed * 1664525U + 1013904223U;
		second = (unsigned long long)seed << 32;
		seed = seed * 1664525U + 1013904223U;
		second |= seed;
		seed = seed * 1664525U + 1013904223U;
		third = (unsigned long long)seed << 32;
		seed = seed * 1664525U + 1013904223U;
		third |= seed;

		pair = (unsigned long long)
			(((__uint128_t)first * second) >> point_pos);
		triple = (unsigned long long)
			(((__uint128_t)pair * third) >> point_pos);
		assert(tx_isp_fixmul_u64(point_pos, first, second) == pair);
		assert(tx_isp_fixmul3_u64(point_pos, first, second, third) ==
		       triple);
	}

	assert(tx_isp_fixmul_u64(
		       63, 0xffffffffffffffffULL, 0xffffffffffffffffULL) ==
	       (unsigned long long)
		       (((__uint128_t)0xffffffffffffffffULL *
			 0xffffffffffffffffULL) >> 63));
}

static void test_fixed_point_divide(void)
{
	unsigned int seed = 0xd17331a4U;
	unsigned int iteration;

	assert(tx_isp_fixdiv_u32(10, 0x400, 0x400) == 0x400);
	assert(tx_isp_fixdiv_u32(10, 0x600, 0x400) == 0x600);
	assert(tx_isp_fixdiv_u32(16, 0x385b0000U, 0x27100000U) ==
	       (unsigned int)(((unsigned long long)0x385b0000U << 16) /
			      0x27100000U));
	assert(tx_isp_fixdiv_u32(10, 1, 0) == 0);
	assert(tx_isp_fixdiv_u32(32, 1, 1) == 0);
	assert(tx_isp_fixdiv_oem_u32(10, 0x400, 0x400) == 0x400);
	assert(tx_isp_fixdiv_oem_u32(10, 1, 0) == 0);
	assert(tx_isp_fixdiv_oem_u32(32, 1, 1) == 0);

	for (iteration = 0; iteration < 100000; iteration++) {
		unsigned int numerator;
		unsigned int denominator;
		unsigned int point_pos;
		unsigned int expected;

		seed = seed * 1664525U + 1013904223U;
		numerator = seed;
		seed = seed * 1664525U + 1013904223U;
		denominator = seed ? seed : 1;
		point_pos = iteration % 32;
		expected = (unsigned int)
			(((unsigned long long)numerator << point_pos) /
			 denominator);

		assert(tx_isp_fixdiv_u32(point_pos, numerator,
					 denominator) == expected);
	}

	assert(tx_isp_fixdiv_u64(10, 0x400, 0x400) == 0x400);
	assert(tx_isp_fixdiv_u64(10, 0x600, 0x400) == 0x600);
	assert(tx_isp_fixdiv_u64(0, 0xffffffffffffffffULL,
				 0xffffffffffffffffULL) == 1);
	assert(tx_isp_fixdiv_u64(63, 0xffffffffffffffffULL,
				 0xffffffffffffffffULL) ==
	       0x8000000000000000ULL);
	assert(tx_isp_fixdiv_u64(10, 1, 0) == 0);
	assert(tx_isp_fixdiv_u64(64, 1, 1) == 0);

	for (iteration = 0; iteration < 10000; iteration++) {
		unsigned long long numerator;
		unsigned long long denominator;
		unsigned long long expected;
		unsigned int point_pos = iteration % 64U;

		seed = seed * 1664525U + 1013904223U;
		numerator = (unsigned long long)seed << 32;
		seed = seed * 1664525U + 1013904223U;
		numerator |= seed;
		seed = seed * 1664525U + 1013904223U;
		denominator = (unsigned long long)seed << 32;
		seed = seed * 1664525U + 1013904223U;
		denominator |= seed;
		if (!denominator)
			denominator = 1;

		expected = (unsigned long long)
			(((__uint128_t)numerator << point_pos) /
			 denominator);
		assert(tx_isp_fixdiv_u64(point_pos, numerator,
					 denominator) == expected);
	}
}

static void test_fixed_point_log2(void)
{
	unsigned int seed = 0x5a17c9e3U;
	unsigned int iteration;

	assert(tx_isp_log2_int_u32(0, 16, 0) == 0);
	assert(tx_isp_log2_int_u32(1, 16, 0) == 0);
	assert(tx_isp_log2_int_u32(2, 16, 0) == (1 << 16));
	assert(tx_isp_log2_fixed_u32(0x400, 10, 16) == 0);
	assert(tx_isp_log2_fixed_u32(0x800, 10, 16) == (1 << 16));
	assert(tx_isp_log2_int_u64(1ULL << 40, 16, 0) ==
	       (40 << 16));
	assert(tx_isp_log2_fixed_u64(1ULL << 40, 40, 16) == 0);

	for (iteration = 0; iteration < 20000; iteration++) {
		unsigned int value;
		unsigned int precision;
		unsigned int output_shift;
		unsigned long long value64;

		seed = seed * 1664525U + 1013904223U;
		value = seed;
		seed = seed * 1664525U + 1013904223U;
		value64 = (unsigned long long)value << 32 | seed;
		precision = iteration % 21;
		output_shift = iteration % 16;

		assert(tx_isp_log2_int_u32(value, precision,
					   output_shift) ==
		       reference_log2_int_u32(value, precision,
					      output_shift));
		assert(tx_isp_log2_int_u64(value64, precision,
					   output_shift) ==
		       reference_log2_int_u64(value64, precision,
					      output_shift));
		assert(tx_isp_log2_fixed_u32(value, precision,
					     output_shift) ==
		       reference_log2_int_u32(value, output_shift, 0) -
			       (int)(precision << (output_shift & 31U)));
		assert(tx_isp_log2_fixed_u64(value64, precision,
					     output_shift) ==
		       reference_log2_int_u64(value64, output_shift, 0) -
			       (int)(precision << (output_shift & 31U)));
	}
}

static void test_fixed_point_exp2(void)
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
	unsigned int index;
	unsigned int value;

	assert(tx_isp_exp2_u32(0, 16, 16) == 0x10000U);
	assert(tx_isp_exp2_u32(1U << 16, 16, 16) == 0x20000U);
	assert(tx_isp_exp2_u32(1U << 15, 16, 16) ==
	       (pow2_lut_q30[16] >> 14));
	assert(!tx_isp_exp2_args_valid_u32(0, 32, 16));
	assert(!tx_isp_exp2_args_valid_u32(0, 16, 31));
	assert(!tx_isp_exp2_args_valid_u32(31U << 16, 16, 0));
	assert(tx_isp_exp2_u32(0, 32, 16) == 0);
	assert(tx_isp_exp2_u32(0, 16, 31) == 0);
	assert(tx_isp_exp2_u32(31U << 16, 16, 0) == 0);

	for (index = 0; index < 32; index++)
		assert(tx_isp_exp2_u32(index, 5, 16) ==
		       (pow2_lut_q30[index] >> 14));

	for (value = 0; value < (1U << 16); value += 97) {
		unsigned int table_index = value >> 11;
		unsigned int remainder = value & 0x7ffU;
		unsigned long long delta =
			(unsigned long long)
			(pow2_lut_q30[table_index + 1] -
			 pow2_lut_q30[table_index]) * remainder;
		unsigned int expected =
			(pow2_lut_q30[table_index] +
			 (unsigned int)(delta >> 11)) >> 14;

		assert(tx_isp_exp2_u32(value, 16, 16) == expected);
	}
}

static void test_reference_equivalence(void)
{
	unsigned int unsigned_values[11];
	int signed_values[9];
	unsigned int seed = 0x31415926U;
	unsigned int iteration;
	unsigned int i;

	for (iteration = 0; iteration < 10000; iteration++) {
		for (i = 0; i < 11; i++) {
			seed = seed * 1664525U + 1013904223U;
			unsigned_values[i] = seed;
			if (i < 9)
				signed_values[i] = (int)(seed & 0xffffU) - 0x8000;
		}

		seed = seed * 1664525U + 1013904223U;
		for (i = 0; i <= 12; i++) {
			assert(tx_isp_lerp_u32(i, seed, unsigned_values, 10) ==
				reference_lerp_u32(i, seed, unsigned_values, 10));
			assert(tx_isp_lerp_s32(i, seed, signed_values, 8) ==
				reference_lerp_s32(i, seed, signed_values, 8));
		}
	}
}

int main(void)
{
	test_unsigned_interpolation();
	test_oem_rounding();
	test_signed_interpolation();
	test_typed_interpolation();
	test_fixed_point_multiply();
	test_fixed_point_add_subtract();
	test_fixed_point_round_s64();
	test_t23_fixed_point_multiply_equivalence();
	test_fixed_point_multiply_64();
	test_fixed_point_divide();
	test_fixed_point_log2();
	test_fixed_point_exp2();
	test_reference_equivalence();
	puts("tx_isp_math_test: ok");
	return 0;
}
