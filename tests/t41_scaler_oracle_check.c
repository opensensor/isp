/* Differential tests of the actual OEM instructions on synthetic inputs. */
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "../driver/t41/tx_isp_t41_scaler.h"

unsigned char oracle_bss[65536] __attribute__((aligned(65536)));
extern unsigned int oracle_addresses[668], oracle_values[668], oracle_count;
extern unsigned int oracle_bad_call, oracle_allocated;
extern int oracle_tisp_sin(unsigned int ratio, unsigned int phases, short *curve);
extern int oracle_tisp_msca_normalized(unsigned int stride, unsigned int phases,
				       short *curve);
extern int oracle_tisp_msca_ch_curve_write(unsigned int isp, unsigned short *pairs);

static uint32_t random_state = 0x4d534341;
static uint32_t next_random(void)
{
	random_state = random_state * 1664525U + 1013904223U;
	return random_state;
}

static unsigned int native_count;
static int compare_write(void *context, u32 reg, u32 value)
{
	(void)context;
	assert(native_count < oracle_count);
	if (oracle_addresses[native_count] != reg || oracle_values[native_count] != value) {
		fprintf(stderr, "write %u: native %x=%x, OEM %x=%x\n", native_count,
			reg, value, oracle_addresses[native_count], oracle_values[native_count]);
		return -1;
	}
	++native_count;
	return 0;
}

static void compare_curves(void)
{
	unsigned int channel, trial;

	for (channel = 0; channel < 3; ++channel) {
		unsigned int phases = channel == 1 ? 256 : 32;

		for (trial = 0; trial < 2000; ++trial) {
			short reference[257], native[257];
			unsigned int source = 1 + next_random() % 65535;
			unsigned int target = 128 + next_random() % 65408;
			unsigned int ratio = ((target << 14) + source / 2) / source;
			int ret;

			if (ratio > 27306)
				ratio = 27306;
			/* Exercise quarter scale and unity explicitly as well. */
			if (trial < 2) {
				source = 2560;
				target = trial ? 2560 : 640;
				ratio = trial ? 16384 : 4096;
			}
			assert(ratio);
			oracle_tisp_sin(ratio, phases, reference);
			oracle_tisp_msca_normalized(phases / 4, phases, reference);
			ret = tx_isp_t41_scaler_channel_curve_generate(channel, target,
								 source, native, 257);
			if (ret || memcmp(reference, native, (phases + 1) * 2)) {
				fprintf(stderr, "curve mismatch ch%u %u/%u ratio=%u ret=%d\n",
					channel, target, source, ratio, ret);
				for (unsigned int i = 0; i <= phases; ++i)
					if (native[i] != reference[i])
						fprintf(stderr, "  [%u] %d != %d\n", i, native[i], reference[i]);
				assert(0);
			}
			assert(!oracle_bad_call && !oracle_allocated);
		}
	}
}

static void compare_writers(void)
{
	static const unsigned int offsets[3] = {8, 0x8c, 0x490};
	unsigned char workspace[0x514] __attribute__((aligned(4)));
	unsigned char params[0xe8], desc[3][26];
	unsigned int mask, modes, channel, i;

	memset(oracle_bss, 0, sizeof(oracle_bss));
	*(uint32_t *)(oracle_bss + 0x475c) = (uintptr_t)workspace;
	*(uint32_t *)workspace = (uintptr_t)params;
	for (channel = 0; channel < 3; ++channel)
		*(uint32_t *)(oracle_bss + 0x4750 + 4 * channel) = (uintptr_t)desc[channel];
	for (mask = 0; mask < 8; ++mask) {
		for (modes = 0; modes < 64; ++modes) {
			unsigned short pairs = 7;
			unsigned int native_pairs = 7;

			for (i = 8; i < sizeof(workspace); ++i)
				workspace[i] = next_random() >> 24;
			for (i = 0; i < sizeof(params); ++i)
				params[i] = next_random() >> 24;
			for (i = 0; i < 6; ++i)
				params[0xdf + i] = (modes >> i) & 1;
			memset(desc, 0, sizeof(desc));
			for (channel = 0; channel < 3; ++channel)
				desc[channel][0] = (mask >> channel) & 1;
			oracle_bss[0x4740] = 1;
			oracle_count = native_count = 0;
			oracle_tisp_msca_ch_curve_write(0, &pairs);
			assert(!oracle_bad_call && !oracle_bss[0x4740]);
			for (channel = 0; channel < 3; ++channel) {
				unsigned int count = channel == 1 ? 257 : 33;
				const short *vertical = (const short *)(workspace + offsets[channel]);
				int ret;

				if (!desc[channel][0])
					continue;
				ret = tx_isp_t41_scaler_curve_write(channel, params, sizeof(params),
					vertical, vertical + count, count, compare_write, NULL);
				if (ret < 0)
					fprintf(stderr, "writer mismatch ch%u mask=%u modes=%u\n",
						channel, mask, modes);
				assert(ret > 0);
				native_pairs += ret;
			}
			assert(native_pairs == pairs && native_count == oracle_count);
		}
	}
}

int main(void)
{
	compare_curves();
	compare_writers();
	puts("T41 scaler OEM oracle: 6000 geometry curves + 512 active-channel/mode writers passed");
	return 0;
}
