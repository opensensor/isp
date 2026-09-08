/* SPDX-License-Identifier: MIT */
#ifndef TX_ISP_T41_AWB_SPECIAL_H
#define TX_ISP_T41_AWB_SPECIAL_H
#include "tx_isp_t41_awb_stats.h"
#include "tx_isp_t41_awb_gain.h"

struct t41_awb_register { unsigned int address, value; };

/* Six special-illuminant regions: region zero follows measured RGB after
 * its two-frame warmup; regions 5..9 use calibrated bounds. Mode 1 programs
 * hardware, other nonzero modes produce calibrated software ratios. Packed
 * region pairs share registers, so keep OEM whole-word writes and ordering.
 * rgb is a caller-owned snapshot of 0x180ac/b0/b4, not captured tuning data. */
static inline int t41_awb_special_prepare(unsigned char *p, unsigned int pb,
		unsigned char *s, unsigned int sb, const unsigned int rgb[3],
		struct t41_awb_register out[18], unsigned int *writes)
{
	unsigned int j, n=0;
	if (!p || pb<0x1200 || !s || sb<T41_AWB_STATE_BYTES || !rgb || !out || !writes)
		return -1;
	for (j=0;j<10;++j) s[0x34e0+j]=p[0x11f6+j];
	if (s[0x34de]==1 && s[0x34df]<2) ++s[0x34df];
	for (j=0;j<6;++j) {
		unsigned int slot=j ? j+4 : 0, mode=p[0x11f6+slot];
		unsigned int control=t41_tmo_le32(p+0x140+slot*4);
		unsigned int red=0, blue=0;
		if (!j && mode) {
			if (s[0x34de]!=1 || s[0x34df]<2) {
				s[0x34e0]=0; mode=0;
			} else {
				if (rgb[1]) { red=(rgb[0]<<8)/rgb[1]; blue=(rgb[2]<<8)/rgb[1]; }
				t41_awb_gain_put16(p+0xd6c,red); t41_awb_gain_put16(p+0xd80,blue);
			}
		} else if (j) {
			red=t41_tmo_le16(p+0xd6c+slot*2);
			blue=t41_tmo_le16(p+0xd80+slot*2);
		}
		if (mode==1) {
			unsigned int pair=slot/2;
			out[n].address=0x18054+pair*4; out[n++].value=t41_tmo_le32(p+0xd6c+pair*4);
			out[n].address=0x18068+pair*4; out[n++].value=t41_tmo_le32(p+0xd80+pair*4);
			control|=(unsigned int)s[0x34e0+slot]<<16;
		} else if (mode) {
			t41_awb_gain_put16(s+0x34ea+slot*2,
				(t41_ae_fixed_mul(10,red<<2,t41_tmo_le32(p+0x30))+2)>>2);
			t41_awb_gain_put16(s+0x34fe + slot*2,
				(t41_ae_fixed_mul(10,blue<<2,t41_tmo_le32(p+0x34))+2)>>2);
		}
		out[n].address=0x1807c+slot*4; out[n++].value=control;
	}
	*writes=n;
	return 0;
}
#endif
