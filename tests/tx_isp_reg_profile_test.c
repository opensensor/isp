#include <assert.h>
#include <errno.h>
#include <stdio.h>

#include "tx_isp/tx_isp_reg_profile.h"

struct write_log {
	struct tx_isp_reg_value values[8];
	unsigned int count;
};

static void record_write(void *opaque, u32 reg, u32 value)
{
	struct write_log *log = opaque;

	assert(log->count < sizeof(log->values) / sizeof(log->values[0]));
	log->values[log->count].reg = reg;
	log->values[log->count].value = value;
	log->count++;
}

static void test_ordered_profile_with_commit(void)
{
	static const struct tx_isp_reg_value values[] = {
		{ 0x4800U, 0x00000000U },
		{ 0x4808U, 0x00000064U },
		{ 0x4810U, 0x00000002U },
	};
	const struct tx_isp_reg_profile profile = {
		.values = values,
		.count = sizeof(values) / sizeof(values[0]),
		.commit_reg = 0x499cU,
		.commit_value = 1U,
		.has_commit = true,
	};
	struct write_log log = { { { 0, 0 } }, 0 };

	assert(tx_isp_reg_profile_apply(&profile, record_write, &log) == 0);
	assert(log.count == 4U);
	assert(log.values[0].reg == 0x4800U);
	assert(log.values[0].value == 0U);
	assert(log.values[1].reg == 0x4808U);
	assert(log.values[1].value == 0x64U);
	assert(log.values[2].reg == 0x4810U);
	assert(log.values[2].value == 2U);
	assert(log.values[3].reg == 0x499cU);
	assert(log.values[3].value == 1U);
}

static void test_profile_validation(void)
{
	const struct tx_isp_reg_profile empty = {
		.values = NULL,
		.count = 0,
		.has_commit = false,
	};
	const struct tx_isp_reg_profile missing_values = {
		.values = NULL,
		.count = 1,
		.has_commit = false,
	};
	struct write_log log = { { { 0, 0 } }, 0 };

	assert(tx_isp_reg_profile_apply(NULL, record_write, &log) == -EINVAL);
	assert(tx_isp_reg_profile_apply(&empty, NULL, &log) == -EINVAL);
	assert(tx_isp_reg_profile_apply(&missing_values, record_write, &log) ==
	       -EINVAL);
	assert(tx_isp_reg_profile_apply(&empty, record_write, &log) == 0);
	assert(log.count == 0U);
}

static void test_register_flag_merge(void)
{
	u32 zeroes[40] = { 0 };
	u32 ones[40];
	u32 non_boolean[] = { 2U };
	unsigned int i;

	for (i = 0; i < sizeof(ones) / sizeof(ones[0]); ++i)
		ones[i] = 1U;

	assert(tx_isp_reg_flags_merge(0xffffffffU, zeroes, 32U) == 0U);
	assert(tx_isp_reg_flags_merge(0U, ones, 32U) == 0xffffffffU);
	assert(tx_isp_reg_flags_merge(0U, ones, 40U) == 0xffffffffU);
	assert(tx_isp_reg_flags_merge(0x12345678U, NULL, 32U) ==
	       0x12345678U);
	assert(tx_isp_reg_flags_merge(0U, non_boolean, 1U) == 2U);
}

int main(void)
{
	test_ordered_profile_with_commit();
	test_profile_validation();
	test_register_flag_merge();
	puts("tx_isp_reg_profile tests passed");
	return 0;
}
