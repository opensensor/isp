/* SPDX-License-Identifier: MIT */
#ifndef TX_ISP_T41_AWB_CONTROL_H
#define TX_ISP_T41_AWB_CONTROL_H
#include "tx_isp_t41_awb_gray.h"
#include "tx_isp_t41_awb_gain.h"
#include "tx_isp_t41_awb.h"

#define T41_AWB_PARAM_BYTES 0x12e4U
#define T41_AWB_REPORT_BYTES 0x41aU
#define T41_AWB_ARRAY_BYTES 0x1700U

/* Private algorithm state, never a view into the installed sensor binary.
 * The runtime owns serialization and commits a candidate only on success. */
struct t41_awb_owned {
	unsigned char p[T41_AWB_PARAM_BYTES];
	unsigned char s[T41_AWB_STATE_BYTES] __attribute__((aligned(4)));
	unsigned int control[7];
	unsigned short red[6], blue[6];
	unsigned char report[T41_AWB_REPORT_BYTES];
	/* Public attribute policy: OEM tisp_tattr +0x910, +0x2d4, +0x308.
	 * Keep this API bookkeeping with the algorithm owner, not a second lock. */
	unsigned char public_policy[44];
	unsigned int public_freeze, public_start;
};

/* H20250310a params_refresh, ELF .text+0x2f5b8. Bayer zone dimensions
 * are half-image units. Refuse underflow, empty zones and hardware truncation.
 * Unused entries remain calibrated; OEM does not clear the 15-entry arrays. */
static inline int t41_awb_params_refresh_owned(struct t41_awb_owned *o)
{
	unsigned int i, rows, cols, x, y, width, height, dx, dy;
	if (!o) return -1;
	rows=t41_tmo_le16(o->p+0xc6e); cols=t41_tmo_le16(o->p+0xc72);
	x=t41_tmo_le16(o->p+0xc70); y=t41_tmo_le16(o->p+0xc6c);
	width=t41_tmo_le16(o->s+0x34ce); height=t41_tmo_le16(o->s+0x34d0);
	if (!rows || rows>15 || !cols || cols>15 || x>4095 || y>4095 ||
	    x>width || y>height) return -1;
	dx=((width-x+1)/2)/cols; dy=((height-y+1)/2)/rows;
	if (!dx || dx>255 || !dy || dy>255) return -1;
	if (!o->s[0xc5f9] && !o->s[0xeaa0]) {
		t41_ae_put32(o->p+4,1); t41_ae_put32(o->p+8,0);
	}
	o->s[0xc5f9]=0;
	for (i=0;i<cols;++i) t41_awb_gain_put16(o->p+0xc74+i*2,dx);
	for (i=0;i<rows;++i) t41_awb_gain_put16(o->p+0xc92+i*2,dy);
	return 0;
}

static inline void t41_awb_show_owned(struct t41_awb_owned *o)
{
	unsigned int i;
	for (i=0;i<120;++i) o->report[i]=o->p[0xe9a+i];
}

/* init, ELF 0x30080: state defaults are algorithm constants. The universal
 * distance kernel is generated mathematically, not copied from OEM data. */
static inline int t41_awb_cold(struct t41_awb_owned *o, const unsigned char *p,
		unsigned int bytes, unsigned int width, unsigned int height)
{
	unsigned int i;
	unsigned char *b=(unsigned char *)o;
	if (!o || !p || bytes<T41_AWB_PARAM_BYTES || !width || width>65535 ||
	    !height || height>65535) return -1;
	for (i=0;i<sizeof(*o);++i) b[i]=0;
	for (i=0;i<T41_AWB_PARAM_BYTES;++i) o->p[i]=p[i];
	for (i=0;i<225;++i) t41_ae_put32(o->s+0x3138+i*4,1);
	t41_ae_put32(o->s+0x34c0,512); t41_ae_put32(o->s+0x34c8,512);
	t41_awb_gain_put16(o->s+0x34ce,width); t41_awb_gain_put16(o->s+0x34d0,height);
	o->s[0x3513]=1;
	t41_awb_gain_put16(o->s+0x3516,8); t41_awb_gain_put16(o->s+0x3518,8);
	if (t41_awb_distance_lut_init(o->s+0x3520,1028)) return -1;
	t41_ae_put32(o->s+0x3924,1024); t41_ae_put32(o->s+0x3928,1024);
	for (i=0;i<15;++i) {
		t41_ae_put32(o->s+0xc604+i*4,1024); t41_ae_put32(o->s+0xc640+i*4,1024);
		t41_ae_put32(o->s+0xc67c+i*4,5000);
	}
	t41_ae_put32(o->s+0xea98,400); t41_ae_put32(o->s+0xea9c,1024);
	t41_ae_put32(o->s+0xeaa4,5000);
	for (i=0;i<12;++i) t41_awb_gain_put16(o->s+0xeaa8+i*2,256);
	if (t41_awb_params_refresh_owned(o)) return -1;
	t41_awb_show_owned(o);
	return 0;
}

/* EV Y weights, ELF 0x32824. Preserve the OEM low-u32 multiply before
 * division, including descending weight curves. Reject unordered EV knots. */
static inline int t41_awb_yweight_owned(struct t41_awb_owned *o)
{
	unsigned int i, ev, value, lo, hi, a, b, delta;
	if (!o) return -1;
	for (i=1;i<9;++i)
		if (t41_tmo_le32(o->p+0xfc+i*4)<=t41_tmo_le32(o->p+0xf8+i*4)) return -1;
	ev=t41_tmo_le32(o->s+0xea98);
	value=t41_tmo_le16(o->p+0xd32);
	if (ev>=t41_tmo_le32(o->p+0x11c)) value=t41_tmo_le16(o->p+0xd42);
	else if (ev>t41_tmo_le32(o->p+0xfc)) {
		for (i=0;i<8;++i) {
			lo=t41_tmo_le32(o->p+0xfc+i*4); hi=t41_tmo_le32(o->p+0x100+i*4);
			if (ev<=lo || ev>hi) continue;
			a=t41_tmo_le16(o->p+0xd32+i*2); b=t41_tmo_le16(o->p+0xd34+i*2);
			delta=((ev-lo)*(a>b ? a-b : b-a))/(hi-lo);
			value=a>b ? a-delta : a+delta;
			break;
		}
	}
	for (i=0;i<4;++i) t41_awb_gain_put16(o->s+0x3514+i*2,value);
	return 0;
}

enum t41_awb_control_op {
	T41_AWB_MODE, T41_AWB_FREEZE, T41_AWB_CT, T41_AWB_WEIGHT,
	T41_AWB_LOCATION, T41_AWB_TREND, T41_AWB_CONVERGE,
	T41_AWB_EV, T41_AWB_PARAMS, T41_AWB_ATTR, T41_AWB_GLOBAL, T41_AWB_ZONE,
	T41_AWB_PUBLIC
};

static inline unsigned int t41_awb_control_bytes(enum t41_awb_control_op op, int get)
{
	switch (op) {
	case T41_AWB_MODE: return get ? 28 : 12;
	case T41_AWB_FREEZE: return 1;
	case T41_AWB_CT: case T41_AWB_EV: return 4;
	case T41_AWB_WEIGHT: return 225;
	case T41_AWB_LOCATION: return 10;
	case T41_AWB_TREND: return 28;
	case T41_AWB_CONVERGE: return 20;
	case T41_AWB_PARAMS: return get ? T41_AWB_ARRAY_BYTES : T41_AWB_PARAM_BYTES;
	case T41_AWB_ATTR: case T41_AWB_PUBLIC: return 76;
	case T41_AWB_GLOBAL: return 16;
	case T41_AWB_ZONE: return 2700;
	default: return 0;
	}
}

/* Setters operate on a private candidate. PARAMS/EV can fail validation after
 * modifying it; callers must discard that candidate, not publish it. */
static inline int t41_awb_control_set(struct t41_awb_owned *o,
		enum t41_awb_control_op op, const unsigned char *in, unsigned int bytes)
{
	unsigned int i, n=t41_awb_control_bytes(op,0), mode, r, b;
	static const unsigned short presets[7][2]={
		/* OEM public manual illuminant modes 2..8, ELF 0x3327c..0x33320.
		 * These are API defaults, not a sensor-specific auto-WB estimate. */
		{384,384},{438,303},{219,690},{240,564},{315,459},{468,279},{240,376}
	};
	if (!o || !in || !n || bytes<n) return -1;
	switch (op) {
	case T41_AWB_MODE:
		mode=t41_tmo_le32(in);
		if (mode>9) return -1;
		r=o->control[1]; b=o->control[2];
		if (mode==1 || mode==9) { r=t41_tmo_le32(in+4); b=t41_tmo_le32(in+8); }
		else if (mode>=2) { r=presets[mode-2][0]; b=presets[mode-2][1]; }
		o->control[0]=mode; o->control[1]=r; o->control[2]=b;
		o->s[0xeaa2]=mode; o->s[0xeaa0]=1;
		t41_awb_gain_put16(o->s+0xeaa8,r); t41_awb_gain_put16(o->s+0xeaaa,b);
		break;
	case T41_AWB_FREEZE: o->s[0xeaa1]=in[0]; break;
	case T41_AWB_CT: t41_ae_put32(o->s+0xeaa4,t41_tmo_le32(in)); break;
	case T41_AWB_WEIGHT:
		for (i=0;i<225;++i) o->p[0x1200+i]=in[i];
		o->s[0xc5f8]=0; break;
	case T41_AWB_LOCATION:
		if (!in[8] || in[8]>15 || !in[9] || in[9]>15 ||
		    t41_tmo_le32(in)>4095 || t41_tmo_le32(in+4)>4095) return -1;
		t41_awb_gain_put16(o->p+0xc6c,t41_tmo_le32(in+4));
		t41_awb_gain_put16(o->p+0xc6e,in[9]);
		t41_awb_gain_put16(o->p+0xc70,t41_tmo_le32(in));
		t41_awb_gain_put16(o->p+0xc72,in[8]); o->s[0xc5f8]=0; break;
	case T41_AWB_TREND:
		for (i=0;i<24;++i) o->p[0xbf4+i]=in[4+i];
		t41_ae_put32(o->p+0xc0c,t41_tmo_le16(in));
		t41_ae_put32(o->p+0xc10,t41_tmo_le16(in+2)); o->s[0xeaa0]=1; break;
	case T41_AWB_CONVERGE:
		if (!t41_tmo_le32(in) || t41_tmo_le32(in)>15) return -1;
		t41_awb_gain_put16(o->p+0xcd6,t41_tmo_le32(in));
		for (i=0;i<16;++i) o->p[0xc+i]=in[4+i];
		break;
	case T41_AWB_EV:
		t41_ae_put32(o->s+0xea98,t41_tmo_le32(in)>>10);
		return t41_awb_yweight_owned(o);
	case T41_AWB_PARAMS:
		o->s[0xeaa0]=1;
		for (i=0;i<T41_AWB_PARAM_BYTES;++i) o->p[i]=in[i];
		if (t41_awb_params_refresh_owned(o) || t41_awb_yweight_owned(o)) return -1;
		t41_awb_show_owned(o); break;
	default: return -1;
	}
	return 0;
}

static inline int t41_awb_control_get(struct t41_awb_owned *o,
		enum t41_awb_control_op op, unsigned char *out, unsigned int bytes)
{
	unsigned int i, n=t41_awb_control_bytes(op,1), mode, at;
	if (!o || !out || !n || bytes<n) return -1;
	switch (op) {
	case T41_AWB_MODE:
		o->control[0]=o->s[0xeaa2];
		for (i=0;i<2;++i) {
			o->control[1+i]=t41_tmo_le16(o->s+0xeaa8+i*2);
			o->control[3+i]=t41_tmo_le16(o->s+0xeaac+i*2);
			o->control[5+i]=t41_tmo_le16(o->s+0xeab8+i*2);
		}
		for (i=0;i<7;++i) t41_ae_put32(out+i*4,o->control[i]);
		break;
	case T41_AWB_FREEZE: out[0]=o->s[0xeaa1]; break;
	case T41_AWB_CT: t41_ae_put32(out,t41_tmo_le32(o->s+0xeaa4)); break;
	case T41_AWB_WEIGHT: for (i=0;i<n;++i) out[i]=o->p[0x1200+i]; break;
	case T41_AWB_TREND:
		for (i=0;i<24;++i) out[4+i]=o->p[0xbf4+i];
		t41_awb_gain_put16(out,t41_tmo_le32(o->p+0xc0c));
		t41_awb_gain_put16(out+2,t41_tmo_le32(o->p+0xc10)); break;
	case T41_AWB_CONVERGE:
		t41_ae_put32(out,t41_tmo_le16(o->p+0xcd6));
		for (i=0;i<16;++i) out[4+i]=o->p[0xc+i];
		break;
	case T41_AWB_PARAMS:
		for (i=0;i<T41_AWB_PARAM_BYTES;++i) out[i]=o->p[i];
		for (i=0;i<T41_AWB_REPORT_BYTES;++i) out[T41_AWB_PARAM_BYTES+i]=o->report[i];
		/* OEM copies two uninitialized padding bytes. Never expose them. */
		out[n-2]=out[n-1]=0; break;
	case T41_AWB_ATTR:
		for (i=0;i<n;++i) out[i]=0;
		mode=o->s[0xeaa2]; at=mode==0 || mode==9 ? 0xeab8 : 0xeaa8;
		t41_ae_put32(out,mode==9 ? 0 : mode);
		for (i=0;i<2;++i) t41_ae_put32(out+4+i*4,t41_tmo_le16(o->s+at+i*2));
		t41_ae_put32(out+16,t41_tmo_le32(o->p+0x28));
		t41_ae_put32(out+20,mode==9); t41_ae_put32(out+64,1);
		for (i=0;i<2;++i) t41_ae_put32(out+68+i*4,t41_tmo_le32(o->p+0x14+i*4));
		break;
	case T41_AWB_GLOBAL:
		for (i=0;i<4;++i) t41_ae_put32(out+i*4,t41_tmo_le16(o->s+(i<2 ? 0xeab8+i*2 : 0xeaac+(i-2)*2)));
		break;
	case T41_AWB_ZONE: for (i=0;i<n;++i) out[i]=o->s[0xeac0+i]; break;
	default: return -1;
	}
	return 0;
}

static inline int t41_awb_daynight_owned(struct t41_awb_owned *o)
{
	if (!o) return -1;
	o->s[0xc5f8]=0; o->s[0xc5f9]=1;
	return t41_awb_params_refresh_owned(o);
}

/* OEM tisp_s_awb_attr (ELF 0x6d798): mode, optional start-gain publication,
 * then freeze/CT and opaque policy bookkeeping. The caller runs gain_prepare
 * BETWEEN begin and finish when start=1. It must discard the whole candidate
 * if gain validation fails; no half-applied control becomes visible. */
static inline int t41_awb_public_begin(struct t41_awb_owned *o,
		const unsigned char *in, unsigned int bytes)
{
	unsigned int i;
	if (!o || !in || bytes<76 || t41_tmo_le32(in+12)>1 ||
	    t41_tmo_le32(in+64)>1) return -1;
	if (t41_awb_control_set(o,T41_AWB_MODE,in,12)) return -1;
	if (t41_tmo_le32(in+64))
		for (i=0;i<4;++i) t41_ae_put32(o->p+0xc+i*4,t41_tmo_le32(in+68+(i&1)*4));
	return 0;
}
static inline void t41_awb_public_finish(struct t41_awb_owned *o,const unsigned char in[76])
{
	unsigned int i;
	o->public_freeze=t41_tmo_le32(in+12); o->public_start=t41_tmo_le32(in+64);
	o->s[0xeaa1]=o->public_freeze;
	if (o->public_freeze) t41_ae_put32(o->s+0xeaa4,t41_tmo_le32(in+16));
	for (i=0;i<44;++i) o->public_policy[i]=in[20+i];
}
static inline int t41_awb_public_get(struct t41_awb_owned *o,unsigned char *out,unsigned int bytes)
{
	unsigned int i;
	if (t41_awb_control_get(o,T41_AWB_ATTR,out,bytes)) return -1;
	for (i=0;i<44;++i) out[20+i]=o->public_policy[i];
	t41_ae_put32(out+12,o->public_freeze); t41_ae_put32(out+64,o->public_start);
	return 0;
}
#endif
