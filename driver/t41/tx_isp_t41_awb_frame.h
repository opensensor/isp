/* SPDX-License-Identifier: MIT */
#ifndef TX_ISP_T41_AWB_FRAME_H
#define TX_ISP_T41_AWB_FRAME_H
#include "tx_isp_t41_awb_control.h"
#include "tx_isp_t41_awb_detect.h"
#include "tx_isp_t41_awb_long.h"
#include "tx_isp_t41_awb_prior.h"
#include "tx_isp_t41_awb_special.h"

#define T41_AWB_FRAME_WORDS 28U
#define T41_AWB_HARDWARE_WORDS 17U
struct t41_awb_frame_buffers {
	unsigned char *p, *s, *report;
	unsigned int pbytes, sbytes, rbytes;
};
static inline int t41_awb_frame_detect(void *context, void **view,
		unsigned int *out, unsigned int *failed)
{
	struct t41_awb_frame_buffers *b=context;
	return t41_awb_detect(b->p,b->pbytes,b->s,b->sbytes,view[22],view[23],out,failed);
}
static inline int t41_awb_gain_words(struct t41_awb_frame_buffers *b,
		struct t41_awb_register *words, unsigned int *count)
{
	static const unsigned int addresses[10]={
		0x4004,0x4008,0x400c,0x4010,0x4000,0x5004,0x5008,0x500c,0x5010,0x5000
	};
	unsigned int i, wb[2], enable;
	if (t41_awb_gain_prepare(b->p,b->pbytes,b->s,b->sbytes,b->report,b->rbytes,wb,&enable)) return -1;
	*count=enable ? 10 : 0;
	for (i=0;i<*count;++i) {
		words[i].address=addresses[i];
		words[i].value=i==4 || i==9 ? 1 : wb[i==1 || i==3 || i==6 || i==8];
	}
	return 0;
}

/* No MMIO and no allocation. Caller serializes a private candidate, then
 * commits all words and state only if this complete frame succeeds. */
static inline int t41_awb_frame(struct t41_awb_frame_buffers *b,
		unsigned short red[6], unsigned short blue[6],
		const unsigned char *dma, unsigned int bytes, const unsigned int rgb[3],
		struct t41_awb_register words[T41_AWB_FRAME_WORDS], unsigned int *count)
{
	void *view[45];
	unsigned int n, gains;
	if (!b || !words || !count || !red || !blue) return -1;
	if (t41_awb_statistics(b->p,b->pbytes,dma,bytes,b->s,b->sbytes) ||
	    t41_awb_prior_prepare(b->p,b->pbytes,b->s,b->sbytes,view,red,blue) ||
	    t41_awb_special_prepare(b->p,b->pbytes,b->s,b->sbytes,rgb,words,&n) ||
	    t41_awb_long(b->p,b->pbytes,b->s,b->sbytes,b->report,b->rbytes,view,t41_awb_frame_detect,b) ||
	    t41_awb_gain_words(b,words+n,&gains)) return -1;
	*count=n+gains;
	return 0;
}

static inline struct t41_awb_frame_buffers t41_awb_buffers(struct t41_awb_owned *o)
{
	struct t41_awb_frame_buffers b={o->p,o->s,o->report,sizeof(o->p),sizeof(o->s),sizeof(o->report)};
	return b;
}

/* hardware_param's ordered geometry/threshold writes and both statistics
 * triggers. Threshold state still advances while regional MMIO is frozen. */
static inline int t41_awb_hardware(struct t41_awb_owned *o, unsigned int geometry,
		struct t41_awb_register words[T41_AWB_HARDWARE_WORDS], unsigned int *count)
{
	unsigned int i,n=0,geo[10],packed[5];
	unsigned short values[11];
	if (!o || !words || !count) return -1;
	geometry=geometry && !o->s[0xc5f8];
	if ((geometry && t41_awb_geometry(o->p,sizeof(o->p),geo)) ||
	    t41_awb_thresholds(o->p,sizeof(o->p),t41_tmo_le32(o->s+0xea9c),values,packed)) return -1;
	if (geometry) {
		for (i=0;i<9;++i) { words[n].address=0x18004+i*4; words[n++].value=geo[i]; }
		words[n].address=0x1804c; words[n++].value=geo[9];
	}
	for (i=0;i<11;++i) t41_awb_gain_put16(o->p+0xcb0+i*2,values[i]);
	if (!o->s[0xeaa1]) {
		for (i=0;i<4;++i) { words[n].address=0x18028+i*4; words[n++].value=packed[i]; }
		words[n].address=0x18000; words[n++].value=1;
	}
	words[n].address=0x18038; words[n++].value=packed[4];
	words[n].address=0x18000; words[n++].value=1;
	*count=n;
	return 0;
}
#endif
