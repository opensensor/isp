/* SPDX-License-Identifier: MIT */
#ifndef TX_ISP_T41_AWB_GRAY_H
#define TX_ISP_T41_AWB_GRAY_H
#include "tx_isp_t41_awb_ct.h"

/* The caller owns the generic distance LUT, initialized independently of
 * sensor calibration. The tail is the OEM's quantized distance support. */
static inline unsigned int t41_awb_distance_weight(const unsigned char *lut,
		unsigned int distance)
{
	if (distance<514) return t41_tmo_le16(lut+distance*2);
	if (distance<540) return 5;
	if (distance<572) return 4;
	if (distance<615) return 3;
	if (distance<679) return 2;
	return distance<818;
}

static inline unsigned int t41_awb_mul3(unsigned int precision,
		unsigned int a, unsigned int b, unsigned int c)
{
	return t41_ae_fixed_mul(precision,t41_ae_fixed_mul(precision,a,b),c);
}

/* Gray-world fallback for one selection (0 neutral, 1 global). Ratios are
 * four dense rows*cols planes as consumed by the OEM estimator interface;
 * distance scratch has hardware stride 15. First weigh in-axis zones four
 * times as strongly, then optionally refine around the mean by distance.
 * This consumes calibrated spatial weights, not sensor-specific gains. */
static inline int t41_awb_grayworld_mode(unsigned char *p, unsigned int pb,
		unsigned char *s, unsigned int sb, const unsigned int *ratios,
		unsigned int ratio_words, unsigned int mode,
		unsigned int out[2], unsigned int *failed)
{
	unsigned int precision, fraction, rows, cols, count, i, r, c, unity, round;
	unsigned int sum_r=0, sum_b=0, sum_w=0, mean_r, mean_b, ct;
	unsigned int min_r,max_r,min_b,max_b;
	const unsigned int *red,*blue;
	if (!p || pb<0x12e1 || !s || sb<T41_AWB_STATE_BYTES || !ratios ||
	    !out || !failed || mode>1) return -1;
	if (t41_tmo_le16(p+0xd60+mode*2)!=1 || *failed!=1) return 0;
	rows=t41_tmo_le16(p+0xc6e); cols=t41_tmo_le16(p+0xc72); count=rows*cols;
	precision=t41_tmo_le16(p+0xcd2); fraction=t41_tmo_le16(p+0xcd4);
	if (!rows || rows>15 || !cols || cols>15 || ratio_words<4*count ||
	    t41_awb_ct_calculate(p,pb,0,0,&ct)) return -1;
	unity=1U<<precision; round=1U<<(fraction-1);
	min_r=t41_tmo_le32(p+0x38)<<fraction; max_r=t41_tmo_le32(p+0x70)<<fraction;
	min_b=t41_tmo_le32(p+0x74)<<fraction; max_b=t41_tmo_le32(p+0xac)<<fraction;
	red=ratios+mode*2*count; blue=red+count;
	for (i=0;i<count;++i) {
		unsigned int spatial=(unsigned int)p[0x1200+i]<<precision;
		unsigned int w=red[i]>=min_r && red[i]<=max_r &&
			blue[i]>=min_b && blue[i]<=max_b ? 4U<<precision : unity;
		sum_r+=t41_awb_mul3(precision,red[i],spatial,w);
		sum_b+=t41_awb_mul3(precision,blue[i],spatial,w);
		sum_w+=t41_ae_fixed_mul(precision,spatial,w);
	}
	if (!sum_w || !sum_r || !sum_b) {
		*failed=1; out[0]=out[1]=unity; t41_ae_put32(p+0x28,5000);
		return 0;
	}
	*failed=0;
	mean_r=t41_ae_fixed_div(precision,sum_r,sum_w);
	mean_b=t41_ae_fixed_div(precision,sum_b,sum_w);
	if (t41_tmo_le16(p+0xd64)==1) {
		unsigned int cr=(round+mean_r)>>fraction, cb=(round+mean_b)>>fraction;
		for (r=0;r<rows;++r) for (c=0;c<cols;++c) {
			unsigned int pos=r*cols+c, rr=(round+red[pos])>>fraction;
			unsigned int bb=(round+blue[pos])>>fraction;
			unsigned int dr=rr>cr ? rr-cr : cr-rr, db=bb>cb ? bb-cb : cb-bb;
			unsigned int w=t41_awb_distance_weight(s+0x3520,dr*dr+db*db)<<fraction;
			t41_ae_put32(s+0xd904+(r*15+c)*4,w);
		}
		sum_r=sum_b=sum_w=0;
		for (r=0;r<rows;++r) for (c=0;c<cols;++c) {
			unsigned int pos=r*cols+c, spatial=(unsigned int)p[0x1200+pos]<<precision;
			unsigned int w=t41_tmo_le32(s+0xd904+(r*15+c)*4);
			sum_r+=t41_awb_mul3(precision,red[pos],spatial,w);
			sum_b+=t41_awb_mul3(precision,blue[pos],spatial,w);
			sum_w+=t41_ae_fixed_mul(precision,spatial,w);
		}
		if (!sum_w || !sum_r || !sum_b) *failed=1;
		else {
			mean_r=t41_ae_fixed_div(precision,sum_r,sum_w);
			mean_b=t41_ae_fixed_div(precision,sum_b,sum_w);
		}
	}
	/* A failed refinement retains the first mean; the OEM's intermediate
	 * unity/5000 assignment is overwritten by this final publication. */
	if (t41_awb_ct_calculate(p,pb,mean_r,mean_b,&ct)) return -1;
	t41_ae_put32(p+0x28,ct); out[0]=mean_r; out[1]=mean_b;
	return 0;
}
#endif
