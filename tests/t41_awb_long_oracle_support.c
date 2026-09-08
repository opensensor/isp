#include "t41_awb_stats_oracle_support.c"
unsigned int oracle_ratios[2], oracle_failed, oracle_temperature, oracle_calls;
unsigned long long oracle_div64_u64(unsigned long long a,unsigned long long b) { return a/b; }
/* Deliberate test seam: no cluster detector instructions in this oracle.
 * The native implementation receives the same synthetic detector outputs. */
int oracle_detector(void **view,unsigned int *ratios,unsigned int *failed)
{
	unsigned int *ct=view[0xb];
	++oracle_calls;
	ratios[0]=oracle_ratios[0]; ratios[1]=oracle_ratios[1];
	failed[0]=oracle_failed; *ct=oracle_temperature;
	return 0;
}
