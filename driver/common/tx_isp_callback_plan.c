/*
 * Shared ordered callback plans for ISP initialization and tuning sequences.
 */

#ifdef __KERNEL__
#include <linux/errno.h>
#else
#include <errno.h>
#endif

#include "../include/tx_isp/tx_isp_callback_plan.h"

int tx_isp_callback_plan_run(const struct tx_isp_callback_plan *plan,
			     void *opaque)
{
	unsigned int i;

	if (!plan)
		return -EINVAL;
	if (plan->count && !plan->steps)
		return -EINVAL;

	for (i = 0; i < plan->count; ++i) {
		if (!plan->steps[i].run)
			return -EINVAL;
	}

	for (i = 0; i < plan->count; ++i)
		plan->steps[i].run(opaque);

	return 0;
}
