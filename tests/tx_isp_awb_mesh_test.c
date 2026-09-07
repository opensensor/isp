#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include "tx_isp/tx_isp_awb_mesh.h"

static void test_ct_prior(void)
{
	struct tx_isp_awb_ct_config c = {
		.ev_low = 10, .ev_high = 50,
		.day = {5000, 4500, 6200, 6600},
		.transition = {5000, 4500, 6500, 7000},
		.night = {1000, 500, 12500, 13000},
		.day_enabled = 1, .night_enabled = 0,
		.day_floor_q8 = 20, .night_floor_q8 = 8,
	};
	struct tx_isp_awb_ct_prior p;
	u32 ev;
	assert(!tx_isp_awb_ct_prior_build(&c, 0, &p));
	assert(p.knots[2] == 6200 && p.floor_q8 == 20);
	assert(!tx_isp_awb_ct_prior_build(&c, 16, &p));
	assert(p.knots[2] == 6338 && p.knots[3] == 6784);
	assert(!tx_isp_awb_ct_prior_build(&c, 28, &p));
	assert(p.knots[2] == 6500 && p.knots[3] == 7000);
	assert(tx_isp_awb_ct_weight(&p, 4499) == 20);
	assert(tx_isp_awb_ct_weight(&p, 4500) == 20);
	assert(tx_isp_awb_ct_weight(&p, 4750) == 138);
	assert(tx_isp_awb_ct_weight(&p, 5000) == 256);
	assert(tx_isp_awb_ct_weight(&p, 6500) == 256);
	assert(tx_isp_awb_ct_weight(&p, 6750) == 138);
	assert(tx_isp_awb_ct_weight(&p, 7000) == 20);
	assert(!tx_isp_awb_ct_prior_build(&c, 56, &p));
	assert(p.floor_q8 == 128);
	assert(!tx_isp_awb_ct_prior_build(&c, ~0U, &p));
	assert(p.floor_q8 == 256);
	c.night_enabled = 1;
	assert(!tx_isp_awb_ct_prior_build(&c, 63, &p));
	assert(p.knots[0] == 1000 && p.floor_q8 == 8);
	c.day_enabled = 0;
	assert(!tx_isp_awb_ct_prior_build(&c, 50, &p));
	assert(p.floor_q8 == 256);
	assert(!tx_isp_awb_ct_prior_build(&c, 63, &p));
	assert(p.floor_q8 == 8);
	c.day_enabled = 1;
	for (ev = 0; ev < 100; ++ev) {
		u32 ct;
		assert(!tx_isp_awb_ct_prior_build(&c, ev, &p));
		for (ct = 0; ct < 15000; ct += 37)
			assert(tx_isp_awb_ct_weight(&p, ct) <= 256);
	}
	c.ev_high = 12;
	assert(tx_isp_awb_ct_prior_build(&c, 10, &p) == -EINVAL);
	c.ev_high = 10;
	assert(tx_isp_awb_ct_prior_build(&c, 10, &p) == -EINVAL);
	c.ev_high = 50;
	c.day[1] = c.day[0];
	assert(tx_isp_awb_ct_prior_build(&c, 10, &p) == -EINVAL);
	/* A second, independent calibration changes both the EV schedule and
	 * illuminant windows. The implementation must not encode the above. */
	c = (struct tx_isp_awb_ct_config){
		.ev_low = 100, .ev_high = 400,
		.day = {3300, 3100, 4200, 4600},
		.transition = {4300, 3900, 5200, 5800},
		.night = {2000, 1500, 8000, 8500},
		.day_enabled = 1, .night_enabled = 1,
		.day_floor_q8 = 64, .night_floor_q8 = 128,
	};
	assert(!tx_isp_awb_ct_prior_build(&c, 150, &p));
	assert(p.knots[0] == 3800 && p.knots[1] == 3500);
	assert(p.knots[2] == 4700 && p.knots[3] == 5200);
	assert(p.floor_q8 == 64 && tx_isp_awb_ct_weight(&p, 3650) == 160);
	assert(!tx_isp_awb_ct_prior_build(&c, 450, &p));
	assert(p.knots[0] == 3150 && p.floor_q8 == 96);
}

int main(void)
{
	u32 axis[15], weights[225], mired[225], r = 99, b = 88, i;
	struct tx_isp_awb_mesh m = {
		.red_axis = axis, .blue_axis = axis, .weights = weights,
		.red_calibration_q10 = 2048, .blue_calibration_q10 = 2048,
		.red_bias_q10 = 1080, .blue_bias_q10 = 1024,
	};
	struct tx_isp_awb_accumulator s = {0};
	test_ct_prior();
	for (i = 0; i < 15; ++i) axis[i] = 64 + 32 * i;
	for (i = 0; i < 225; ++i) weights[i] = 256;
	assert(!tx_isp_awb_mesh_validate(&m));
	assert(tx_isp_awb_mesh_validate(NULL) == -EINVAL);
	axis[2] = axis[1];
	assert(tx_isp_awb_mesh_validate(&m) == -EINVAL);
	axis[2] += 32;
	weights[0] = 257;
	assert(tx_isp_awb_mesh_validate(&m) == -EINVAL);
	weights[0] = 256;
	assert(tx_isp_awb_mesh_result(&m, &s, 1, &r, &b) == -ENODATA);
	assert(r == 99 && b == 88);
	assert(!tx_isp_awb_mesh_add(&m, &s, 100, 0, 100, 1));
	assert(!tx_isp_awb_mesh_add(&m, &s, 100, 200, 100, 0));
	assert(!tx_isp_awb_mesh_add(&m, &s, 0x3ffffff, 1, 100, 1));
	assert(!tx_isp_awb_mesh_add(&m, &s, 100, 10000, 100, 1));
	assert(tx_isp_awb_mesh_add(&m, &s, 100, 200, 100, 1));
	assert(!tx_isp_awb_mesh_result(&m, &s, 1, &r, &b));
	assert(r == 2160 && b == 2048);
	/* CT selection changes evidence weights, never post-gain calibration. */
	m.ct_mired = mired;
	m.ct_prior = (struct tx_isp_awb_ct_prior){{5000, 4500, 6500, 7000}, 20};
	for (i = 0; i < 225; ++i) mired[i] = 200;
	assert(!tx_isp_awb_mesh_validate(&m));
	memset(&s, 0, sizeof(s));
	assert(tx_isp_awb_mesh_add(&m, &s, 100, 200, 100, 1));
	assert(s.weight == 256);
	for (i = 0; i < 225; ++i) mired[i] = 300;
	memset(&s, 0, sizeof(s));
	assert(tx_isp_awb_mesh_add(&m, &s, 100, 200, 100, 1));
	assert(s.weight == 20);
	assert(!tx_isp_awb_mesh_result(&m, &s, 1, &r, &b));
	assert(r == 2160 && b == 2048);
	memset(mired, 0, sizeof(mired));
	assert(!tx_isp_awb_mesh_add(&m, &s, 100, 200, 100, 1));
	mired[0] = 65536;
	assert(tx_isp_awb_mesh_validate(&m) == -EINVAL);
	m.ct_mired = NULL;
	/* Both far edges and the final mesh cell stay in bounds. */
	assert(tx_isp_awb_mesh_add(&m, &s, 512, 512, 512, 1));
	assert(tx_isp_awb_mesh_add(&m, &s, 64, 512, 64, 1));
	memset(weights, 0, sizeof(weights));
	memset(&s, 0, sizeof(s));
	/* Bilinear center of four knots; reject an off-locus green scene. */
	weights[6 * 15 + 6] = 256;
	assert(tx_isp_awb_mesh_add(&m, &s, 272, 512, 272, 1));
	assert(s.weight == 64);
	assert(!tx_isp_awb_mesh_add(&m, &s, 100, 512, 100, 1));
	/* Maximum supported accumulation must not wrap at Q10 conversion. */
	for (i = 0; i < 225; ++i) weights[i] = 256;
	memset(&s, 0, sizeof(s));
	for (i = 0; i < 65536; ++i)
		assert(tx_isp_awb_mesh_add(&m, &s, 0x1ffffff, 0x3fffffe, 0x1ffffff, 255));
	assert(!tx_isp_awb_mesh_add(&m, &s, 1, 2, 1, 1));
	assert(!tx_isp_awb_mesh_result(&m, &s, 65536, &r, &b));
	assert(r == 2160 && b == 2048);
	puts("AWB neutral mesh: passed");
	return 0;
}
