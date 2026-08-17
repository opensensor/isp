#include <assert.h>
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "../driver/t31/tx_isp_t31_adr.h"

static void test_absolute_strength_scaling(void)
{
	static const u32 base[TX_ISP_T31_ADR_MAP_CHANNELS]
		[TX_ISP_T31_ADR_MAP_POINTS] = {
		{ 160, 180, 200, 220, 240, 260, 280, 300, 320 },
		{ 200, 225, 250, 275, 300, 325, 350, 375, 400 },
		{ 240, 270, 300, 330, 360, 390, 420, 450, 480 },
		{ 260, 290, 320, 350, 380, 410, 440, 470, 500 },
	};
	static const u32 minimum[TX_ISP_T31_ADR_MAP_CHANNELS] = {
		32, 64, 96, 128,
	};
	u32 base_copy[TX_ISP_T31_ADR_MAP_CHANNELS]
		[TX_ISP_T31_ADR_MAP_POINTS];
	u32 first[TX_ISP_T31_ADR_MAP_CHANNELS]
		[TX_ISP_T31_ADR_MAP_POINTS];
	u32 second[TX_ISP_T31_ADR_MAP_CHANNELS]
		[TX_ISP_T31_ADR_MAP_POINTS];
	const u32 *source[TX_ISP_T31_ADR_MAP_CHANNELS];
	u32 *first_out[TX_ISP_T31_ADR_MAP_CHANNELS];
	u32 *second_out[TX_ISP_T31_ADR_MAP_CHANNELS];
	u32 channel;

	memcpy(base_copy, base, sizeof(base));
	for (channel = 0; channel < TX_ISP_T31_ADR_MAP_CHANNELS;
	     ++channel) {
		source[channel] = base[channel];
		first_out[channel] = first[channel];
		second_out[channel] = second[channel];
	}

	assert(tx_isp_t31_adr_scale_mapb(64, minimum, source,
					 first_out) == 0);
	assert(tx_isp_t31_adr_scale_mapb(64, minimum, source,
					 second_out) == 0);
	assert(memcmp(first, second, sizeof(first)) == 0);
	assert(memcmp(base, base_copy, sizeof(base)) == 0);

	assert(tx_isp_t31_adr_scale_mapb(255, minimum, source,
					 second_out) == 0);
	assert(memcmp(base, base_copy, sizeof(base)) == 0);
	assert(tx_isp_t31_adr_scale_mapb(256, minimum, source,
					 second_out) == -EINVAL);
}

int main(void)
{
	test_absolute_strength_scaling();
	puts("tx_isp_t31_adr tests passed");
	return 0;
}
