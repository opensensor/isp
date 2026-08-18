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

int main(void)
{
	test_full_reference_mode();
	test_reduced_memory_mode();
	puts("tx_isp_t31_mdns tests passed");
	return 0;
}
