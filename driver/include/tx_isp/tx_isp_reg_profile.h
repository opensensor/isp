#ifndef TX_ISP_REG_PROFILE_H
#define TX_ISP_REG_PROFILE_H

#ifdef __KERNEL__
#include <linux/types.h>
#else
#include <stdbool.h>
#include <stdint.h>
typedef uint32_t u32;
#endif

struct tx_isp_reg_value {
	u32 reg;
	u32 value;
};

struct tx_isp_reg_profile {
	const struct tx_isp_reg_value *values;
	unsigned int count;
	u32 commit_reg;
	u32 commit_value;
	bool has_commit;
};

typedef void (*tx_isp_reg_profile_write_fn)(void *opaque, u32 reg, u32 value);

/*
 * Apply an ordered register profile and, when requested, latch it with one
 * final shadow-commit write.
 */
int tx_isp_reg_profile_apply(const struct tx_isp_reg_profile *profile,
			     tx_isp_reg_profile_write_fn write,
			     void *opaque);

/*
 * Replace the low 32 register-mask bits with their corresponding parameter
 * flags.  Values are deliberately not coerced to boolean: recovered tuning
 * banks occasionally rely on the original shift-and-merge semantics.
 */
u32 tx_isp_reg_flags_merge(u32 value, const u32 *flags,
			   unsigned int count);

#endif /* TX_ISP_REG_PROFILE_H */
