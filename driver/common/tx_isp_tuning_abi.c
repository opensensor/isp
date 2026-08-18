#ifdef __KERNEL__
#include <linux/errno.h>
#include <linux/math64.h>
#include <linux/string.h>
#else
#include <errno.h>
#include <string.h>
#endif

#include "../include/tx_isp/tx_isp_tuning_abi.h"

const struct tx_isp_tuning_cmd_desc *
tx_isp_tuning_cmd_find(const struct tx_isp_tuning_cmd_desc *table,
		       unsigned int count, u32 id, u8 direction)
{
	unsigned int i;

	if (!table || !direction)
		return NULL;

	for (i = 0; i < count; i++) {
		if (table[i].id == id && (table[i].directions & direction))
			return &table[i];
	}

	return NULL;
}

int tx_isp_tuning_cmd_table_validate(
	const struct tx_isp_tuning_cmd_desc *table, unsigned int count)
{
	unsigned int i;
	unsigned int j;

	if (!count)
		return 0;
	if (!table)
		return -EINVAL;

	for (i = 0; i < count; i++) {
		if (!table[i].directions ||
		    (table[i].directions &
		     ~(TX_ISP_TUNING_DIR_GET | TX_ISP_TUNING_DIR_SET)))
			return -EINVAL;
		if (table[i].payload_kind != TX_ISP_TUNING_PAYLOAD_INLINE &&
		    table[i].payload_kind != TX_ISP_TUNING_PAYLOAD_USER_PTR)
			return -EINVAL;

		for (j = i + 1; j < count; j++) {
			if (table[i].id == table[j].id &&
			    (table[i].directions & table[j].directions))
				return -EINVAL;
		}
	}

	return 0;
}

int tx_isp_tuning_expr_pack(struct tx_isp_tuning_expr *out, u32 mode,
			    u32 integration_time, u32 integration_time_min,
			    u32 integration_time_max, u32 one_line_expr_in_us)
{
	if (!out)
		return -EINVAL;

	out->mode = mode;
	out->integration_time = (u16)integration_time;
	out->integration_time_min = (u16)integration_time_min;
	out->integration_time_max = (u16)integration_time_max;
	out->one_line_expr_in_us = (u16)one_line_expr_in_us;
	return 0;
}

static u16 tx_isp_tuning_load_u16(const u8 *bytes, unsigned int offset)
{
	u16 value;

	memcpy(&value, bytes + offset, sizeof(value));
	return value;
}

static u32 tx_isp_tuning_load_u32(const u8 *bytes, unsigned int offset)
{
	u32 value;

	memcpy(&value, bytes + offset, sizeof(value));
	return value;
}

int tx_isp_tuning_expr_from_sparse(struct tx_isp_tuning_expr *out,
				   const void *sparse,
				   unsigned int sparse_bytes)
{
	const u8 *bytes = sparse;

	if (!out || !sparse ||
	    sparse_bytes < TX_ISP_TUNING_EV_LINE_US_OFFSET + sizeof(u16))
		return -EINVAL;

	return tx_isp_tuning_expr_pack(
		out,
		tx_isp_tuning_load_u32(bytes, 12U * sizeof(u32)) > 0,
		tx_isp_tuning_load_u32(bytes, 0),
		tx_isp_tuning_load_u16(bytes,
				      TX_ISP_TUNING_EV_EXPR_MIN_OFFSET),
		tx_isp_tuning_load_u16(bytes,
				      TX_ISP_TUNING_EV_EXPR_MAX_OFFSET),
		tx_isp_tuning_load_u16(bytes,
				      TX_ISP_TUNING_EV_LINE_US_OFFSET));
}

int tx_isp_tuning_ev_pack(struct tx_isp_tuning_ev_attr *out, u32 ev,
			  u32 expr_us, u32 ev_log2, u32 again, u32 dgain,
			  u32 gain_log2)
{
	if (!out)
		return -EINVAL;

	out->ev = ev;
	out->expr_us = expr_us;
	out->ev_log2 = ev_log2;
	out->again = again;
	out->dgain = dgain;
	out->gain_log2 = gain_log2;
	return 0;
}

int tx_isp_tuning_wb_pack(struct tx_isp_tuning_wb *out,
			  const u32 *words, u32 mode_count)
{
	if (!out || !words || words[0] >= mode_count)
		return -EINVAL;

	out->mode = words[0];
	out->r_gain = (u16)words[1];
	out->b_gain = (u16)words[2];
	return 0;
}

u32 tx_isp_tuning_wb_stats_pack(u32 r_gain, u32 b_gain)
{
	return ((r_gain & 0xffffU) << 16) | (b_gain & 0xffffU);
}

static void tx_isp_tuning_store_u16(u8 *bytes, unsigned int offset, u16 value)
{
	memcpy(bytes + offset, &value, sizeof(value));
}

static void tx_isp_tuning_store_u32(u8 *bytes, unsigned int offset, u32 value)
{
	memcpy(bytes + offset, &value, sizeof(value));
}

static void tx_isp_tuning_store_u64(u8 *bytes, unsigned int offset, u64 value)
{
	memcpy(bytes + offset, &value, sizeof(value));
}

int tx_isp_tuning_t41_ae_expr_pack(
	void *out, unsigned int out_bytes,
	const struct tx_isp_tuning_t41_ae_expr_values *values)
{
	u8 *bytes = out;

	if (!out || !values || out_bytes < TX_ISP_TUNING_T41_AE_EXPR_BYTES)
		return -EINVAL;

	memset(bytes, 0, TX_ISP_TUNING_T41_AE_EXPR_BYTES);
	tx_isp_tuning_store_u32(bytes,
		TX_ISP_TUNING_T41_AE_EXPR_INTEGRATION,
		values->integration_time);
	tx_isp_tuning_store_u32(bytes, TX_ISP_TUNING_T41_AE_EXPR_AGAIN,
		values->analog_gain_x1024);
	/* Unity ISP digital gain and the stock OS04D10 constraint envelope. */
	tx_isp_tuning_store_u32(bytes, 36, 1024);
	tx_isp_tuning_store_u32(bytes, 68, 1);
	tx_isp_tuning_store_u32(bytes, 72, values->min_integration_time);
	tx_isp_tuning_store_u32(bytes, 76, 1024);
	tx_isp_tuning_store_u32(bytes, 80, 1024);
	tx_isp_tuning_store_u32(bytes, 84, 1024);
	tx_isp_tuning_store_u32(bytes, 88, values->max_integration_time);
	tx_isp_tuning_store_u32(bytes, 92, values->max_analog_gain_x1024);
	tx_isp_tuning_store_u32(bytes, 96, 1024);
	tx_isp_tuning_store_u32(bytes, 100, 32767);
	/* Retain the stock linear-mode short-frame defaults. */
	tx_isp_tuning_store_u32(bytes, 168, 1);
	tx_isp_tuning_store_u32(bytes, 172, 3);
	tx_isp_tuning_store_u32(bytes, 176, 1024);
	tx_isp_tuning_store_u32(bytes, 180, 1024);
	tx_isp_tuning_store_u32(bytes, 184, 1024);
	tx_isp_tuning_store_u32(bytes, 188, 92);
	tx_isp_tuning_store_u32(bytes, 192, 1024);
	tx_isp_tuning_store_u32(bytes, 196, 1024);
	tx_isp_tuning_store_u32(bytes, 200, 1024);
	tx_isp_tuning_store_u32(bytes,
		TX_ISP_TUNING_T41_AE_EXPR_TOTAL_GAIN,
		values->total_gain_db);
	tx_isp_tuning_store_u64(bytes,
		TX_ISP_TUNING_T41_AE_EXPR_EXPOSURE,
		values->exposure_value);
	tx_isp_tuning_store_u32(bytes,
		TX_ISP_TUNING_T41_AE_EXPR_EV_LOG2, values->ev_log2);
	return 0;
}

int tx_isp_tuning_t41_ae_stats_pack(void *out, unsigned int out_bytes,
				    const u32 *histogram,
				    unsigned int histogram_bins,
				    u32 *mean_q8)
{
	u8 *bytes = out;
	u64 samples = 0;
	u64 weighted = 0;
	u32 five_bin[5] = { 0 };
	u32 mean;
	unsigned int i;

	if (!out || !histogram ||
	    out_bytes < TX_ISP_TUNING_T41_AE_STATS_BYTES ||
	    histogram_bins < TX_ISP_TUNING_T41_AE_HIST_BINS)
		return -EINVAL;

	memset(bytes, 0, TX_ISP_TUNING_T41_AE_STATS_BYTES);
	for (i = 0; i < TX_ISP_TUNING_T41_AE_HIST_BINS; i++) {
		u32 count = histogram[i] & 0x1fffffU;
		unsigned int group = (i * 5U) >> 8;

		tx_isp_tuning_store_u32(bytes,
			TX_ISP_TUNING_T41_AE_STATS_HIST256 + i * sizeof(u32),
			count);
		five_bin[group] += count;
		samples += count;
		weighted += (u64)count * i;
	}
	for (i = 0; i < 5; i++)
		tx_isp_tuning_store_u16(bytes,
			TX_ISP_TUNING_T41_AE_STATS_HIST5 + i * sizeof(u16),
			five_bin[i] > 0xffffU ? 0xffffU : (u16)five_bin[i]);

	if (samples) {
#ifdef __KERNEL__
		mean = (u32)div64_u64(weighted << 8, samples);
#else
		mean = (u32)((weighted << 8) / samples);
#endif
	} else {
		mean = 0;
	}
	for (i = 0; i < TX_ISP_TUNING_T41_AE_ZONE_COUNT; i++)
		tx_isp_tuning_store_u32(bytes,
			TX_ISP_TUNING_T41_AE_STATS_ZONES + i * sizeof(u32),
			(mean + 128U) >> 8);
	if (mean_q8)
		*mean_q8 = mean;
	return 0;
}
