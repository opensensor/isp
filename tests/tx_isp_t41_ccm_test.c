#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "../driver/t41/tx_isp_t41_ccm.h"
static void put16(unsigned char *p, unsigned int v) { p[0] = v; p[1] = v >> 8; }
static void put32(unsigned char *p, unsigned int v) { put16(p, v); put16(p+2, v >> 16); }
int main(void)
{
	unsigned char storage[149] = {0}, *p = storage + 1;
	short m[9], transformed[9], saved[9];
	unsigned int sat, words[8], i, j;
	for (i = 0; i < 6; ++i) put16(p+i*2, 2000+i*1000);
	for (i = 0; i < 9; ++i) { put32(p+20+i*4, 100+i*100); put16(p+56+i*2, 256-i*16); }
	for (j = 0; j < 4; ++j)
		for (i = 0; i < 9; ++i)
			put16(p+74+j*18+i*2, i%4 == 0 ? 1024+(int)j*100 : -((int)j*50));
	assert(!t41_ccm_select(p, 148, 2500, 150, m, &sat));
	assert(m[0] == 1074 && m[1] == -25 && sat == 248);
	assert(!t41_ccm_select(p, 148, 3500, 0, m, &sat));
	assert(m[0] == 1124 && m[1] == -50 && sat == 256);
	assert(!t41_ccm_saturate(m, 256, 0, NULL, transformed));
	assert(!memcmp(m, transformed, sizeof(m)));
	assert(!t41_ccm_select(p, 148, 100000, ~0U, m, &sat));
	assert(m[0] == 1324 && m[1] == -150 && sat == 128);
	assert(t41_ccm_round(-32, 6) == 0 && t41_ccm_round(-33, 6) == -1);
	p[16] = 1; p[17] = 10; p[18] = 20;
	put16(p+12, 511); put16(p+14, 8191);
	assert(!t41_ccm_pack(p, 148, m, words));
	assert(words[0] == (1324U | ((16384U-150)<<16)));
	assert(words[5] == 0x000a1014 && words[6] == 3 && words[7] == 0x1fff01ff);
	memcpy(saved, m, sizeof(m));
	put16(p+2, 2000);
	assert(t41_ccm_select(p, 148, 2500, 150, m, &sat));
	assert(!memcmp(m, saved, sizeof(m)) && sat == 128);
	put16(p+2, 3000); put16(p+56, 1025);
	assert(t41_ccm_select(p, 148, 2500, 150, m, &sat));
	put16(p+56, 256); put16(p+74, 8192);
	assert(t41_ccm_select(p, 148, 2500, 150, m, &sat));
	assert(!memcmp(m, saved, sizeof(m)));
	assert(t41_ccm_select(p, 147, 0, 0, m, &sat));
	assert(t41_ccm_select(NULL, 148, 0, 0, m, &sat));
	assert(t41_ccm_saturate(m, 1025, 0, NULL, transformed));
	assert(t41_ccm_saturate(m, 256, 6, NULL, transformed));
	puts("T41 CCM calibration selection, rounding, packing and atomic rejection: passed");
	return 0;
}
