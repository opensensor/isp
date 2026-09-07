/* Offline synthetic comparison. No calibration bin or device memory input. */
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "../driver/t41/tx_isp_t41_ccm.h"

unsigned char oracle_bss[0x5000] __attribute__((aligned(65536)));
extern int oracle_ct(unsigned int channel, unsigned int ct, unsigned int force);
extern int oracle_ev(unsigned int channel, unsigned int unused, unsigned int ev,
		unsigned int high, unsigned int force);
extern int oracle_saturate(unsigned int channel);
extern int oracle_pack(unsigned int channel);
extern unsigned int oracle_words[8], oracle_bad_write;
static unsigned char params[148], info[196];
static uint32_t seed = 13;
static uint32_t random_u32(void)
{
	seed ^= seed << 13; seed ^= seed >> 17; seed ^= seed << 5;
	return seed;
}
static void put16(unsigned int offset, unsigned int v)
{
	params[offset] = v; params[offset+1] = v >> 8;
}
static void put32(unsigned int offset, unsigned int v)
{
	put16(offset, v); put16(offset+2, v >> 16);
}
int main(void)
{
	unsigned int frame, i, failures = 0;
	short selected[9], transformed[9];
	unsigned int saturation, words[8];
	*(uint32_t *)(void *)(oracle_bss + 0x4790) = (uintptr_t)info;
	*(uint32_t *)(void *)info = (uintptr_t)params;
	for (frame = 0; frame < 10000; ++frame) {
		unsigned int ct = frame % 10000, ev = frame % 1000;
		for (i = 0; i < 6; ++i) put16(i*2, 1800 + i*1000);
		for (i = 0; i < 9; ++i) {
			put32(20+i*4, 10+i*100);
			put16(56+i*2, random_u32() % 1025);
		}
		for (i = 0; i < 36; ++i) put16(74+i*2, (random_u32() & 16383) - 8192);
		for (i = 12; i < 20; ++i) params[i] = random_u32();
		*(uint32_t *)(void *)(info+4) = frame % 6;
		info[192] = 0;
		oracle_ct(0, ct, 1);
		oracle_ev(0, 0, ev, 0, 1);
		if (t41_ccm_select(params, sizeof(params), ct, ev, selected, &saturation))
			return 2;
		if (memcmp(selected, info+156, sizeof(selected)) || saturation != *(uint32_t *)(void *)(info+140)) {
			if (failures++ < 12) printf("select frame=%u ct=%u ev=%u sat=%u/%u\n", frame, ct, ev,
				saturation, *(uint32_t *)(void *)(info+140));
		}
		oracle_saturate(0);
		if (t41_ccm_saturate(selected, saturation, frame%6, NULL, transformed)) return 3;
		for (i = 0; i < 9; ++i)
			if (transformed[i] != t41_ccm_s16(info+174+i*2))
				if (failures++ < 12) printf("transform frame=%u i=%u scalar=%d OEM=%d\n",
					frame, i, transformed[i], t41_ccm_s16(info+174+i*2));
		oracle_pack(0);
		if (t41_ccm_pack(params, sizeof(params), transformed, words)) return 4;
		for (i = 0; i < 8; ++i)
			if (words[i] != oracle_words[i])
				if (failures++ < 12) printf("pack frame=%u i=%u scalar=%x OEM=%x\n",
					frame, i, words[i], oracle_words[i]);
	}
	printf("10000 synthetic CT/EV/CSC cases: %u mismatches, %u unexpected register destinations\n", failures, oracle_bad_write);
	return failures || oracle_bad_write ? 1 : 0;
}
