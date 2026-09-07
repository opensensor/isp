#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "../driver/t41/tx_isp_t41_awb.h"

static void put16(unsigned char *p, unsigned int v)
{
	p[0] = v;
	p[1] = v >> 8;
}

int main(void)
{
	/* Synthetic, deliberately asymmetric calibration; no sensor bin needed. */
	unsigned char p[T41_AWB_HW_PARAM_BYTES + 1], *params = p + 1;
	unsigned int words[10], thresholds[5], i, j, gain;
	unsigned short values[11];
	memset(p, 0, sizeof(p));
	put16(params + 0xc6c, 17);
	put16(params + 0xc6e, 7);
	put16(params + 0xc70, 23);
	put16(params + 0xc72, 9);
	for (i = 0; i < 30; ++i) put16(params + 0xc74 + i * 2, i + 1);
	put16(params + 0xcc8, 2);
	put16(params + 0xcca, 1);
	put16(params + 0xccc, 3);
	assert(!t41_awb_geometry(params, T41_AWB_HW_PARAM_BYTES, words));
	assert(words[0] == 0x90177011);
	assert(words[1] == 0x04030201 && words[4] == 0x000f0e0d);
	assert(words[5] == 0x13121110 && words[8] == 0x001e1d1c);
	assert(words[9] == 0x17012103);
	put16(params + 0xc6e, 16);
	assert(t41_awb_geometry(params, T41_AWB_HW_PARAM_BYTES, words));
	assert(words[0] == 0x90177011);
	put16(params + 0xc6e, 7);
	put16(params + 0xc74, 256);
	assert(t41_awb_geometry(params, T41_AWB_HW_PARAM_BYTES, words));
	put16(params + 0xc74, 1);
	assert(t41_awb_geometry(params, T41_AWB_HW_PARAM_BYTES - 1, words));
	assert(t41_awb_geometry(NULL, T41_AWB_HW_PARAM_BYTES, words));
	for (i = 0; i < 11; ++i)
		for (j = 0; j < 11; ++j)
			put16(params + 0xd94 + i * 22 + j * 2,
				(i & 1) ? 3000 - i * 100 - j * 7 : i * 100 + j * 7);
	put16(params + 0xcc6, 3);
	for (gain = 0; gain <= 12 * 65536; gain += 1024) {
		assert(!t41_awb_thresholds(params, T41_AWB_HW_PARAM_BYTES,
			gain, values, thresholds));
		for (i = 0; i < 11; ++i) {
			unsigned int g = gain > 10 * 65536 ? 10 * 65536 : gain;
			unsigned int step = (g * 7 + 32768) >> 16;
			assert(values[i] == ((i & 1) ? 3000 - i * 100 - step :
				i * 100 + step));
		}
		for (i = 0; i < 4; ++i)
			assert(thresholds[i] == (values[i * 2] |
				((unsigned int)values[i * 2 + 1] << 16)));
		assert(thresholds[4] == (values[8] | ((unsigned int)values[9] << 8) |
			((unsigned int)values[10] << 16) | 0x03000000));
	}
	assert(t41_awb_thresholds(params, T41_AWB_HW_PARAM_BYTES - 1,
		0, values, thresholds));
	puts("T41 calibration-driven AWB register packing: passed");
	return 0;
}
