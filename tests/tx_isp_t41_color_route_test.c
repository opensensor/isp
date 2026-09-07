/* Test the actual exposure adapter with private register-write stubs. */
#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include "../driver/t41/tx_isp_t41_exposure.h"
#include "../driver/include/tx_isp/tx_isp_top.h"
static unsigned int count, ccm_calls, last_address, last_value;
int32_t system_reg_write(uint32_t address, uint32_t value)
{
	assert(address == 0x8000 || address == 0x8004 ||
	       address == 0x8040 || address == 0x40);
	++count; last_address = address; last_value = value;
	return 0;
}
void tx_isp_t41_calibrated_ccm_apply(void) { ++ccm_calls; }
int main(void)
{
	unsigned int bit, initial;
	unsigned char flags[32] = {0};
	unsigned int top = 0xffffffffU;
	for (bit = 0; bit < 32; ++bit) flags[bit] = bit & 1;
	assert(!tx_isp_top_restore(flags, 32, 0xffffffffU, &top));
	assert(top == 0xaaaaaaaaU);
	top = 0x55555555U;
	assert(!tx_isp_top_restore(flags, 32, 0x000f000f, &top));
	assert(top == 0x555a555aU);
	flags[19] = 2;
	assert(tx_isp_top_restore(flags, 32, 1U << 19, &top));
	assert(tx_isp_top_restore(flags, 31, 1, &top));
	assert(top == 0x555a555aU);
	for (bit = 0; bit < 2; ++bit)
		for (initial = 0; initial < 2; ++initial) {
			unsigned int top = 0xa5a50005 | (initial << 9);
			assert(!tx_isp_t41_flicker_profile_apply(0, false,
				1024, 0, 0, bit, &top));
			assert(top == (0xa5a50005 | (bit << 9)));
			assert(last_address == 0x40 && last_value == top);
		}
	assert(count == 16 && ccm_calls == 4);
	initial = 0x12345678;
	assert(tx_isp_t41_flicker_profile_apply(0, false,
		1024, 0, 0, 2, &initial) == -EINVAL);
	assert(initial == 0x12345678 && count == 16 && ccm_calls == 4);
	puts("T41 calibrated CCM routing, other-bit preservation and atomic rejection: passed");
	return 0;
}
