#ifndef TX_ISP_STATE_H
#define TX_ISP_STATE_H

/*
 * Evaluate the common recovered subdevice state policy from adapter-supplied
 * values. Object layout and field width remain generation-local.
 */
int tx_isp_subdev_state_ready(
	unsigned long object,
	unsigned long queue_next,
	unsigned long queue_self,
	unsigned int state);

#endif /* TX_ISP_STATE_H */
