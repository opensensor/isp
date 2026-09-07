#include "t41_lce_oracle_support.c"
static unsigned int pools[8][32];
unsigned int oracle_allocations;
unsigned int oracle_params[2],oracle_callbacks[32],oracle_heap_used;
unsigned int oracle_mutex_key;
static unsigned char oracle_heap[65536] __attribute__((aligned(4096)));
void *oracle_kmalloc(unsigned int bytes,unsigned int flags)
{
	unsigned int start=(oracle_heap_used+4095)&~4095U; (void)flags;
	if(start+bytes>sizeof(oracle_heap)) { ++oracle_bad_write; return (void *)0; }
	oracle_heap_used=start+bytes; return oracle_heap+start;
}
void *oracle_alloc(unsigned int bytes)
{
	if(bytes!=128 || oracle_allocations>=8) { ++oracle_bad_write; return (void *)0; }
	return pools[oracle_allocations++];
}
void oracle_free(void *p) { (void)p; }
unsigned long long oracle_left_shift(unsigned long long value,unsigned int bits) { return value<<bits; }
unsigned int oracle_unsafe_divisions;
unsigned long long oracle_div64_u64(unsigned long long numerator,unsigned long long denominator)
{
	/* Invalid OEM arithmetic is counted separately, never used as a parity
	 * result. This lets the harness verify the native rejection safely. */
	if(!denominator) { ++oracle_unsafe_divisions; return 0; }
	return numerator/denominator;
}
