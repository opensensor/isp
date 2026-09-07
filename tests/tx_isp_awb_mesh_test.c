#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include "tx_isp/tx_isp_awb_mesh.h"

int main(void)
{
	u32 axis[15], weights[225], r = 99, b = 88, i;
	struct tx_isp_awb_mesh m = {axis, axis, weights, 2048, 2048, 1080, 1024};
	struct tx_isp_awb_accumulator s = {0};
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
