#include <stdint.h>
#define ORACLE_WRITE_LIMIT 8
#include "t41_awb_stats_oracle_support.c"
/* Only external ownership/MMIO seams are stubbed. All initialization and
 * control policy, including nested refreshes, executes the OEM instructions. */
static unsigned char heap[210000] __attribute__((aligned(32)));
unsigned int oracle_allocated,oracle_hw_calls,oracle_gain_calls;
unsigned int oracle_params[2],oracle_callbacks[64];
void *oracle_kmalloc(unsigned int n)
{
	unsigned int at=oracle_allocated;
	if (n>sizeof(heap)-at) { ++oracle_bad; return 0; }
	oracle_allocated+=(n+31)&~31U;
	return heap+at;
}
void oracle_kfree(void *p) { (void)p; }
int oracle_noop(void) { return 0; }
int oracle_unexpected(void) { ++oracle_bad; return 0; }
int oracle_hardware(unsigned int channel)
{
	if (channel) ++oracle_bad;
	++oracle_hw_calls; return 0;
}
int oracle_gain(unsigned int channel)
{
	if (channel) ++oracle_bad;
	++oracle_gain_calls; return 0;
}
