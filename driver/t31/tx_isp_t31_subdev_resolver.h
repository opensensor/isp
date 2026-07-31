#ifndef TX_ISP_T31_SUBDEV_RESOLVER_H
#define TX_ISP_T31_SUBDEV_RESOLVER_H

#include "../include/tx_isp/tx_isp_subdev.h"
#include "include/tx_isp.h"
#include "include/tx_isp_device.h"

struct tx_isp_subdev_pad *tx_isp_t31_resolve_link_pad(
	struct tx_isp_dev *isp_dev,
	const struct link_pad_description *descriptor,
	enum tx_isp_subdev_resolve_status *status);

#endif /* TX_ISP_T31_SUBDEV_RESOLVER_H */
