#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "../driver/t41/tx_isp_t41_tmo_map.h"

static unsigned char params[T41_TMO_PARAM_BYTES];
static unsigned int sums[3750], counts[3750], history[3750], output[3750];

static void run(int first)
{
	assert(!t41_tmo_map(params, sizeof(params), 8, sums, counts, history, first, output));
}

int main(void)
{
	unsigned int b, i;
	params[0x1739] = 1;
	params[0x173b] = 3;
	params[0x173a] = 2;
	params[0x1742] = 4;
	/* Missing bins are zero padded; the lower-coordinate clamp is optional. */
	run(1);
	for (i = 0; i < 3750; ++i) assert(output[i] == 0);
	params[0x173c] = 1;
	run(1);
	for (b = 0; b < 10; ++b)
		for (i = 0; i < 375; ++i)
			assert(output[b*375+i] == (b*4095+5)/10);
	for (i = 0; i < 3750; ++i) history[i] = 4095;
	run(0);
	assert(output[0] == (4095*15)/16);
	assert(output[9*375] == (4095*15+3686)/16);
	/* A single populated histogram bin changes a neighbourhood, not a
	 * hard-coded scene map. With radius zero it must not affect other tiles. */
	params[0x173c] = 0;
	params[0x1742] = 0;
	counts[187*10+5] = 4096;
	sums[187*10+5] = 4096;
	run(1);
	assert(output[5*375+187] > 2000);
	assert(output[5*375+186] == 0);
	assert(output[5*375+188] == 0);
	params[0x1742] = 4;
	run(1);
	assert(output[5*375+186] > 2000);
	assert(output[5*375+188] == output[5*375+186]);
	assert(output[5*375+162] == output[5*375+186]);
	/* Rejection is atomic, including invalid values late in each buffer. */
	memset(output, 0x5a, sizeof(output));
	params[0x1739] = 2;
	assert(t41_tmo_map(params, sizeof(params), 8, sums, counts, history, 0, output));
	params[0x1739] = 0;
	params[0x3a7+374] = 15;
	assert(t41_tmo_map(params, sizeof(params), 8, sums, counts, history, 0, output));
	params[0x3a7+374] = 0;
	counts[3749] = 65536;
	assert(t41_tmo_map(params, sizeof(params), 8, sums, counts, history, 0, output));
	counts[3749] = 0;
	history[3749] = 4096;
	assert(t41_tmo_map(params, sizeof(params), 8, sums, counts, history, 0, output));
	history[3749] = 0;
	assert(t41_tmo_map(params, sizeof(params)-1, 8, sums, counts, history, 0, output));
	assert(t41_tmo_map(params, sizeof(params), 10, sums, counts, history, 0, output));
	assert(t41_tmo_map(NULL, sizeof(params), 8, sums, counts, history, 0, output));
	for (i = 0; i < 3750; ++i) assert(output[i] == 0x5a5a5a5a);
	puts("T41 scalar local TMO: padding, spatial response, history and rejection passed");
	return 0;
}
