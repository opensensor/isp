/* SPDX-License-Identifier: MIT */
#ifndef TX_ISP_TOP_H
#define TX_ISP_TOP_H

/* One calibration byte per bypass bit. Restore only writer-owned bits;
 * unrelated safety bypasses must survive. Invalid input is atomic. */
static inline int tx_isp_top_restore(const unsigned char *flags,
		unsigned int bytes, unsigned int mask, unsigned int *shadow)
{
	unsigned int bit, value = 0;
	if (!flags || bytes < 32 || !shadow)
		return -1;
	for (bit = 0; bit < 32; ++bit) {
		if (!(mask & (1U << bit)))
			continue;
		if (flags[bit] > 1)
			return -1;
		value |= (unsigned int)flags[bit] << bit;
	}
	*shadow = (*shadow & ~mask) | value;
	return 0;
}
#endif
