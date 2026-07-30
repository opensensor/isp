/*
 * Shared ordered register-profile application.
 *
 * Sensor/SoC adapters own the actual tables and MMIO primitive.  Keeping the
 * ordering and shadow-commit contract here avoids open-coded register loops
 * in individual ISP blocks.
 */

#ifdef __KERNEL__
#include <linux/errno.h>
#else
#include <errno.h>
#endif

#include "../include/tx_isp/tx_isp_reg_profile.h"

int tx_isp_reg_profile_apply(const struct tx_isp_reg_profile *profile,
			     tx_isp_reg_profile_write_fn write,
			     void *opaque)
{
	unsigned int i;

	if (!profile || !write)
		return -EINVAL;
	if (profile->count && !profile->values)
		return -EINVAL;

	for (i = 0; i < profile->count; ++i)
		write(opaque, profile->values[i].reg,
		      profile->values[i].value);

	if (profile->has_commit)
		write(opaque, profile->commit_reg, profile->commit_value);

	return 0;
}

u32 tx_isp_reg_flags_merge(u32 value, const u32 *flags,
			   unsigned int count)
{
	unsigned int i;

	if (!flags)
		return value;
	if (count > 32U)
		count = 32U;

	for (i = 0; i < count; ++i)
		value = (value & ~(1U << i)) | (flags[i] << i);

	return value;
}
