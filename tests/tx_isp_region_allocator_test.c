#include <assert.h>
#include <errno.h>
#include <stdint.h>
#include <stdio.h>

#include "tx_isp/tx_isp_region_allocator.h"

#define TEST_BASE 0x06000000U

static void test_validation_and_alignment(void)
{
	struct tx_isp_region_allocator allocator;

	assert(tx_isp_region_allocator_init(NULL, TEST_BASE, 0x10000U) ==
	       -EINVAL);
	assert(tx_isp_region_allocator_init(&allocator, 0, 0x10000U) ==
	       -EINVAL);
	assert(tx_isp_region_allocator_init(&allocator, TEST_BASE, 0) ==
	       -EINVAL);
	assert(tx_isp_region_allocator_init(&allocator, 0xfffff000U,
					    0x2000U) == -ERANGE);
	assert(tx_isp_region_allocator_init(&allocator, TEST_BASE,
					    0x10000U) == 0);
	assert(tx_isp_region_alloc(&allocator, 1) == TEST_BASE);
	assert(tx_isp_region_alloc(&allocator, 4096) == TEST_BASE + 0x1000U);
	assert(tx_isp_region_alloc(&allocator, 4097) == TEST_BASE + 0x2000U);
	assert(allocator.used_size == 0x4000U);
	assert(tx_isp_region_alloc(&allocator, UINT32_MAX) == 0);
}

static void test_first_fit_and_coalescing(void)
{
	struct tx_isp_region_allocator allocator;
	u32 first;
	u32 second;
	u32 third;

	assert(tx_isp_region_allocator_init(&allocator, TEST_BASE,
					    0x8000U) == 0);
	first = tx_isp_region_alloc(&allocator, 0x1000U);
	second = tx_isp_region_alloc(&allocator, 0x2000U);
	third = tx_isp_region_alloc(&allocator, 0x1000U);
	assert(first == TEST_BASE);
	assert(second == TEST_BASE + 0x1000U);
	assert(third == TEST_BASE + 0x3000U);
	assert(tx_isp_region_free(&allocator, second) == 0);
	assert(tx_isp_region_alloc(&allocator, 0x1001U) == second);
	assert(tx_isp_region_free(&allocator, first) == 0);
	assert(tx_isp_region_free(&allocator, second) == 0);
	assert(tx_isp_region_free(&allocator, third) == 0);
	assert(allocator.used_size == 0);
	assert(allocator.first->addr == TEST_BASE);
	assert(allocator.first->size == 0x8000U);
	assert(allocator.first->next == NULL);
	assert(tx_isp_region_free(&allocator, TEST_BASE + 0x7000U) == -ENOENT);
	assert(tx_isp_region_free(&allocator, third) == -ENOENT);
	assert(tx_isp_region_free(&allocator, 0) == -EINVAL);
}

static void test_descriptor_limit_and_reuse(void)
{
	struct tx_isp_region_allocator allocator;
	u32 addresses[TX_ISP_REGION_MAX_NODES];
	u32 index;

	assert(tx_isp_region_allocator_init(
		&allocator, TEST_BASE,
		(TX_ISP_REGION_MAX_NODES + 1U) *
		TX_ISP_REGION_ALIGNMENT) == 0);
	for (index = 0; index < TX_ISP_REGION_MAX_NODES - 1U; index++) {
		addresses[index] = tx_isp_region_alloc(
			&allocator, TX_ISP_REGION_ALIGNMENT);
		assert(addresses[index] ==
		       TEST_BASE + index * TX_ISP_REGION_ALIGNMENT);
	}
	/* Twenty live descriptors leave space but no descriptor for a split. */
	assert(tx_isp_region_alloc(&allocator, TX_ISP_REGION_ALIGNMENT) == 0);
	addresses[TX_ISP_REGION_MAX_NODES - 1U] = tx_isp_region_alloc(
		&allocator, 2U * TX_ISP_REGION_ALIGNMENT);
	assert(addresses[TX_ISP_REGION_MAX_NODES - 1U] ==
	       TEST_BASE + (TX_ISP_REGION_MAX_NODES - 1U) *
	       TX_ISP_REGION_ALIGNMENT);
	for (index = 0; index < TX_ISP_REGION_MAX_NODES - 1U; index += 2U)
		assert(tx_isp_region_free(&allocator, addresses[index]) == 0);
	for (index = 0; index < TX_ISP_REGION_MAX_NODES - 1U; index += 2U)
		assert(tx_isp_region_alloc(&allocator, TX_ISP_REGION_ALIGNMENT) ==
		       addresses[index]);
}

int main(void)
{
	test_validation_and_alignment();
	test_first_fit_and_coalescing();
	test_descriptor_limit_and_reuse();
	puts("tx_isp_region_allocator tests passed");
	return 0;
}
