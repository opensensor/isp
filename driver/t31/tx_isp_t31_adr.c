#ifdef __KERNEL__
#include <linux/errno.h>
#include <linux/types.h>
#else
#include <errno.h>
#endif

#include "tx_isp_t31_adr.h"

int tx_isp_t31_adr_scale_mapb(
	u32 strength,
	const u32 minimum[TX_ISP_T31_ADR_MAP_CHANNELS],
	const u32 *const source[TX_ISP_T31_ADR_MAP_CHANNELS],
	u32 *const output[TX_ISP_T31_ADR_MAP_CHANNELS])
{
	static const u32 ceiling[TX_ISP_T31_ADR_MAP_CHANNELS] = {
		0x190, 0x1f4, 0x258, 0x258,
	};
	u32 channel;
	u32 point;

	if (strength > 0xff || !minimum || !source || !output)
		return -EINVAL;

	for (channel = 0; channel < TX_ISP_T31_ADR_MAP_CHANNELS;
	     ++channel) {
		if (!source[channel] || !output[channel])
			return -EINVAL;

		for (point = 0; point < TX_ISP_T31_ADR_MAP_POINTS; ++point) {
			u32 base = source[channel][point];
			u32 value;

			if (strength < 0x81) {
				value = (strength * base) >> 7;
			} else {
				u32 room = base >= ceiling[channel] ? 0 :
					ceiling[channel] - base;

				value = base + ((room * (strength - 0x80)) >> 7);
			}

			if (value < minimum[channel])
				value = minimum[channel];
			output[channel][point] = value;
		}
	}

	return 0;
}
