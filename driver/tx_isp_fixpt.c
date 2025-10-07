#include <linux/types.h>
#include <linux/module.h>
#include "include/tx_isp_fixpt.h"

/* Non-inline wrappers in case other compilation units want linkable symbols. */

u32 tx_fix_point_mult2_32(u32 q, u32 a, u32 b)
{
    return fix_point_mult2_32(q, a, b);
}
EXPORT_SYMBOL_GPL(tx_fix_point_mult2_32);

u32 tx_fix_point_mult3_32(u32 q, u32 a, u32 b, u32 c)
{
    return fix_point_mult3_32(q, a, b, c);
}
EXPORT_SYMBOL_GPL(tx_fix_point_mult3_32);

u32 tx_fix_point_div_32(u32 q, u32 num, u32 den)
{
    return fix_point_div_32(q, num, den);
}
EXPORT_SYMBOL_GPL(tx_fix_point_div_32);

