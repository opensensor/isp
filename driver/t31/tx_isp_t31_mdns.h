#ifndef TX_ISP_T31_MDNS_H
#define TX_ISP_T31_MDNS_H

#ifdef __KERNEL__
#include <linux/types.h>
#else
#include <stdbool.h>
#include <stdint.h>
typedef uint32_t u32;
#endif

u32 tx_isp_t31_mdns_top1(u32 ass_enable, u32 bgm_enable,
			 u32 ref_weight_bypass, u32 sta_group_num,
			 u32 psn_enable, u32 psn_max_num,
			 bool memory_optimized);

#endif
