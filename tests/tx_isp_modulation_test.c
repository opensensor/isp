#include <assert.h>
#include <stdio.h>

#include "tx_isp/tx_isp_modulation.h"

static void test_pair_modulation(void)
{
	const unsigned short pairs_u16[] = {
		0, 10,
		100, 110,
		200, 210,
	};
	const unsigned int pairs_u32[] = {
		0, 0xffffffffU,
		100, 0xffffff00U,
	};
	unsigned int expected;

	assert(tx_isp_modulate_pairs_u16(0, pairs_u16, 3) == 10);
	assert(tx_isp_modulate_pairs_u16(50, pairs_u16, 3) == 60);
	assert(tx_isp_modulate_pairs_u16(100, pairs_u16, 3) == 110);
	assert(tx_isp_modulate_pairs_u16(150, pairs_u16, 3) == 160);
	assert(tx_isp_modulate_pairs_u16(200, pairs_u16, 3) == 210);

	expected = (0xffffffffU * 128U + 0xffffff00U * 128U) >> 8;
	assert(tx_isp_modulate_pairs_u32(50, pairs_u32, 2) == expected);
	assert(tx_isp_modulate_pairs_u32(0, pairs_u32, 2) ==
	       0xffffffffU);
	assert(tx_isp_modulate_pairs_u32(100, pairs_u32, 2) ==
	       0xffffff00U);

	assert(tx_isp_modulate_pairs_u16(10, NULL, 3) == 0);
	assert(tx_isp_modulate_pairs_u16(10, pairs_u16, 0) == 0);
}

static void test_scaled_pair_modulation(void)
{
	const unsigned short pairs[] = {
		0, 100,
		100, 200,
	};
	const unsigned short zero_endpoint[] = {
		0, 0,
		100, 200,
	};

	assert(tx_isp_modulate_pairs_scaled_u16(0, 50, 400,
					   pairs, 2) == 50);
	assert(tx_isp_modulate_pairs_scaled_u16(50, 50, 400,
					   pairs, 2) == 187);
	assert(tx_isp_modulate_pairs_scaled_u16(100, 50, 400,
					   pairs, 2) == 400);
	assert(tx_isp_modulate_pairs_scaled_u16(50, 50, 400,
					   zero_endpoint, 2) == 0);
}

static void test_adjusted_pair_modulation(void)
{
	const unsigned short pairs[] = {
		0, 10,
		100, 30,
		200, 50,
	};
	const unsigned short invalid_endpoints[] = {
		0, 10,
		100, 10,
	};

	assert(tx_isp_modulate_pairs_adjusted_u16(0, 20, 100,
					     pairs, 3) == 20);
	assert(tx_isp_modulate_pairs_adjusted_u16(50, 20, 100,
					     pairs, 3) == 40);
	assert(tx_isp_modulate_pairs_adjusted_u16(100, 20, 100,
					     pairs, 3) == 60);
	assert(tx_isp_modulate_pairs_adjusted_u16(200, 20, 100,
					     pairs, 3) == 100);
	assert(tx_isp_modulate_pairs_adjusted_u16(50, 20, 100,
					     invalid_endpoints, 2) == 0);
}

static void test_equidistant_modulation(void)
{
	const unsigned short table_u16[] = { 0, 100, 200 };
	const unsigned int table_u32[] = { 0, 100000, 200000 };

	assert(tx_isp_modulate_equidistant_u16(0, table_u16, 3) == 0);
	assert(tx_isp_modulate_equidistant_u16(0x4000, table_u16, 3) ==
	       50);
	assert(tx_isp_modulate_equidistant_u16(0x8000, table_u16, 3) ==
	       100);
	assert(tx_isp_modulate_equidistant_u16(0xffff, table_u16, 3) ==
	       200);
	assert(tx_isp_modulate_equidistant_u32(0x4000, table_u32, 3) ==
	       50000);
	assert(tx_isp_modulate_equidistant_u32(0x8000, table_u32, 3) ==
	       100000);
	assert(tx_isp_modulate_equidistant_u32(0xffff, table_u32, 3) ==
	       200000);
}

static void test_inverse_equidistant_modulation(void)
{
	const unsigned short table_u16[] = { 0, 100, 200 };
	const unsigned int table_u32[] = { 0, 100000, 200000 };

	assert(tx_isp_modulate_inverse_equidistant_u16(
		       0, table_u16, 3) == 0);
	assert(tx_isp_modulate_inverse_equidistant_u16(
		       50, table_u16, 3) == 0x4000);
	assert(tx_isp_modulate_inverse_equidistant_u16(
		       100, table_u16, 3) == 0x8000);
	assert(tx_isp_modulate_inverse_equidistant_u16(
		       200, table_u16, 3) == 0xffff);
	assert(tx_isp_modulate_inverse_equidistant_u32(
		       50000, table_u32, 3) == 0x4000);
	assert(tx_isp_modulate_inverse_equidistant_u32(
		       100000, table_u32, 3) == 0x8000);
	assert(tx_isp_modulate_inverse_equidistant_u32(
		       200000, table_u32, 3) == 0xffff);
}

int main(void)
{
	test_pair_modulation();
	test_scaled_pair_modulation();
	test_adjusted_pair_modulation();
	test_equidistant_modulation();
	test_inverse_equidistant_modulation();
	puts("tx_isp_modulation_test: ok");
	return 0;
}
