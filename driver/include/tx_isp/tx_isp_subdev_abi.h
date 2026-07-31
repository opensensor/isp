#ifndef TX_ISP_SUBDEV_ABI_H
#define TX_ISP_SUBDEV_ABI_H

/*
 * Recovered TX-ISP subdevice ABI facts shared by the three supported SoCs.
 *
 * These are byte offsets in objects exchanged with OEM modules, not a
 * suggested C object model.  Keep the two pad slots physically named here:
 * the recovered T23/T40 code and T31's declared struct assign directions to
 * them consistently, while T31's current raw accessors assign the opposite
 * directions.  Neutral slot names let that discrepancy remain visible until
 * it is resolved with a device smoke test.
 */

/* Pad layout shared by the recovered T23 and T40 implementations. */
#define TX_ISP_ABI_PAD_OWNER_OFFSET             0x00
#define TX_ISP_ABI_PAD_INDEX_OFFSET             0x04
#define TX_ISP_ABI_PAD_TYPE_OFFSET              0x05
#define TX_ISP_ABI_PAD_LINKS_TYPE_OFFSET        0x06
#define TX_ISP_ABI_PAD_STATE_OFFSET             0x07
#define TX_ISP_ABI_PAD_LINK_OFFSET              0x08
#define TX_ISP_ABI_PAD_EVENT_OFFSET             0x1c
#define TX_ISP_ABI_PAD_PRIVATE_OFFSET           0x20
#define TX_ISP_ABI_PAD_STRIDE                   0x24

#define TX_ISP_ABI_PADTYPE_UNDEFINED            0x00
#define TX_ISP_ABI_PADTYPE_INPUT                0x01
#define TX_ISP_ABI_PADTYPE_OUTPUT               0x02

#define TX_ISP_ABI_PADSTATE_FREE                0x02
#define TX_ISP_ABI_PADSTATE_LINKED              0x03
#define TX_ISP_ABI_PADSTATE_STREAM              0x04

/* Active link record embedded in every 32-bit recovered pad. */
#define TX_ISP_ABI_LINK_SOURCE_OFFSET            0x00
#define TX_ISP_ABI_LINK_SINK_OFFSET              0x04
#define TX_ISP_ABI_LINK_REVERSE_OFFSET           0x08
#define TX_ISP_ABI_LINK_FLAG_OFFSET              0x0c
#define TX_ISP_ABI_LINK_STATE_OFFSET             0x10
#define TX_ISP_ABI_LINK_SIZE                     0x14

/*
 * Recovered T23/T41 link teardown clears the endpoint/reverse/flag prefix but
 * deliberately leaves the link-state word alone. Callers still own pad state.
 * `link` must be a side-effect-free pointer expression.
 */
#define TX_ISP_ABI_LINK_CLEAR_ENDPOINTS(link) do {			\
	*(unsigned int *)((char *)(link) +				\
		TX_ISP_ABI_LINK_SOURCE_OFFSET) = 0;			\
	*(unsigned int *)((char *)(link) +				\
		TX_ISP_ABI_LINK_SINK_OFFSET) = 0;			\
	*(unsigned int *)((char *)(link) +				\
		TX_ISP_ABI_LINK_REVERSE_OFFSET) = 0;			\
	*(unsigned int *)((char *)(link) +				\
		TX_ISP_ABI_LINK_FLAG_OFFSET) = 0;			\
} while (0)

#define TX_ISP_ABI_LINK_DETACH(link, source, reverse, sink) do {		\
	(source) = *(unsigned int *)((char *)(link) +			\
		TX_ISP_ABI_LINK_SOURCE_OFFSET);				\
	(reverse) = *(unsigned int *)((char *)(link) +			\
		TX_ISP_ABI_LINK_REVERSE_OFFSET);				\
	(sink) = *(unsigned int *)((char *)(link) +			\
		TX_ISP_ABI_LINK_SINK_OFFSET);				\
	TX_ISP_ABI_LINK_CLEAR_ENDPOINTS(link);				\
} while (0)

/* T23 and T31 share this recovered subdevice prefix. */
#define TX_ISP_ABI_SUBDEV_NAME_OFFSET           0x08
#define TX_ISP_ABI_LEGACY_SUBDEV_IRQDEV_OFFSET  0x80
#define TX_ISP_ABI_LEGACY_SUBDEV_CLOCKS_OFFSET  0xbc
#define TX_ISP_ABI_LEGACY_SUBDEV_CLK_NUM_OFFSET 0xc0
#define TX_ISP_ABI_LEGACY_SUBDEV_OPS_OFFSET     0xc4
#define TX_ISP_ABI_LEGACY_SUBDEV_PAD_COUNT0     0xc8
#define TX_ISP_ABI_LEGACY_SUBDEV_PAD_COUNT1     0xca
#define TX_ISP_ABI_LEGACY_SUBDEV_PAD_PTR0       0xcc
#define TX_ISP_ABI_LEGACY_SUBDEV_PAD_PTR1       0xd0
#define TX_ISP_ABI_LEGACY_SUBDEV_PRIVATE0       0xd4
#define TX_ISP_ABI_LEGACY_SUBDEV_PRIVATE1       0xd8
#define TX_ISP_ABI_LEGACY_SUBDEV_SIZE           0xdc

/* T40 and T41 use the extended subdevice prefix. */
#define TX_ISP_ABI_EXTENDED_SUBDEV_IRQDEV_OFFSET     0x84
#define TX_ISP_ABI_EXTENDED_SUBDEV_RES_OFFSET        0xdc
#define TX_ISP_ABI_EXTENDED_SUBDEV_BASE_OFFSET       0xe8
#define TX_ISP_ABI_EXTENDED_SUBDEV_CLOCKS_OFFSET     0xf4
#define TX_ISP_ABI_EXTENDED_SUBDEV_CLK_NUM_OFFSET    0xf8
#define TX_ISP_ABI_EXTENDED_SUBDEV_OPS_OFFSET        0xfc
#define TX_ISP_ABI_EXTENDED_SUBDEV_PAD_COUNT0        0x100
#define TX_ISP_ABI_EXTENDED_SUBDEV_PAD_COUNT1        0x102
#define TX_ISP_ABI_EXTENDED_SUBDEV_PAD_PTR0           0x104
#define TX_ISP_ABI_EXTENDED_SUBDEV_PAD_PTR1           0x108

#define TX_ISP_ABI_T40_SUBDEV_IRQDEV_OFFSET \
	TX_ISP_ABI_EXTENDED_SUBDEV_IRQDEV_OFFSET
#define TX_ISP_ABI_T40_SUBDEV_RES_OFFSET \
	TX_ISP_ABI_EXTENDED_SUBDEV_RES_OFFSET
#define TX_ISP_ABI_T40_SUBDEV_BASE_OFFSET \
	TX_ISP_ABI_EXTENDED_SUBDEV_BASE_OFFSET
#define TX_ISP_ABI_T40_SUBDEV_CLOCKS_OFFSET \
	TX_ISP_ABI_EXTENDED_SUBDEV_CLOCKS_OFFSET
#define TX_ISP_ABI_T40_SUBDEV_CLK_NUM_OFFSET \
	TX_ISP_ABI_EXTENDED_SUBDEV_CLK_NUM_OFFSET
#define TX_ISP_ABI_T40_SUBDEV_OPS_OFFSET \
	TX_ISP_ABI_EXTENDED_SUBDEV_OPS_OFFSET
#define TX_ISP_ABI_T40_SUBDEV_PAD_COUNT0 \
	TX_ISP_ABI_EXTENDED_SUBDEV_PAD_COUNT0
#define TX_ISP_ABI_T40_SUBDEV_PAD_COUNT1 \
	TX_ISP_ABI_EXTENDED_SUBDEV_PAD_COUNT1
#define TX_ISP_ABI_T40_SUBDEV_PAD_PTR0 \
	TX_ISP_ABI_EXTENDED_SUBDEV_PAD_PTR0
#define TX_ISP_ABI_T40_SUBDEV_PAD_PTR1 \
	TX_ISP_ABI_EXTENDED_SUBDEV_PAD_PTR1

/* File-scope assertion usable by old kernel toolchains without C11. */
#define TX_ISP_ABI_ASSERT(name, expression) \
	typedef char tx_isp_abi_assert_##name[(expression) ? 1 : -1]

#endif /* TX_ISP_SUBDEV_ABI_H */
