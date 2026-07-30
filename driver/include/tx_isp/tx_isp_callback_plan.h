#ifndef TX_ISP_CALLBACK_PLAN_H
#define TX_ISP_CALLBACK_PLAN_H

typedef void (*tx_isp_callback_plan_fn)(void *opaque);

struct tx_isp_callback_step {
	tx_isp_callback_plan_fn run;
};

struct tx_isp_callback_plan {
	const struct tx_isp_callback_step *steps;
	unsigned int count;
};

/*
 * Validate the complete plan, then invoke every callback in declaration
 * order.  Pre-validation prevents a malformed plan from partially programming
 * hardware.
 */
int tx_isp_callback_plan_run(const struct tx_isp_callback_plan *plan,
			     void *opaque);

#endif /* TX_ISP_CALLBACK_PLAN_H */
