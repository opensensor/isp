/* SPDX-License-Identifier: MIT */
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "../driver/t41/tx_isp_t41_sensor_gain.h"
static unsigned int requested, calls, variant;
static unsigned int allocate(unsigned int gain, unsigned char shift, unsigned int *code)
{
	unsigned int index = gain / 16384;
	assert(shift == 16); requested = gain; ++calls;
	*code = variant ? (index * 0x137U) ^ 0x3210U : index + 7;
	return index * 16384;
}
static unsigned int missing_code(unsigned int g, unsigned char s, unsigned int *v)
{ (void)g; (void)s; (void)v; return 0; }
static unsigned int bad_gain(unsigned int g, unsigned char s, unsigned int *v)
{ (void)s; *v=1; return g+1; }
int main(void)
{
	struct t41_sensor_gain a, b, saved;
	for (unsigned int gain = 1024; gain < 65536; gain += 13) {
		variant=0; assert(!t41_sensor_gain_allocate(gain,4U<<16,allocate,&a));
		variant=1; assert(!t41_sensor_gain_allocate(gain,4U<<16,allocate,&b));
		assert(a.code != b.code && a.log2_q16 == b.log2_q16 && a.linear_q10 == b.linear_q10);
		assert(requested <= (4U<<16) && b.log2_q16 <= requested);
	}
	assert(!t41_sensor_gain_allocate(1024,0,allocate,&a));
	assert(a.log2_q16==0 && a.linear_q10==1024);
	assert(!t41_sensor_gain_allocate(tx_isp_exp2_u32(251937,16,10),251937,allocate,&a));
	assert(requested==251937);
	saved=a; calls=0;
	assert(t41_sensor_gain_allocate(1023,65536,allocate,&a));
	assert(t41_sensor_gain_allocate(1024,17U<<16,allocate,&a));
	assert(t41_sensor_gain_allocate(1024,65536,NULL,&a));
	assert(!calls && !memcmp(&a,&saved,sizeof(a)));
	assert(t41_sensor_gain_allocate(1024,65536,missing_code,&a));
	assert(t41_sensor_gain_allocate(1024,65536,bad_gain,&a));
	assert(!memcmp(&a,&saved,sizeof(a)));
	puts("T41 sensor-owned gain allocation: nonlinear encodings, bounds and units passed");
	return 0;
}
