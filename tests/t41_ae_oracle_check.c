/* Synthetic calibrations/statistics only. OEM instructions are test-only. */
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "../driver/t41/tx_isp_t41_ae_alloc.h"
unsigned char oracle_bss[0x5000] __attribute__((aligned(65536)));
unsigned char oracle_rodata[0x3000] __attribute__((aligned(65536)));
unsigned int oracle_y[225];
extern unsigned int oracle_target(unsigned long long, const unsigned long long *,
	const unsigned short *, unsigned int);
extern int oracle_statistics(unsigned int, const void *);
extern void oracle_adjust(void *, unsigned long long *, unsigned short *);
extern int oracle_mean(unsigned int, unsigned int *, unsigned int *,
	unsigned int, unsigned int, unsigned int *);
extern unsigned int oracle_convergence(const unsigned int *, unsigned short *,
	unsigned short *, unsigned int, unsigned short, unsigned int);
extern unsigned int oracle_deflicker(unsigned int);
extern unsigned long long oracle_mul64(unsigned int, unsigned long long, unsigned long long);
extern unsigned long long oracle_div64_fixed(unsigned int, unsigned long long, unsigned long long);
extern unsigned int oracle_fixed_div(unsigned int, unsigned int, unsigned int);
extern void *oracle_allocate(unsigned int, unsigned long long,
	unsigned int *, unsigned int *, unsigned int *, unsigned int);
static unsigned char p[T41_AE_PARAM_BYTES], state[T41_AE_STATE_BYTES],
	scalar[T41_AE_STATE_BYTES], cache[0x688], dma[4096];
static uint32_t info[4], seed = 7894;
static unsigned int rng(void) { seed ^= seed<<13; seed ^= seed>>17; seed ^= seed<<5; return seed; }
static void put16(unsigned char *at, unsigned int v) { at[0] = v; at[1] = v>>8; }
int main(void)
{
	unsigned int f, i, fail = 0, unsafe_lattices = 0;
	*(uint32_t *)(void *)(oracle_bss+0x40b0) = (uintptr_t)info;
	*(uint32_t *)(void *)(oracle_rodata+0x1dc4) = (uintptr_t)cache;
	info[0] = (uintptr_t)p; info[1] = (uintptr_t)state;
	t41_ae_put32(cache+0x504, 1);
	for (f = 0; f < 10000; ++f) {
		unsigned long long knots[15], x, base = f&1 ? 0x100000000ULL : 10;
		unsigned short targets[15];
		unsigned long long adjusted_knots[15], scalar_knots[15];
		unsigned short adjusted_targets[15], scalar_targets[15];
		unsigned int shift = rng()%24, target, expected, rows = 1+rng()%15, cols = 1+rng()%15;
		unsigned int weights[2], fg[2], mean;
		struct t41_ae_meter meter;
		{
			struct t41_ae_allocation result;
			unsigned int precision = 1 + rng() % 16, unity = 1U << precision;
			unsigned int min_e = 1 + rng() % 10, max_e = min_e + rng() % 3000;
			unsigned int min_a = unity + rng() % unity, min_d = unity + rng() % unity;
			unsigned int max_a = min_a * (1 + rng() % 32), max_d = min_d * (1 + rng() % 16);
			unsigned int oe, oa, od, fps = 1 + rng() % 120, last = rng() % 120;
			unsigned int step = 1 + rng() % 400;
			unsigned int short_scale = precision >= 8 && step >= 120 ? 1 + rng() % 240 : fps;
			unsigned long long ev = ((unsigned long long)rng() << 32) | rng();
			unsigned char old_p[sizeof(p)], old_cache[sizeof(cache)];
			if (f % 4) ev %= ((unsigned long long)max_e * max_a * max_d >> precision) + 1;
			if (f % 11 == 0) ev = rng() % ((min_e + 1) * unity);
			memset(p, 0, sizeof(p)); memset(state, 0, sizeof(state)); memset(cache, 0, sizeof(cache));
			put16(p + 0x6c0, precision); put16(p + 0x7a0, rng() % 2);
			put16(p + 0x7a2, rng() % 3); put16(p + 0x7a4, short_scale);
			t41_ae_put32(p + 0x64c, rng() % 20);
			t41_ae_put32(cache + 0x260, min_e); t41_ae_put32(cache + 0x270, max_e);
			t41_ae_put32(cache + 0x264, min_a); t41_ae_put32(cache + 0x274, max_a);
			t41_ae_put32(cache + 0x26c, min_d); t41_ae_put32(cache + 0x27c, max_d);
			t41_ae_put32(cache + 0x4a0, f % 13 == 0 ? ~0U : rng() % 25);
			put16(state + 0x2178, rng() % 2); state[0x2612] = last;
			for (i = 0; i < 120; ++i) put16(state + 0x2438 + i * 2, (i + 1) * step);
			memcpy(old_p, p, sizeof(p)); memcpy(old_cache, cache, sizeof(cache));
			memcpy(scalar, state, sizeof(state));
			if (t41_ae_auto_allocate(p, sizeof(p), state, sizeof(state), cache,
					sizeof(cache), ev, fps, &result)) {
				/* OEM decrements an unsigned 16-bit zero to 65535 and
				 * reads beyond its node table. Never execute that path. */
				if (!t41_tmo_le16(p + 0x7a0) || last ||
						((20U + max_e) << precision) >=
						oracle_fixed_div(precision, step << precision, min_e << precision)) {
					printf("unexpected allocation rejection %u\n", f); return 2;
				}
				++unsafe_lattices;
			} else {
				oracle_allocate(0, ev, &oe, &oa, &od, fps);
				put16(scalar + 0x2178, result.settled);
				t41_ae_put32(old_cache + 0x4a0, result.saturated_frames);
				if (result.integration != oe || result.again != oa || result.dgain != od ||
						memcmp(old_p, p, sizeof(p)) || memcmp(scalar, state, sizeof(state)) ||
						memcmp(old_cache, cache, sizeof(cache))) {
					if (fail++ < 20) printf("allocation %u q=%u ev=%llu min/maxE=%u/%u "
						"gain=%u/%u/%u/%u scalar=%u/%u/%u/%u/%u OEM=%u/%u/%u/%u/%u\n",
						f, precision, ev, min_e, max_e, min_a, max_a, min_d, max_d,
						result.integration, result.again, result.dgain, result.saturated_frames, result.settled,
						oe, oa, od, t41_tmo_le32(cache + 0x4a0), t41_tmo_le16(state + 0x2178));
				}
			}
			t41_ae_put32(cache + 0x504, 1);
		}
		{
			unsigned int precision = 1 + f % 31;
			unsigned long long a = ((unsigned long long)rng() << 32) | rng();
			unsigned long long b = (((unsigned long long)rng() << 32) | rng()) | 1;
			unsigned long long product = oracle_mul64(precision, a, b);
			unsigned long long quotient = oracle_div64_fixed(precision, a, b);
			if (product != t41_ae_fixed_mul64(precision, a, b) ||
					quotient != t41_ae_fixed_div64(precision, a, b)) {
				if (fail++ < 20) printf("wide arithmetic case %u mismatch\n", f);
			}
		}
		{
			unsigned short nodes[120];
			unsigned int precision = 1 + f % 16, frequency = (f & 1) ? 50 : 60;
			unsigned int fps = (1 + rng() % 120) << precision;
			unsigned int min_fps = 1 + rng() % fps, height = 1 + rng() % 8192;
			unsigned int last, reference;
			put16(p + 0x6c0, precision); t41_ae_put32(p + 0x674, frequency);
			t41_ae_put32(cache + 0x4ec, fps); t41_ae_put32(cache + 0x4f0, min_fps);
			put16(state + 0x216a, height); memset(scalar, 0xa5, sizeof(scalar));
			memcpy(state + 0x2438, scalar + 0x2438, 240);
			reference = oracle_deflicker(0);
			if (t41_ae_deflicker(frequency, precision, fps, min_fps, height,
					nodes, &last)) return 2;
			if (last != reference || state[0x2612] != last ||
					memcmp(nodes, state + 0x2438, sizeof(nodes))) {
				if (fail++ < 20) printf("deflicker case %u mismatch last=%u OEM=%u\n",
					f, last, reference);
			}
		}
		{
			unsigned int delta[2] = {rng(), rng()}, sum, reference;
			unsigned int precision = f % 25, speed = rng(), limit = rng();
			unsigned short down = rng(), up = rng(), od = down, ou = up;
			/* Mix normal small calibrated ramps with wrap/precision edges. */
			if (f & 1) {
				precision = 10; speed &= 255; limit = 65535;
				delta[0] &= 4095; delta[1] &= 4095;
			}
			reference = oracle_convergence(delta, &od, &ou, speed, precision, limit);
			if (t41_ae_convergence_speed(delta, speed, precision, limit,
					&down, &up, &sum)) return 2;
			if (down != od || up != ou || sum != reference) {
				if (fail++ < 20) printf("convergence case %u scalar=%u/%u/%u OEM=%u/%u/%u\n",
					f, down, up, sum, od, ou, reference);
			}
		}
		for (i = 0; i < 15; ++i) { knots[i] = base; base += 1+rng()%10000000; targets[i] = rng()%65536; }
		x = (knots[0]-10 + rng()%(knots[14]-knots[0]+20)) << shift;
		x += rng() & ((1U<<shift)-1);
		expected = oracle_target(x, knots, targets, shift);
		if (t41_ae_long_target(x, knots, targets, shift, &target)) return 2;
		if (target != expected) {
			if (fail++ < 20) printf("target case %u: scalar=%u OEM=%u\n",f,target,expected);
		}
		for (i = 0; i < 15; ++i) {
			t41_ae_put32(p+0x5d0+i*8,knots[i]);
			t41_ae_put32(p+0x5d4+i*8,knots[i] >> 32);
			put16(p+0x76e + i*2,targets[i]); put16(p+0x7d4+i*2,rng());
		}
		put16(p+0x7c4,1); put16(p+0x7c6,rng()); put16(p+0x7c8,rng());
		memcpy(adjusted_knots,knots,sizeof(knots)); memcpy(adjusted_targets,targets,sizeof(targets));
		oracle_adjust(p,adjusted_knots,adjusted_targets);
		if (t41_ae_target_tables(p,sizeof(p),scalar_knots,scalar_targets)) return 2;
		if (memcmp(adjusted_knots,scalar_knots,sizeof(scalar_knots)) ||
		    memcmp(adjusted_targets,scalar_targets,sizeof(scalar_targets))) {
			if (fail++ < 20) printf("adjustment case %u mismatch\n",f);
		}
		put16(p+0x70a,rows); put16(p+0x70e,cols);
		for (i = 0; i < sizeof(dma); i += 4) t41_ae_put32(dma+i,rng());
		memset(state,0xa5,sizeof(state)); memcpy(scalar,state,sizeof(state));
		oracle_statistics(0,dma);
		if (t41_ae_statistics(dma,sizeof(dma),rows,cols,scalar,sizeof(scalar))) return 2;
		if (memcmp(scalar,state,sizeof(state))) {
			for (i = 0; i < sizeof(state); ++i) if (scalar[i] != state[i]) {
				if (fail++ < 20) printf("parser case %u byte %x scalar=%x OEM=%x\n",f,i,scalar[i],state[i]);
			}
		}
		put16(p+0x6c0, 5+rng()%12); put16(p+0x7b8, 1+rng()%16);
		put16(p+0x7b4, 1+rng()%16); put16(p+0x7b6, 1+rng()%16);
		put16(p+0x712, rng()%400); t41_ae_put32(p+0x68c,f&1);
		for (i = 0; i < 15; ++i) {
			put16(state+0x21fc+i*2, 1+rng()%200); put16(state+0x221a+i*2, 1+rng()%200);
		}
		for (i = 0; i < 225; ++i) {
			unsigned int area = t41_tmo_le16(state+0x21fc+i/15*2)*t41_tmo_le16(state+0x221a+i%15*2);
			p[0x4ee + i] = 1+rng()%7; p[0x82e + i] = 1+rng()%32;
			t41_ae_put32(state+0x800+i*4,rng()%(area*80));
			t41_ae_put32(state+0xb84+i*4,rng()%(area*80));
			t41_ae_put32(state+0xf08+i*4,rng()%(area*80));
			t41_ae_put32(state+0x128c+i*4,rng()%area);
			t41_ae_put32(state+0x1610+i*4,rng()%area);
		}
		for (i = 0; i < 256; ++i) t41_ae_put32(state+i*4,rng()%8000);
		oracle_mean(0,&mean,weights,f&1 ? t41_tmo_le16(p+0x7b4) : 1,
			f&1 ? t41_tmo_le16(p+0x7b6) : 1,fg);
		if (t41_ae_weight_mean(p,sizeof(p),state,sizeof(state),&meter)) return 2;
		if (mean != meter.mean || fg[0] != meter.foreground || fg[1] != meter.background ||
		    weights[0] != meter.bright_q || weights[1] != meter.dark_q) {
			if (fail++ < 20) printf("meter case %u scalar=%u/%u/%u/%u/%u OEM=%u/%u/%u/%u/%u\n",f,
				meter.mean,meter.foreground,meter.background,meter.bright_q,meter.dark_q,
				mean,fg[0],fg[1],weights[0],weights[1]);
		}
	}
	printf("10000 synthetic AE automatic allocations, wide arithmetic, deflicker lattices, convergence ramps, targets, DMA pages and weighted means: %u mismatches\n",fail);
	printf("Rejected %u unsafe OEM lattice-index underflows without executing them\n", unsafe_lattices);
	return fail ? 1 : 0;
}
