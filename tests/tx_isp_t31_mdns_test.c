#include <assert.h>
#include <stdio.h>

#include "../driver/t31/tx_isp_t31_mdns.h"

static void test_full_reference_mode(void)
{
	assert(tx_isp_t31_mdns_top1(1, 1, 1, 1, 1, 0xf, false) ==
	       0x00f11111U);
}

static void test_reduced_memory_mode(void)
{
	assert(tx_isp_t31_mdns_top1(1, 1, 1, 1, 1, 0xf, true) ==
	       0x00f01100U);
}

static void test_ratio_is_derived_from_immutable_base(void)
{
	const u32 base[9] = { 4, 8, 16, 32, 64, 96, 128, 160, 200 };
	u32 scaled[9];
	u32 first[9];
	unsigned int i;

	tx_isp_t31_mdns_scale_ratio_table(scaled, base, 160);
	for (i = 0; i < 9; i++)
		first[i] = scaled[i];

	/* Reapplying a setting must not compound the previous result. */
	tx_isp_t31_mdns_scale_ratio_table(scaled, base, 160);
	for (i = 0; i < 9; i++)
		assert(scaled[i] == first[i]);

	/* The neutral setting must reproduce the source tuning table. */
	tx_isp_t31_mdns_scale_ratio_table(scaled, base, 128);
	for (i = 0; i < 9; i++)
		assert(scaled[i] == base[i]);
}

int main(void)
{
	test_full_reference_mode();
	test_reduced_memory_mode();
	test_ratio_is_derived_from_immutable_base();
	puts("tx_isp_t31_mdns tests passed");
	return 0;
}
