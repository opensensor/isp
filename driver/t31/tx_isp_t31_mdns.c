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
