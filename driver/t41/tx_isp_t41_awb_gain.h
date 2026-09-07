/* SPDX-License-Identifier: MIT */
#ifndef TX_ISP_T41_AWB_GAIN_H
#define TX_ISP_T41_AWB_GAIN_H
#include "tx_isp_t41_ae.h"

#define T41_AWB_GAIN_PARAM_BYTES 0xcd6U
#define T41_AWB_GAIN_STATE_BYTES 0xeac0U
#define T41_AWB_GAIN_REPORT_BYTES 0x96U

static inline void t41_awb_gain_put16(unsigned char *p, unsigned int v)
{
	p[0] = v; p[1] = v >> 8;
}

static inline void t41_awb_gain_pack(const unsigned int gains[2], unsigned int words[2])
{
	unsigned int i;
	for (i = 0; i < 2; ++i)
		words[i] = 0x04000000U | (gains[i] < 0x4000 ? gains[i] : 0x3fff);
}

/* H20250310a gain conversion and CT-offset history, not the AWB illuminant
 * estimator. Inputs are the active calibration, owned runtime and report.
 * Validate all divisors/lengths/modes before changing any caller state.
 * Returned words go to both WB banks; freeze suppresses MMIO, not history.
 */
static inline int t41_awb_gain_prepare(unsigned char *p, unsigned int pbytes,
		unsigned char *s, unsigned int sbytes, unsigned char *report,
		unsigned int rbytes, unsigned int words[2], unsigned int *write_enable)
{
	unsigned int precision, shift, i, gains[2], reciprocal[2], offsets[2];
	unsigned int ct, last_ct, delta, mode, refresh, at;
	if (!p || !s || !report || !words || !write_enable ||
	    pbytes < T41_AWB_GAIN_PARAM_BYTES || sbytes < T41_AWB_GAIN_STATE_BYTES ||
	    rbytes < T41_AWB_GAIN_REPORT_BYTES)
		return -1;
	precision = t41_tmo_le16(p + 0xcd2);
	shift = t41_tmo_le16(p + 0xcd4);
	mode = s[0xeaa2];
	if (precision < 10 || precision > 31 || !shift || shift > 31 || mode > 9)
		return -1;
	for (i = 0; i < 2; ++i) {
		gains[i] = t41_ae_fixed_mul(precision,
			t41_tmo_le32(p + 0x14 + i*4) << shift,
			t41_tmo_le32(p + 0x30 + i*4) << (precision - 10));
		gains[i] = (gains[i] + (1U << (shift - 1))) >> shift;
		if (!gains[i])
			return -1;
		reciprocal[i] = (65536U / gains[i]) & 0xffff;
	}
	ct = t41_tmo_le32(p + 0x28);
	last_ct = t41_tmo_le32(s + 0x351c);
	if (mode >= 1 && mode <= 8) {
		for (i = 0; i < 2; ++i)
			gains[i] = t41_tmo_le16(s + 0xeaa8 + i*2);
		ct = t41_tmo_le32(s + 0xeaa4);
	} else if (mode == 9) {
		for (i = 0; i < 2; ++i)
			gains[i] = ((t41_tmo_le16(s + 0xeaa8 + i*2) + 32) * gains[i]) >> 6;
	}
	delta = ct > last_ct ? ct - last_ct : last_ct - ct;
	refresh = delta >= 200 || s[0xeaa0] == 1;
	at = ct >= t41_tmo_le32(p + 0xc10) ? 0xbf4 :
		ct > t41_tmo_le32(p + 0xc0c) ? 0xbfc : 0xc04;
	for (i = 0; i < 2; ++i) {
		offsets[i] = t41_tmo_le32(refresh ? p + at + i*4 : s + 0x3924 + i*4);
		gains[i] = (gains[i] << 2) - 1024U + offsets[i];
	}
	/* Commit only after all failure paths. Reports describe the unoverridden
	 * calibrated result, just as OEM; manual CT is stored afterward. */
	for (i = 0; i < 2; ++i) {
		t41_awb_gain_put16(s + 0xeabc + i*2, t41_tmo_le16(s + 0xeab8 + i*2));
		t41_awb_gain_put16(s + 0xeab8 + i*2, reciprocal[i]);
		t41_awb_gain_put16(report + 0x90 + i*2, reciprocal[i] << 2);
		if (refresh)
			t41_ae_put32(s + 0x3924 + i*4, offsets[i]);
	}
	t41_awb_gain_put16(report + 0x94, t41_tmo_le32(p + 0x28));
	t41_ae_put32(p + 0x28, ct);
	if (refresh)
		t41_ae_put32(s + 0x351c, ct);
	s[0xeaa0] = 0;
	t41_awb_gain_pack(gains, words);
	*write_enable = s[0xeaa1] == 0;
	return 0;
}
#endif
