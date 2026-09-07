/* SPDX-License-Identifier: MIT */
#include "tx_isp/tx_isp_awb_mesh.h"
#ifdef __KERNEL__
#include <linux/errno.h>
#include <linux/math64.h>
#define awb_div(n, d) div64_u64((n), (d))
#else
#include <errno.h>
#define awb_div(n, d) ((n) / (d))
#endif

int tx_isp_awb_mesh_validate(const struct tx_isp_awb_mesh *m)
{
	u32 i;
	if (!m || !m->red_axis || !m->blue_axis || !m->weights ||
	    !m->red_calibration_q10 || m->red_calibration_q10 > 8192 ||
	    !m->blue_calibration_q10 || m->blue_calibration_q10 > 8192 ||
	    !m->red_bias_q10 || m->red_bias_q10 > 4096 ||
	    !m->blue_bias_q10 || m->blue_bias_q10 > 4096)
		return -EINVAL;
	for (i = 0; i < TX_ISP_AWB_MESH_SIZE; ++i) {
		if (!m->red_axis[i] || m->red_axis[i] > 4096 ||
		    !m->blue_axis[i] || m->blue_axis[i] > 4096 ||
		    (i && (m->red_axis[i] <= m->red_axis[i - 1] ||
			   m->blue_axis[i] <= m->blue_axis[i - 1])))
			return -EINVAL;
	}
	for (i = 0; i < TX_ISP_AWB_MESH_SIZE * TX_ISP_AWB_MESH_SIZE; ++i)
		if (m->weights[i] > 256)
			return -EINVAL;
	return 0;
}

static u32 awb_axis_cell(const u32 *axis, u32 value)
{
	u32 i = 0;
	while (i < TX_ISP_AWB_MESH_SIZE - 2 && value >= axis[i + 1])
		++i;
	return i;
}

int tx_isp_awb_mesh_add(const struct tx_isp_awb_mesh *m,
		      struct tx_isp_awb_accumulator *s,
		      u32 r, u32 g, u32 b, u32 spatial)
{
	u32 x, y, ix, iy, dx, dy, a, c, w;
	u64 xr, yb;
	const u32 *row;
	if (!r || !g || !b || r > 0x3ffffff || g > 0x3ffffff ||
	    b > 0x3ffffff || !spatial || spatial > 255 || s->samples >= 65536)
		return 0;
	xr = awb_div((u64)r * m->red_calibration_q10, (u64)g * 4);
	yb = awb_div((u64)b * m->blue_calibration_q10, (u64)g * 4);
	/* Out-of-model colors must not be clamped onto a neutral mesh edge. */
	if (xr < m->red_axis[0] || xr > m->red_axis[14] ||
	    yb < m->blue_axis[0] || yb > m->blue_axis[14])
		return 0;
	x = xr;
	y = yb;
	ix = awb_axis_cell(m->red_axis, x);
	iy = awb_axis_cell(m->blue_axis, y);
	dx = ((x - m->red_axis[ix]) << 8) /
		(m->red_axis[ix + 1] - m->red_axis[ix]);
	dy = ((y - m->blue_axis[iy]) << 8) /
		(m->blue_axis[iy + 1] - m->blue_axis[iy]);
	row = m->weights + iy * TX_ISP_AWB_MESH_SIZE + ix;
	a = row[0] * (256 - dx) + row[1] * dx;
	c = row[15] * (256 - dx) + row[16] * dx;
	w = ((a * (256 - dy) + c * dy + 32768) >> 16) * spatial;
	if (!w)
		return 0;
	/* At most 65536 samples, 26-bit sums, 16-bit weights: < 2^58. */
	s->red += (u64)r * w;
	s->green += (u64)g * w;
	s->blue += (u64)b * w;
	s->weight += w;
	++s->samples;
	return 1;
}

static u64 awb_ratio_q10(u64 numerator, u64 denominator)
{
	while (numerator > (~(u64)0 >> 10)) {
		numerator >>= 1;
		denominator >>= 1;
	}
	return denominator ? awb_div(numerator << 10, denominator) : 0;
}

int tx_isp_awb_mesh_result(const struct tx_isp_awb_mesh *m,
			 const struct tx_isp_awb_accumulator *s,
			 u32 minimum, u32 *red, u32 *blue)
{
	u64 r, b;
	if (!minimum || minimum > 65536 || !red || !blue)
		return -EINVAL;
	if (s->samples < minimum || !s->red || !s->green || !s->blue ||
	    s->weight < minimum * 16U)
		return -ENODATA;
	/* Normalize large accumulators before adding fractional precision. */
	r = awb_ratio_q10(s->green, s->red);
	b = awb_ratio_q10(s->green, s->blue);
	if (r > 16384 || b > 16384)
		return -ERANGE;
	r = (r * m->red_bias_q10 + 512) >> 10;
	b = (b * m->blue_bias_q10 + 512) >> 10;
	if (r < 512 || r > 6144 || b < 512 || b > 6144)
		return -ERANGE;
	*red = r;
	*blue = b;
	return 0;
}
