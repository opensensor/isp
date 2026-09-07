#include "t41_lce_oracle_support.c"
static unsigned int pools[8][32];
unsigned int oracle_allocations;
void *oracle_alloc(unsigned int bytes)
{
	if(bytes!=128 || oracle_allocations>=8) { ++oracle_bad_write; return (void *)0; }
	return pools[oracle_allocations++];
}
void oracle_free(void *p) { (void)p; }
unsigned long long oracle_left_shift(unsigned long long value,unsigned int bits) { return value<<bits; }
unsigned long long oracle_div64_u64(unsigned long long numerator,unsigned long long denominator) { return numerator/denominator; }
