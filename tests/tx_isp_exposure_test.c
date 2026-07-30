#include <assert.h>
#include <errno.h>
#include <stdint.h>
#include <stdio.h>

#include "tx_isp/tx_isp_exposure.h"
#include "../driver/t31/tx_isp_t31_exposure.h"

static const u16 nodes_60hz[] = { 369, 737, 1106, 1474 };

static void test_target_scaling(void)
{
	u32 scaled = 99;

	assert(tx_isp_exposure_target_scale(12400, 0x200, 0x400,
					    &scaled) == 0);
	assert(scaled == 6200);
	assert(tx_isp_exposure_target_scale(12400, 0x1e0, 0x400,
					    &scaled) == 0);
	assert(scaled == 5813);

	scaled = 99;
	assert(tx_isp_exposure_target_scale(0, 0x200, 0x400,
					    &scaled) == -EINVAL);
	assert(tx_isp_exposure_target_scale(UINT32_MAX, UINT32_MAX, 1,
					    &scaled) == -ERANGE);
	assert(scaled == 99);
}

static void test_flicker_node_generation(void)
{
	u16 nodes[4] = { 0 };
	u32 count = 99;

	assert(tx_isp_flicker_nodes_build(44225, 120, 1760, nodes, 4,
					  &count) == 0);
	assert(count == 4);
	assert(nodes[0] == 369);
	assert(nodes[1] == 737);
	assert(nodes[2] == 1106);
	assert(nodes[3] == 1474);

	count = 99;
	assert(tx_isp_flicker_nodes_build(44225, 100, 1760, nodes, 4,
					  &count) == 0);
	assert(count == 3);
	assert(nodes[0] == 442);
	assert(nodes[1] == 885);
	assert(nodes[2] == 1327);

	count = 99;
	assert(tx_isp_flicker_nodes_build(10, 1, 100, nodes, 4,
					  &count) == -ENOSPC);
	assert(count == 99);
	assert(tx_isp_flicker_nodes_build(0, 1, 10, nodes, 4,
					  &count) == -EINVAL);
}

static void test_t31_flicker_lut_adapter(void)
{
	u32 lut[TX_ISP_T31_FLICKER_LUT_ENTRIES];
	u32 last_index = 99;
	u32 index;

	for (index = 0; index < TX_ISP_T31_FLICKER_LUT_ENTRIES; ++index)
		lut[index] = 0xdeadbeefU;
	assert(tx_isp_t31_flicker_lut_build(
		       300, 1440, lut, TX_ISP_T31_FLICKER_LUT_ENTRIES,
		       &last_index) == 0);
	assert(last_index == 3);
	assert(lut[0] == 300);
	assert(lut[1] == 600);
	assert(lut[2] == 900);
	assert(lut[3] == 1200);
	assert(lut[4] == 1200);
	assert(lut[TX_ISP_T31_FLICKER_LUT_ENTRIES - 1] == 1200);

	assert(tx_isp_t31_flicker_lut_build(
		       360, 1440, lut, TX_ISP_T31_FLICKER_LUT_ENTRIES,
		       &last_index) == 0);
	assert(last_index == 3);
	assert(lut[0] == 360);
	assert(lut[3] == 1440);
	assert(lut[4] == 1440);

	assert(tx_isp_t31_flicker_lut_build(300, 200, lut, 4,
					    &last_index) == 0);
	assert(last_index == 0);
	assert(lut[0] == 300);
	assert(lut[3] == 300);

	lut[0] = 0x12345678U;
	last_index = 99;
	assert(tx_isp_t31_flicker_lut_build(0, 1440, lut, 4,
					    &last_index) == -EINVAL);
	assert(lut[0] == 0x12345678U);
	assert(last_index == 99);
}

static void test_short_and_flicker_floor(void)
{
	struct tx_isp_exposure_plan plan;

	assert(tx_isp_exposure_plan_build(120U * 0x10U,
					  1, 1760, 0x10, 0xf8,
					  nodes_60hz, 4, 0, &plan) == 0);
	assert(plan.integration == 120);
	assert(plan.again == 0x10);

	assert(tx_isp_exposure_plan_build(120U * 0x10U,
					  1, 1760, 0x10, 0xf8,
					  nodes_60hz, 4, 369, &plan) == 0);
	assert(plan.integration == 369);
	assert(plan.again == 0x10);

	assert(tx_isp_exposure_plan_build(120U * 0x10U,
					  1, 1760, 0x10, 0xf8,
					  nodes_60hz, 4, 443, &plan) == 0);
	assert(plan.integration == 443);
	assert(plan.again == 0x10);

	assert(tx_isp_exposure_plan_build(500U * 0x10U,
					  1, 1760, 0x10, 0xf8,
					  nodes_60hz, 4, 443, &plan) == 0);
	assert(plan.integration == 443);
	assert(plan.again == 18);
}

static void test_node_and_gain_selection(void)
{
	struct tx_isp_exposure_plan plan;

	assert(tx_isp_exposure_plan_build(369U * 0x1fU,
					  1, 1760, 0x10, 0xf8,
					  nodes_60hz, 4, 0, &plan) == 0);
	assert(plan.integration == 369);
	assert(plan.again == 0x1f);

	assert(tx_isp_exposure_plan_build(800U * 0x10U,
					  1, 1760, 0x10, 0xf8,
					  nodes_60hz, 4, 0, &plan) == 0);
	assert(plan.integration == 737);
	assert(plan.again == 17);

	assert(tx_isp_exposure_plan_build(1600U * 0x10U,
					  1, 1760, 0x10, 0xf8,
					  nodes_60hz, 4, 0, &plan) == 0);
	assert(plan.integration == 1474);
	assert(plan.again == 17);
}

static void test_bounds_and_validation(void)
{
	static const u16 bad_nodes[] = { 369, 369 };
	struct tx_isp_exposure_plan plan = { 11, 22 };

	assert(tx_isp_exposure_plan_build(0, 4, 100, 0x10, 0x20,
					  NULL, 0, 0, &plan) == 0);
	assert(plan.integration == 4);
	assert(plan.again == 0x10);

	assert(tx_isp_exposure_plan_build(UINT64_MAX,
					  4, 100, 0x10, 0x20,
					  NULL, 0, 0, &plan) == 0);
	assert(plan.integration == 100);
	assert(plan.again == 0x20);

	plan.integration = 11;
	plan.again = 22;
	assert(tx_isp_exposure_plan_build(1000, 0, 100, 0x10, 0x20,
					  NULL, 0, 0, &plan) == -EINVAL);
	assert(tx_isp_exposure_plan_build(1000, 4, 100, 0x10, 0x20,
					  bad_nodes, 2, 0, &plan) == -EINVAL);
	assert(tx_isp_exposure_plan_build(1000, 4, 100, 0x10, 0x20,
					  NULL, 0, 101, &plan) == -EINVAL);
	assert(tx_isp_exposure_plan_build(1000, 4, 100, 0x10, 0x20,
					  NULL, 0, 0, NULL) == -EINVAL);
	assert(plan.integration == 11);
	assert(plan.again == 22);
}

int main(void)
{
	test_target_scaling();
	test_flicker_node_generation();
	test_t31_flicker_lut_adapter();
	test_short_and_flicker_floor();
	test_node_and_gain_selection();
	test_bounds_and_validation();
	puts("tx_isp_exposure tests passed");
	return 0;
}
