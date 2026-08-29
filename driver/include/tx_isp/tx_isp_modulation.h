#ifndef TX_ISP_MODULATION_H
#define TX_ISP_MODULATION_H

/*
 * Kernel-independent Apical modulation primitives.
 *
 * Pair tables are flat { x, y } arrays.  Keeping the table storage out of
 * this interface lets each SoC adapter retain its vendor-facing type names
 * and layout while sharing the interpolation arithmetic.
 */

static inline unsigned int
tx_isp_modulate_pairs_u16(unsigned short x, const unsigned short *pairs,
			  int pair_count)
{
	unsigned int index;
	unsigned int lower_x;
	unsigned int upper_x;
	unsigned int fraction;
	unsigned int weighted;

	if (!pairs || pair_count <= 0)
		return 0;
	if (x <= pairs[0])
		return pairs[1];
	if (x >= pairs[(pair_count - 1) * 2])
		return pairs[(pair_count - 1) * 2 + 1];

	for (index = 1; index < (unsigned int)pair_count; index++)
		if (x < pairs[index * 2])
			break;

	lower_x = pairs[(index - 1) * 2];
	upper_x = pairs[index * 2];
	if (lower_x == upper_x)
		return pairs[index * 2 + 1];

	fraction = ((unsigned int)(x - lower_x) << 8) /
		   (upper_x - lower_x);
	weighted = pairs[(index - 1) * 2 + 1] * (256U - fraction);
	weighted += pairs[index * 2 + 1] * fraction;
	return (weighted >> 8) & 0xffffU;
}

static inline unsigned int
tx_isp_modulate_pairs_u32(unsigned int x, const unsigned int *pairs,
			  int pair_count)
{
	unsigned int index;
	unsigned int lower_x;
	unsigned int upper_x;
	unsigned int fraction;
	unsigned int weighted;

	if (!pairs || pair_count <= 0)
		return 0;
	if (x <= pairs[0])
		return pairs[1];
	if (x >= pairs[(pair_count - 1U) * 2U])
		return pairs[(pair_count - 1U) * 2U + 1U];

	for (index = 1; index < (unsigned int)pair_count; index++)
		if (x < pairs[index * 2U])
			break;

	lower_x = pairs[(index - 1U) * 2U];
	upper_x = pairs[index * 2U];
	if (lower_x == upper_x)
		return pairs[index * 2U + 1U];

	fraction = ((x - lower_x) << 8) / (upper_x - lower_x);
	/* The OEM MIPS routine retains the low 32-bit weighted sum. */
	weighted = pairs[(index - 1U) * 2U + 1U] *
		   (256U - fraction);
	weighted += pairs[index * 2U + 1U] * fraction;
	return weighted >> 8;
}

static inline unsigned int
tx_isp_modulate_pairs_scaled_u16(unsigned short x,
				 unsigned short target_min,
				 unsigned short target_max,
				 const unsigned short *pairs,
				 int pair_count)
{
	unsigned int index;
	unsigned int first_x;
	unsigned int last_x;
	unsigned int first_y;
	unsigned int last_y;
	unsigned int lower_x;
	unsigned int upper_x;
	unsigned int global_fraction;
	unsigned int local_fraction;
	unsigned int minimum_scale;
	unsigned int maximum_scale;
	unsigned int scale;
	unsigned int weighted_y;

	if (!pairs || pair_count <= 0)
		return 0;
	first_x = pairs[0];
	last_x = pairs[(pair_count - 1) * 2];
	if (x <= first_x)
		return target_min;
	if (x >= last_x)
		return target_max;

	first_y = pairs[1];
	last_y = pairs[(pair_count - 1) * 2 + 1];
	if (!first_y || !last_y || first_x == last_x)
		return 0;

	for (index = 1; index < (unsigned int)pair_count; index++)
		if (x < pairs[index * 2])
			break;

	lower_x = pairs[(index - 1U) * 2U];
	upper_x = pairs[index * 2U];
	if (lower_x == upper_x)
		return pairs[index * 2U + 1U];

	global_fraction = ((unsigned int)(x - first_x) << 8) /
			  (last_x - first_x);
	local_fraction = ((unsigned int)(x - lower_x) << 8) /
			 (upper_x - lower_x);
	minimum_scale = ((unsigned int)target_min << 8) / first_y;
	maximum_scale = ((unsigned int)target_max << 8) / last_y;
	scale = (global_fraction * maximum_scale +
		 (256U - global_fraction) * minimum_scale) >> 8;
	weighted_y = pairs[(index - 1U) * 2U + 1U] *
		     (256U - local_fraction);
	weighted_y += pairs[index * 2U + 1U] * local_fraction;

	/* Preserve the OEM low-word multiply before the Q16 shift. */
	return (scale * weighted_y) >> 16;
}

/*
 * Remap a pair table onto caller-supplied endpoints while retaining the
 * table's shape.  This is the APICAL calc_adjust_modulation_u16 ABI used by
 * T20 for user/IQ-controlled sinter and temper limits.
 */
static inline unsigned int
tx_isp_modulate_pairs_adjusted_u16(unsigned short x,
				   unsigned short target_min,
				   unsigned short target_max,
				   const unsigned short *pairs,
				   int pair_count)
{
	unsigned int first_x;
	unsigned int last_x;
	unsigned int first_y;
	unsigned int last_y;
	unsigned int value;
	unsigned int numerator;

	if (!pairs || pair_count <= 0)
		return 0;
	first_x = pairs[0];
	last_x = pairs[(pair_count - 1) * 2];
	if (x <= first_x)
		return target_min;
	if (x >= last_x)
		return target_max;

	first_y = pairs[1];
	last_y = pairs[(pair_count - 1) * 2 + 1];
	if (!first_y || !last_y || first_x == last_x || first_y == last_y)
		return 0;

	value = tx_isp_modulate_pairs_u16(x, pairs, pair_count);
	/* Match the OEM's Q8 operation ordering. */
	numerator = ((unsigned int)(target_max - target_min) << 8) *
		(value - first_y);
	return (target_min + ((numerator / (last_y - first_y)) >> 8)) &
		0xffffU;
}

static inline unsigned int
tx_isp_modulate_equidistant_u16(unsigned short x,
				const unsigned short *table,
				unsigned short table_len)
{
	unsigned int step;
	unsigned int index;
	unsigned int fraction;
	unsigned int weighted;

	if (!table || !table_len)
		return 0;
	if (!x || table_len == 1U)
		return table[0];
	if (x == 0xffffU)
		return table[table_len - 1U];

	step = 0x10000U / (table_len - 1U);
	if (!step)
		return table[0];
	index = x / step;
	if (index >= table_len - 1U)
		return table[table_len - 1U];
	fraction = ((x - index * step) << 8) / step;
	weighted = table[index + 1U] * fraction;
	weighted += table[index] * (256U - fraction);
	return (weighted >> 8) & 0xffffU;
}

static inline unsigned int
tx_isp_modulate_equidistant_u32(unsigned int x,
				const unsigned int *table,
				unsigned int table_len)
{
	unsigned int step;
	unsigned int index;
	unsigned int fraction;
	unsigned int weighted;

	if (!table || !table_len)
		return 0;
	if (!x || table_len == 1U)
		return table[0];
	if (x == 0xffffU)
		return table[table_len - 1U];

	step = 0x10000U / (table_len - 1U);
	if (!step)
		return table[0];
	index = x / step;
	if (index >= table_len - 1U)
		return table[table_len - 1U];
	fraction = ((x - index * step) << 8) / step;
	weighted = table[index + 1U] * fraction;
	weighted += table[index] * (256U - fraction);
	return weighted >> 8;
}

static inline unsigned int
tx_isp_modulate_inverse_equidistant_u16(unsigned short x,
					const unsigned short *table,
					unsigned short table_len)
{
	unsigned int index;
	unsigned int step;
	unsigned int fraction;
	unsigned int weighted_index;

	if (!table || !table_len)
		return 0;
	if (x <= table[0])
		return 0;
	if (x >= table[table_len - 1U])
		return 0xffffU;

	for (index = 1; index < table_len; index++)
		if (x < table[index])
			break;

	step = 0x10000U / (table_len - 1U);
	if (table[index] == table[index - 1U])
		return (index * step) & 0xffffU;

	fraction = ((unsigned int)(x - table[index - 1U]) << 8) /
		   (table[index] - table[index - 1U]);
	weighted_index = index * fraction;
	weighted_index += (index - 1U) * (256U - fraction);
	return ((step * weighted_index) >> 8) & 0xffffU;
}

static inline unsigned int
tx_isp_modulate_inverse_equidistant_u32(unsigned int x,
					const unsigned int *table,
					unsigned int table_len)
{
	unsigned int index;
	unsigned int step;
	unsigned int fraction;
	unsigned int weighted_index;

	if (!table || !table_len)
		return 0;
	if (x <= table[0])
		return 0;
	if (x >= table[table_len - 1U])
		return 0xffffU;

	for (index = 1; index < table_len; index++)
		if (x < table[index])
			break;

	step = 0x10000U / (table_len - 1U);
	if (table[index] == table[index - 1U])
		return index * step;

	fraction = ((x - table[index - 1U]) << 8) /
		   (table[index] - table[index - 1U]);
	weighted_index = index * fraction;
	weighted_index += (index - 1U) * (256U - fraction);
	return (step * weighted_index) >> 8;
}

#endif /* TX_ISP_MODULATION_H */
