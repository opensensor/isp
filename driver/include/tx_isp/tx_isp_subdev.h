#ifndef TX_ISP_SUBDEV_H
#define TX_ISP_SUBDEV_H

/*
 * Generation-neutral description of one endpoint in the TX-ISP graph.
 * OEM link records encode output as type 1 and input as type 2.
 */
#define TX_ISP_SUBDEV_PAD_OUTPUT 1U
#define TX_ISP_SUBDEV_PAD_INPUT  2U

struct tx_isp_subdev_pad_descriptor {
	const char *name;
	unsigned char type;
	unsigned char index;
};

enum tx_isp_subdev_resolve_status {
	TX_ISP_SUBDEV_RESOLVE_OK = 0,
	TX_ISP_SUBDEV_RESOLVE_INVALID,
	TX_ISP_SUBDEV_RESOLVE_NOT_FOUND,
	TX_ISP_SUBDEV_RESOLVE_BAD_TYPE,
	TX_ISP_SUBDEV_RESOLVE_NO_PADS,
	TX_ISP_SUBDEV_RESOLVE_INDEX_RANGE,
};

/*
 * The search algorithm is common; object layout and pointer validation are
 * generation-specific. Accessors must return NULL for unusable objects.
 */
struct tx_isp_subdev_resolver_ops {
	unsigned int subdev_count;
	unsigned int pad_stride;
	void *(*subdev_at)(void *graph, unsigned int index);
	const char *(*subdev_name)(void *subdev);
	unsigned int (*pad_count)(void *subdev, unsigned int type);
	void *(*pad_base)(void *subdev, unsigned int type);
};

void *tx_isp_subdev_resolve_pad(
	void *graph,
	const struct tx_isp_subdev_pad_descriptor *descriptor,
	const struct tx_isp_subdev_resolver_ops *ops,
	enum tx_isp_subdev_resolve_status *status);

#endif /* TX_ISP_SUBDEV_H */
