#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "../driver/t41/tx_isp_t41_tmo.h"

static void put16(unsigned char *p, unsigned int v)
{
	p[0] = v; p[1] = v >> 8;
}

static void put32(unsigned char *p, unsigned int v)
{
	put16(p, v); put16(p + 2, v >> 16);
}

int main(void)
{
	static const unsigned int offsets[10] = {
		4950, 5416, 1416, 1818, 2220, 2622, 3024, 3426, 3828, 4230,
	};
	unsigned char params[T41_TMO_PARAM_BYTES];
	unsigned short curve[T41_TMO_CURVE_ENTRIES];
	unsigned int i, j;
	memset(params, 0, sizeof(params));
	for (i = 0; i < 10; ++i) {
		put32(params + 1312 + i * 4, 100 + i * 100);
		for (j = 0; j < T41_TMO_CURVE_ENTRIES; ++j)
			put16(params + offsets[i] + j * 2, i * 500 + j);
	}
	assert(!t41_tmo_curve(params, sizeof(params), 0, curve));
	assert(curve[0] == 0 && curve[200] == 200);
	assert(!t41_tmo_curve(params, sizeof(params), 150, curve));
	assert(curve[0] == 250 && curve[200] == 450);
	assert(!t41_tmo_curve(params, sizeof(params), 1000, curve));
	assert(curve[0] == 4500 && curve[200] == 4700);
	assert(!t41_tmo_curve(params, sizeof(params), ~0U, curve));
	assert(curve[0] == 4500);
	assert(t41_tmo_curve(params, sizeof(params) - 1, 0, curve));
	put32(params + 1316, 100);
	assert(t41_tmo_curve(params, sizeof(params), 0, curve));
	assert(curve[0] == 4500); /* Invalid parameters leave output untouched. */
	put32(params + 1316, 200);
	put16(params + offsets[9] + 400, 8192);
	assert(t41_tmo_curve(params, sizeof(params), 0, curve));
	put16(params + offsets[9] + 400, 4700);
	put16(params + 0x55c, 1);
	assert(t41_tmo_curve(params, sizeof(params), 0, curve));
	puts("T41 checked mode-0 TMO EV interpolation: passed");
	return 0;
}
