#ifdef __KERNEL__
#include <linux/errno.h>
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
