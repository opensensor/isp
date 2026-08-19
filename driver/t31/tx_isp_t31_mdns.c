#include "tx_isp_t31_mdns.h"

u32 tx_isp_t31_mdns_top1(u32 ass_enable, u32 bgm_enable,
			 u32 ref_weight_bypass, u32 sta_group_num,
			 u32 psn_enable, u32 psn_max_num,
			 bool memory_optimized)
{
	if (memory_optimized) {
		ass_enable = 0;
		bgm_enable = 0;
		psn_enable = 0;
	}

	return ass_enable |
	       (bgm_enable << 4) |
	       (ref_weight_bypass << 8) |
	       (sta_group_num << 12) |
	       (psn_enable << 16) |
	       (psn_max_num << 20);
}

void tx_isp_t31_mdns_scale_ratio_table(u32 dst[9], const u32 src[9],
				       u32 ratio)
{
	unsigned int i;

	for (i = 0; i < 9; i++) {
		u32 value = src[i];

		if (ratio <= 0x80) {
			value = (ratio * value) >> 7;
		} else {
			u32 headroom = value < 0xc8 ? 0xc8 - value : 0;

			value += (headroom * (ratio - 0x80)) >> 7;
		}

		dst[i] = value;
	}
}
