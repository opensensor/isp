#include <assert.h>
#include <errno.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "tx_isp/tx_isp_tuning_abi.h"

#define ARRAY_SIZE(values) (sizeof(values) / sizeof((values)[0]))

#define ABI_ASSERT(name, expression) \
	typedef char abi_assert_##name[(expression) ? 1 : -1]

ABI_ASSERT(control_size, sizeof(struct tx_isp_tuning_control) == 8);
ABI_ASSERT(control_value,
	offsetof(struct tx_isp_tuning_control, value_or_ptr) == 4);
ABI_ASSERT(ext_control_size,
	sizeof(struct tx_isp_tuning_ext_control) == 12);
ABI_ASSERT(t23_ext_control_size,
	sizeof(struct tx_isp_tuning_t23_ext_control) == 16);
ABI_ASSERT(t41_control_size,
	sizeof(struct tx_isp_tuning_t41_control) == 16);
ABI_ASSERT(t41_control_id,
	offsetof(struct tx_isp_tuning_t41_control, id) == 8);
ABI_ASSERT(expr_size, sizeof(struct tx_isp_tuning_expr) == 12);
ABI_ASSERT(expr_min,
	offsetof(struct tx_isp_tuning_expr, integration_time_min) == 6);
ABI_ASSERT(expr_line,
	offsetof(struct tx_isp_tuning_expr, one_line_expr_in_us) == 10);
ABI_ASSERT(ev_size, sizeof(struct tx_isp_tuning_ev_attr) == 24);
ABI_ASSERT(wb_size, sizeof(struct tx_isp_tuning_wb) == 8);

static void test_descriptors(void)
{
	static const struct tx_isp_tuning_cmd_desc valid[] = {
		{ TX_ISP_TUNING_CMD_EXPR, 12, TX_ISP_TUNING_DIR_GET,
		  TX_ISP_TUNING_PAYLOAD_USER_PTR },
		{ TX_ISP_TUNING_CMD_T41_RUNNING_MODE, 4,
		  TX_ISP_TUNING_DIR_SET, TX_ISP_TUNING_PAYLOAD_USER_PTR },
		{ TX_ISP_TUNING_CMD_T41_SHARPNESS, 1,
		  TX_ISP_TUNING_DIR_GET | TX_ISP_TUNING_DIR_SET,
		  TX_ISP_TUNING_PAYLOAD_USER_PTR },
		{ TX_ISP_TUNING_CMD_BCSH_HUE, 4,
		  TX_ISP_TUNING_DIR_GET | TX_ISP_TUNING_DIR_SET,
		  TX_ISP_TUNING_PAYLOAD_INLINE },
	};
	static const struct tx_isp_tuning_cmd_desc duplicate[] = {
		{ TX_ISP_TUNING_CMD_EXPR, 12, TX_ISP_TUNING_DIR_GET,
		  TX_ISP_TUNING_PAYLOAD_USER_PTR },
		{ TX_ISP_TUNING_CMD_EXPR, 24, TX_ISP_TUNING_DIR_GET,
		  TX_ISP_TUNING_PAYLOAD_USER_PTR },
	};
	static const struct tx_isp_tuning_cmd_desc split_direction[] = {
		{ TX_ISP_TUNING_CMD_EXPR, 12, TX_ISP_TUNING_DIR_GET,
		  TX_ISP_TUNING_PAYLOAD_USER_PTR },
		{ TX_ISP_TUNING_CMD_EXPR, 12, TX_ISP_TUNING_DIR_SET,
		  TX_ISP_TUNING_PAYLOAD_USER_PTR },
	};
	static const struct tx_isp_tuning_cmd_desc malformed_direction = {
		TX_ISP_TUNING_CMD_EXPR, 12, 0,
		TX_ISP_TUNING_PAYLOAD_USER_PTR
	};
	static const struct tx_isp_tuning_cmd_desc malformed_kind = {
		TX_ISP_TUNING_CMD_EXPR, 12, TX_ISP_TUNING_DIR_GET, 9
	};
	const struct tx_isp_tuning_cmd_desc *found;

	assert(tx_isp_tuning_cmd_table_validate(NULL, 0) == 0);
	assert(tx_isp_tuning_cmd_table_validate(NULL, 1) == -EINVAL);
	assert(tx_isp_tuning_cmd_table_validate(valid,
						ARRAY_SIZE(valid)) == 0);
	assert(tx_isp_tuning_cmd_table_validate(duplicate,
						ARRAY_SIZE(duplicate)) == -EINVAL);
	assert(tx_isp_tuning_cmd_table_validate(split_direction,
				ARRAY_SIZE(split_direction)) == 0);
	assert(tx_isp_tuning_cmd_table_validate(&malformed_direction, 1) ==
	       -EINVAL);
	assert(tx_isp_tuning_cmd_table_validate(&malformed_kind, 1) == -EINVAL);

	found = tx_isp_tuning_cmd_find(valid, ARRAY_SIZE(valid),
				       TX_ISP_TUNING_CMD_EXPR,
				       TX_ISP_TUNING_DIR_GET);
	assert(found != NULL);
	assert(found->payload_size == sizeof(struct tx_isp_tuning_expr));
	assert(found->payload_kind == TX_ISP_TUNING_PAYLOAD_USER_PTR);
	assert(tx_isp_tuning_cmd_find(valid, ARRAY_SIZE(valid),
				      TX_ISP_TUNING_CMD_EXPR,
				      TX_ISP_TUNING_DIR_SET) == NULL);
	assert(tx_isp_tuning_cmd_find(valid, ARRAY_SIZE(valid), 0, 0) == NULL);
}

static void store_u16(uint8_t *bytes, unsigned int offset, uint16_t value)
{
	memcpy(bytes + offset, &value, sizeof(value));
}

static void store_u32(uint8_t *bytes, unsigned int offset, uint32_t value)
{
	memcpy(bytes + offset, &value, sizeof(value));
}

static void test_response_packers(void)
{
	uint8_t sparse[TX_ISP_TUNING_EV_SPARSE_BYTES] = { 0 };
	uint8_t t41_expr[TX_ISP_TUNING_T41_AE_EXPR_BYTES];
	uint8_t t41_stats[TX_ISP_TUNING_T41_AE_STATS_BYTES];
	uint32_t histogram[TX_ISP_TUNING_T41_AE_HIST_BINS] = { 0 };
	const uint32_t wb_words[3] = { 2, 0x12345, 0x2abcd };
	const struct tx_isp_tuning_t41_ae_expr_values t41_values = {
		.integration_time = 369,
		.analog_gain_x1024 = 1344,
		.min_integration_time = 1,
		.max_integration_time = 1760,
		.max_analog_gain_x1024 = 15872,
		.total_gain_db = 29906,
		.exposure_value = 506,
		.ev_log2 = 588809,
	};
	struct tx_isp_tuning_expr expr;
	struct tx_isp_tuning_ev_attr ev;
	struct tx_isp_tuning_wb wb;
	uint64_t exposure;
	uint32_t value;
	uint32_t mean_q8;

	assert(tx_isp_tuning_expr_pack(&expr, 1, 0x12345, 2, 0x34567,
					 0x45678) == 0);
	assert(expr.mode == 1);
	assert(expr.integration_time == 0x2345);
	assert(expr.integration_time_min == 2);
	assert(expr.integration_time_max == 0x4567);
	assert(expr.one_line_expr_in_us == 0x5678);

	store_u32(sparse, 0, 1120);
	store_u32(sparse, 12 * sizeof(uint32_t), 1);
	store_u16(sparse, TX_ISP_TUNING_EV_EXPR_MIN_OFFSET, 1);
	store_u16(sparse, TX_ISP_TUNING_EV_EXPR_MAX_OFFSET, 1436);
	store_u16(sparse, TX_ISP_TUNING_EV_LINE_US_OFFSET, 28);
	assert(tx_isp_tuning_expr_from_sparse(&expr, sparse,
					      sizeof(sparse)) == 0);
	assert(expr.mode == 1);
	assert(expr.integration_time == 1120);
	assert(expr.integration_time_min == 1);
	assert(expr.integration_time_max == 1436);
	assert(expr.one_line_expr_in_us == 28);
	assert(tx_isp_tuning_expr_from_sparse(&expr, sparse,
			TX_ISP_TUNING_EV_LINE_US_OFFSET) == -EINVAL);

	assert(tx_isp_tuning_ev_pack(&ev, 80, 30000, 7, 256, 256, 9) == 0);
	assert(ev.ev == 80);
	assert(ev.expr_us == 30000);
	assert(ev.ev_log2 == 7);
	assert(ev.again == 256);
	assert(ev.dgain == 256);
	assert(ev.gain_log2 == 9);

	assert(tx_isp_tuning_wb_pack(&wb, wb_words, 10) == 0);
	assert(wb.mode == 2);
	assert(wb.r_gain == 0x2345);
	assert(wb.b_gain == 0xabcd);
	assert(tx_isp_tuning_wb_pack(&wb, wb_words, 2) == -EINVAL);

	assert(tx_isp_tuning_t41_ae_expr_pack(t41_expr, sizeof(t41_expr),
					      &t41_values) == 0);
	memcpy(&value, t41_expr + TX_ISP_TUNING_T41_AE_EXPR_INTEGRATION,
	       sizeof(value));
	assert(value == 369);
	memcpy(&value, t41_expr + TX_ISP_TUNING_T41_AE_EXPR_TOTAL_GAIN,
	       sizeof(value));
	assert(value == 29906);
	memcpy(&exposure, t41_expr + TX_ISP_TUNING_T41_AE_EXPR_EXPOSURE,
	       sizeof(exposure));
	assert(exposure == 506);

	histogram[40] = 10;
	histogram[80] = 10;
	assert(tx_isp_tuning_t41_ae_stats_pack(t41_stats, sizeof(t41_stats),
					       histogram,
					       ARRAY_SIZE(histogram),
					       &mean_q8) == 0);
	assert(mean_q8 == 60U * 256U);
	memcpy(&value, t41_stats + TX_ISP_TUNING_T41_AE_STATS_HIST256 +
	       40 * sizeof(uint32_t), sizeof(value));
	assert(value == 10);
	memcpy(&value, t41_stats + TX_ISP_TUNING_T41_AE_STATS_ZONES,
	       sizeof(value));
	assert(value == 60);
	assert(tx_isp_tuning_expr_pack(NULL, 0, 0, 0, 0, 0) == -EINVAL);
	assert(tx_isp_tuning_ev_pack(NULL, 0, 0, 0, 0, 0, 0) == -EINVAL);
	assert(tx_isp_tuning_wb_pack(NULL, wb_words, 10) == -EINVAL);
	assert(tx_isp_tuning_t41_ae_expr_pack(NULL, 0, &t41_values) ==
	       -EINVAL);
	assert(tx_isp_tuning_t41_ae_stats_pack(NULL, 0, histogram,
					       ARRAY_SIZE(histogram), NULL) ==
	       -EINVAL);
}

int main(void)
{
	test_descriptors();
	test_response_packers();
	puts("tx_isp_tuning_abi tests passed");
	return 0;
}
