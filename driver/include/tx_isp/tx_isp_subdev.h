#ifndef TX_ISP_SUBDEV_H
#define TX_ISP_SUBDEV_H

/*
 * Generation-neutral description of one endpoint in the TX-ISP graph.
 * OEM link records encode output as type 1 and input as type 2.
 */
#define TX_ISP_SUBDEV_PAD_OUTPUT 1U
#define TX_ISP_SUBDEV_PAD_INPUT  2U

/*
 * Compiler-independent MIPS32 graph record ABI. The two bytes after index are
 * alignment padding in the vendor structure and are retained explicitly.
 */
#define TX_ISP_SUBDEV_DESC_NAME_OFFSET       0x00U
#define TX_ISP_SUBDEV_DESC_TYPE_OFFSET       0x04U
#define TX_ISP_SUBDEV_DESC_INDEX_OFFSET      0x05U
#define TX_ISP_SUBDEV_DESC_SIZE              0x08U

#define TX_ISP_SUBDEV_LINK_SOURCE_OFFSET     0x00U
#define TX_ISP_SUBDEV_LINK_SINK_OFFSET       0x08U
#define TX_ISP_SUBDEV_LINK_FLAG_OFFSET       0x10U
#define TX_ISP_SUBDEV_LINK_CONFIG_SIZE       0x14U

#define TX_ISP_SUBDEV_LINK_SET_RECORDS_OFFSET 0x00U
#define TX_ISP_SUBDEV_LINK_SET_COUNT_OFFSET   0x04U
#define TX_ISP_SUBDEV_LINK_SET_SIZE           0x08U

#define TX_ISP_SUBDEV_LINK_ENABLED             0x01U

struct tx_isp_subdev_pad_descriptor32 {
	unsigned int name;
	unsigned char type;
	unsigned char index;
	unsigned char reserved[2];
};

struct tx_isp_subdev_link_config32 {
	struct tx_isp_subdev_pad_descriptor32 source;
	struct tx_isp_subdev_pad_descriptor32 sink;
	unsigned int flag;
};

struct tx_isp_subdev_link_set32 {
	unsigned int records;
	unsigned int count;
};

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

enum tx_isp_subdev_link_status {
	TX_ISP_SUBDEV_LINK_OK = 0,
	TX_ISP_SUBDEV_LINK_INVALID,
	TX_ISP_SUBDEV_LINK_TYPE_MISMATCH,
	TX_ISP_SUBDEV_LINK_BUSY,
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

/*
 * Validate the generation-neutral portion of a link transition. Teardown
 * callbacks and their ordering remain local to each SoC.
 */
enum tx_isp_subdev_link_status tx_isp_subdev_validate_link(
	unsigned int source_links_type,
	unsigned int sink_links_type,
	unsigned int source_state,
	unsigned int sink_state,
	unsigned int flags,
	unsigned int *enabled_flags);

/* Initialize or connect the common 32-bit pad/link prefix in recovered order. */
void tx_isp_subdev_init_link_record(void *pad);
void tx_isp_subdev_connect_link_pair(
	void *source_pad,
	void *sink_pad,
	unsigned int flags);

/*
 * Recovered T23/T41 teardown clears the endpoint/reverse/flag prefix while
 * deliberately leaving link state alone. Callers continue to own pad state
 * and the generation-local callback order around this operation.
 */
void tx_isp_subdev_clear_link_endpoints(void *link);
void tx_isp_subdev_detach_link(
	void *link,
	unsigned int *source,
	unsigned int *reverse,
	unsigned int *sink);

#endif /* TX_ISP_SUBDEV_H */
