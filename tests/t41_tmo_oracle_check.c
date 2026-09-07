/* OFFLINE userspace test; no MMIO, ISP ioctls or live memory access. */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "../driver/t41/tx_isp_t41_tmo_map.h"

unsigned char oracle_bss[0x6000] __attribute__((aligned(65536)));
unsigned char oracle_data[0x6000] __attribute__((aligned(65536)));
extern unsigned char oracle_rodata[];
extern void oracle_fpga(unsigned int channel);
static uint32_t info[12], geom[6], sums[3750], counts[3750];
static uint32_t reference[3750], previous[3750], scalar[3750], history[3750];
static unsigned char params[T41_TMO_PARAM_BYTES], refparams[T41_TMO_PARAM_BYTES];
static uint32_t random_state = 1;

static uint32_t random_u32(void)
{
	random_state ^= random_state << 13;
	random_state ^= random_state >> 17;
	random_state ^= random_state << 5;
	return random_state;
}

static void pointer(unsigned int offset, void *value)
{
	*(uint32_t *)(void *)(oracle_bss + offset) = (uintptr_t)value;
}

int main(int argc, char **argv)
{
	unsigned int frame, i, sk, rk, radius, shift, first, failures = 0;
	clock_t start = clock();
	(void)argc; (void)argv;
	pointer(0x4fdc, info); pointer(0x4fcc, reference);
	pointer(0x4fd0, previous); pointer(0x4fd4, counts); pointer(0x4fd8, sums);
	info[0] = (uintptr_t)refparams; info[3] = (uintptr_t)geom;
	geom[0] = 375;
	((unsigned char *)geom)[13] = 25;
	((unsigned char *)geom)[15] = 4;
	((unsigned char *)geom)[20] = 19;
	((unsigned char *)geom)[21] = 29;
	for (sk = 0; sk < 11; ++sk)
		for (i = 0; i < 81; ++i) {
			int x = (int)(i % 9) - 4, y = (int)(i / 9) - 4, swap;
			if (x < 0) x = -x;
			if (y < 0) y = -y;
			if (x > y) { swap = x; x = y; y = swap; }
			if (t41_tmo_spatial_kernels[sk][y*(y+1)/2+x] != oracle_rodata[0x3090+sk*81+i])
				return 3;
		}
	for (rk = 0; rk < 15; ++rk)
		for (i = 0; i < 5; ++i)
			if (t41_tmo_range_kernels[rk][i < 2 ? 2-i : i-2] != oracle_rodata[0x340c+rk*5+i])
				return 4;
	puts("All 891 spatial and 75 range coefficients match the sensor-independent driver kernels");
	for (frame = 0; frame < 660; ++frame) {
		sk = frame % 11; rk = (frame / 11) % 15;
		radius = frame % 5; shift = frame % 10; first = (frame % 3 == 0);
		params[0x1739] = frame & 1;
		params[0x173a] = rk; params[0x173b] = sk;
		params[0x173c] = (frame >> 1) & 1; params[0x1742] = radius;
		for (i = 0; i < 375; ++i) {
			params[0x230+i] = (sk + i) % 11;
			params[0x3a7+i] = (rk + i) % 15;
		}
		for (i = 0; i < 3750; ++i) {
			counts[i] = frame == 0 ? 0 : random_u32() & 65535;
			sums[i] = frame == 0 ? 0 : random_u32() & 65535;
			previous[i] = history[i] = random_u32() & 4095;
			reference[i] = scalar[i] = 0xdeadbeef;
		}
		memcpy(refparams, params, sizeof(params));
		((unsigned char *)geom)[22] = shift;
		oracle_data[0x5de2] = first;
		oracle_fpga(0);
		if (t41_tmo_map(params, sizeof(params), shift, sums, counts, history, first, scalar))
			return 5;
		for (i = 0; i < 3750; ++i)
			if (reference[i] != scalar[i]) {
				if (failures++ < 20)
					printf("frame=%u mode=%u r=%u sk/rk=%u/%u shift=%u first=%u i=%u vendor=%u scalar=%u\n",
						frame, params[0x1739], radius, sk, rk, shift, first, i, reference[i], scalar[i]);
			}
	}
	printf("660 synthetic frames / 2475000 coefficients: %u mismatches, %.3f CPU seconds\n",
		failures, (double)(clock()-start)/CLOCKS_PER_SEC);
	return failures ? 1 : 0;
}
