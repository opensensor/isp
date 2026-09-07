/* SPDX-License-Identifier: MIT */
#ifndef TX_ISP_T41_DPC_H
#define TX_ISP_T41_DPC_H
#include "tx_isp_t41_tmo.h"
#include "../include/tx_isp/tx_isp_math.h"

#define T41_DPC_PARAM_BYTES 0x5a2U
#define T41_DPC_STATE_BYTES 0x6eU
#define T41_DPC_LONG_WRITES 28U
#define T41_DPC_SHORT_WRITES 19U
#define T41_DPC_OTHER_WRITES 71U
struct t41_dpc_word { unsigned int address, value; };

static inline void t41_dpc_put16(unsigned char *p, unsigned int v)
{ p[0] = v; p[1] = v >> 8; }

static inline unsigned int t41_dpc_interpolate(const unsigned char *p,
		unsigned int gain, unsigned int width)
{
	unsigned int index = gain >> 16, a, b;
	if (index >= 10) index = 10;
	a = width == 2 ? t41_tmo_le16(p + index * 2) : p[index];
	if (index == 10) return a;
	b = width == 2 ? t41_tmo_le16(p + (index + 1) * 2) : p[index + 1];
	return tx_isp_lerp_pair_u32(a, b, gain & 65535);
}

/* H20250310a tisp_dpc_gain_interp. Eleven gain knots per field; the
 * sensor supplies every threshold/mask. Preserve padding and the other bank. */
static inline int t41_dpc_interpolate_bank(const unsigned char *p, unsigned int bytes,
		unsigned char *state, unsigned int state_bytes, unsigned int gain,
		unsigned int short_bank)
{
	unsigned int i, source = short_bank ? 8 : 0x31e, target = short_bank ? 0x26 : 0;
	if (!p || !state || bytes < T41_DPC_PARAM_BYTES ||
	    state_bytes < T41_DPC_STATE_BYTES || short_bank > 1)
		return -1;
	for (i = 0; i < 18; ++i)
		t41_dpc_put16(state + target + (i < 2 ? i : i + 1) * 2,
			t41_dpc_interpolate(p + source + i * 22, gain, 2));
	state[target + 4] = t41_dpc_interpolate(p + (short_bank ? 0x2e5 : 0x4c0), gain, 1);
	if (short_bank) return 0;
	for (i = 0; i < 4; ++i)
		state[0x4e + i] = t41_dpc_interpolate(p + 0x4f2 + i * 11, gain, 1);
	for (i = 0; i < 8; ++i)
		t41_dpc_put16(state + 0x52 + i * 2,
			t41_dpc_interpolate(p + 0x19c + i * 22, gain, 2));
	for (i = 0; i < 12; ++i)
		state[0x62 + i] = t41_dpc_interpolate(p + 0x51e + i * 11, gain, 1);
	return 0;
}

static inline unsigned int t41_dpc_neighbors(unsigned int mode)
{
	unsigned int a = mode >= 1, b = mode >= 2, c = mode >= 3, d = mode >= 4;
	unsigned int value = a | (b << 1) | (c << 2) | (d << 3) |
		(b << 4) | (c << 5) | (d << 6) | (c << 8) | (d << 9) | (d << 12);
	return value | (value << 16);
}

/* Pack both reference and black-subtracted thresholds, including the
 * exceptional reversed second pair and unaligned word loads. Underflow is
 * the OEM's unsigned u32 arithmetic, not a signed left-shift in C. */
static inline int t41_dpc_pack_bank(const unsigned char *p, unsigned int bytes,
		const unsigned char *s, unsigned int state_bytes, unsigned int short_bank,
		struct t41_dpc_word *out, unsigned int capacity)
{
	unsigned int i, n = 0, base = short_bank ? 0x1d000 : 0x7000;
	unsigned int offset = short_bank ? 0x26 : 0;
	unsigned int black = short_bank ? 2 : 0x318;
	unsigned int tail = short_bank ? 0x2f0 : 0x4cb;
	if (!p || !s || !out || bytes < T41_DPC_PARAM_BYTES ||
	    state_bytes < T41_DPC_STATE_BYTES || short_bank > 1 ||
	    capacity < (short_bank ? T41_DPC_SHORT_WRITES : T41_DPC_LONG_WRITES))
		return -1;
#define DPC_EMIT(a, v) do { out[n].address = (a); out[n++].value = (v); } while (0)
	for (i = 0; i < 7; ++i) {
		unsigned int pair = offset + (i ? 2 + i * 4 : 0);
		unsigned int lo = t41_tmo_le16(s + pair + (i == 1 ? 2 : 0));
		unsigned int hi = t41_tmo_le16(s + pair + (i == 1 ? 0 : 2));
		unsigned int subtract = t41_tmo_le16(p + black + (i ? i == 6 ? 4 : 2 : 0));
		DPC_EMIT(base + 0x38 + i * 4, lo | (hi << 16));
		DPC_EMIT(base + 0x1c + i * 4, (lo - subtract) | ((hi - subtract) << 16));
		if (!i) DPC_EMIT(base + 0xc, t41_dpc_neighbors(s[offset + 4]));
	}
	DPC_EMIT(base + 0x54, p[tail] | (t41_tmo_le16(s + offset + 0x1e) << 16));
	DPC_EMIT(base + 0x58, p[tail + 1] | ((unsigned int)p[tail + 2] << 8) |
		(t41_tmo_le16(s + offset + 0x20) << 16));
	DPC_EMIT(base + 0x60, t41_tmo_le32(s + offset + 0x22));
	DPC_EMIT(base + 0x9c, 1);
	if (!short_bank) {
		DPC_EMIT(0x71a0, t41_tmo_le32(s + 0x4e));
		for (i = 0; i < 3; ++i) DPC_EMIT(0x7234 + i * 4, t41_tmo_le32(s + 0x62 + i * 4));
		for (i = 0; i < 4; ++i) {
			unsigned int mask = i < 2 ? 4095 : 1023;
			DPC_EMIT(0x71b4 + i * 4, (t41_tmo_le16(s + 0x52 + i * 4) & mask) |
				((t41_tmo_le16(s + 0x54 + i * 4) & mask) << 16));
		}
		DPC_EMIT(base + 0x9c, 1);
	}
#undef DPC_EMIT
	return n;
}

/* Static control fields and the IR extension of tisp_dpc_write_reg_other.
 * These are calibration offsets and hardware masks, not captured values. */
static inline int t41_dpc_pack_other(const unsigned char *p, unsigned int bytes,
		unsigned int infrared, struct t41_dpc_word *out, unsigned int capacity)
{
	unsigned int i, bank, n=0;
	static const unsigned short offsets[2][22] = {
		{0x4be,0x4bf,0x4bc, 0x4b9,0x4ba,0x4bd,0x4bb,
		 0x4ab,0x4ac,0x4aa,0x4b0, 0x4b5,0x4b6,0x4b4,0x4b7,0x4b8,
		 0x4b2,0x4b3,0x4ae, 0x4b1,0x4af,0x4ce},
		{0x2d1,0x2d2,0x2cf, 0x2e2,0x2e3,0x2d0,0x2e4,
		 0x2d4,0x2d5,0x2d3,0x2d9, 0x2de,0x2df,0x2dd,0x2e0,0x2e1,
		 0x2db,0x2dc,0x2d7, 0x2da,0x2d8,0x2f3}
	};
	if (!p || !out || bytes < T41_DPC_PARAM_BYTES || infrared > 1 ||
	    capacity < T41_DPC_OTHER_WRITES) return -1;
#define DPC_EMIT(a, v) do { out[n].address = (a); out[n++].value = (v); } while (0)
#define F(i) ((unsigned int)p[o[i]])
	for(bank=0;bank<2;++bank) {
		const unsigned short *o=offsets[bank];
		unsigned int base=bank ? 0x1d000 : 0x7000;
		unsigned int tail=bank ? 0x2f7 : 0x4d2;
		DPC_EMIT(base, (F(0)<<8)|(F(1)<<12)|F(2)|(t41_tmo_le16(p)<<16));
		DPC_EMIT(base+4, (F(3)<<8)|(F(4)<<16)|F(5)|(F(6)<<24));
		DPC_EMIT(base+8, (F(7)<<8)|(F(8)<<16)|F(9)|(F(10)<<24));
		DPC_EMIT(base+0x10, (F(11)<<8)|(F(12)<<16)|F(13)|(F(14)<<20)|(F(15)<<24));
		DPC_EMIT(base+0x14, (F(16)<<16)|(F(17)<<24)|F(18));
		DPC_EMIT(base+0x18, (F(19)<<16)|F(20));
		DPC_EMIT(base+0x5c, (unsigned int)p[o[21]+1]<<8 | (unsigned int)p[o[21]+2]<<16 | F(21));
		for(i=0;i<8;++i) DPC_EMIT(base+0x64+i*4,t41_tmo_le32(p+tail+i*4));
		DPC_EMIT(base+0x9c,1);
	}
#undef F
	DPC_EMIT(0x7084,p[0x2bd]&7);
	DPC_EMIT(0x1d084,p[0x2be]&7);
	DPC_EMIT(0x71a4,(p[0x2bf]&3) | (p[0x2c0]&3)<<8 | (p[0x2c1]&3)<<16 | (p[0x2c2]&3)<<24);
	DPC_EMIT(0x71a8,(p[0x2bc]&1) | (p[0x2c3]&1)<<4 | (p[0x2c4]&1)<<8 | (p[0x2c5]&1)<<12 | (p[0x2c6]&1)<<16);
	DPC_EMIT(0x71ac,(t41_tmo_le16(p+0x194)&4095) | (t41_tmo_le16(p+0x196)&4095)<<16);
	DPC_EMIT(0x71b0,(t41_tmo_le16(p+0x194)&4095) * 0x10001U);
	for(i=0;i<(infrared ? 28U : 21U);++i)
		DPC_EMIT(0x71c4+i*4,(t41_tmo_le16(p+0x24c+i*4)&2047) | (t41_tmo_le16(p+0x24e + i*4)&2047)<<16);
	DPC_EMIT(0x709c,1);
	if(infrared) {
		DPC_EMIT(0x1d300,0x4ffffff);
		DPC_EMIT(0x1d304,0x4ffff);
	}
	DPC_EMIT(0x1d308,infrared ? p[0x2c7]&1 : 1);
	DPC_EMIT(0x1d09c,1);
#undef DPC_EMIT
	return n;
}
#endif
