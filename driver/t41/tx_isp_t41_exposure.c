/*
 * T41 exposure policy adapter.
 *
 * The checked exposure arithmetic is shared with the other SoCs.  Hardware
 * register ownership stays here: OS04D10 needs pre-tone-map attenuation to
 * use a 1/120 second shutter in a bright room, and the otherwise bypassed CCM
 * gives us a neutral-preserving place to remove the remaining magenta error.
 */
#include <linux/bitops.h>
#include <linux/errno.h>
#include <linux/types.h>

#include "tx_isp_t41_exposure.h"

extern int32_t system_reg_write(uint32_t reg, uint32_t value);

#include "../common/tx_isp_exposure.c"

#define T41_TOP_CCM_BYPASS		BIT(9)
#define T41_GIB_GAIN_UNITY_Q10		0x400U
#define T41_COLOR_MATRIX_UNITY_Q10	0x400U
#define T41_COLOR_MATRIX_MASK		0x3fffU

static u32 tx_isp_t41_ccm_pack(s32 high, s32 low)
{
	return ((u32)high & T41_COLOR_MATRIX_MASK) << 16 |
	       ((u32)low & T41_COLOR_MATRIX_MASK);
}

static int tx_isp_t41_correction_validate(u32 correction_q10)
{
	/*
	 * The correction matrix contains both -correction and
	 * unity+correction in signed 14-bit fields.
	 */
	if (correction_q10 > 0x1bffU)
		return -ERANGE;
	return 0;
}

int tx_isp_t41_flicker_profile_apply(u32 channel, bool enable,
				     u32 gib_gain_q10,
				     u32 green_correction_q10,
				     u32 blue_correction_q10,
				     u32 *top_bypass)
{
	s32 green;
	s32 blue;
	u32 packed_gib;
	int ret;

	if (channel || !top_bypass)
		return -EINVAL;
	if (gib_gain_q10 > T41_COLOR_MATRIX_MASK)
		return -ERANGE;
	ret = tx_isp_t41_correction_validate(green_correction_q10);
	if (ret)
		return ret;
	ret = tx_isp_t41_correction_validate(blue_correction_q10);
	if (ret)
		return ret;

	if (!enable) {
		packed_gib = T41_GIB_GAIN_UNITY_Q10 << 16 |
			     T41_GIB_GAIN_UNITY_Q10;
		system_reg_write(0x08000, packed_gib);
		system_reg_write(0x08004, packed_gib);
		system_reg_write(0x08040, 1);
		*top_bypass |= T41_TOP_CCM_BYPASS;
		system_reg_write((channel + 16U) << 2, *top_bypass);
		return 0;
	}
	if (!gib_gain_q10)
		return -EINVAL;

	packed_gib = gib_gain_q10 << 16 | gib_gain_q10;
	system_reg_write(0x08000, packed_gib);
	system_reg_write(0x08004, packed_gib);
	system_reg_write(0x08040, 1);

	/*
	 * Row sums stay at Q10 unity:
	 *
	 *   R' = R
	 *   G' = G - green * (R - G)
	 *   B' = B - blue  * (R - G)
	 *
	 * Pure blue remains blue and neutrals remain neutral.  This is important:
	 * a convex G-to-B blend made the calibration wall neutral only by
	 * collapsing the blue gamut and its chroma detail.  The measured T41
	 * day profile instead removes the OS04D10's red-dependent magenta error.
	 */
	green = (s32)green_correction_q10;
	blue = (s32)blue_correction_q10;
	system_reg_write(0x0b004,
			 tx_isp_t41_ccm_pack(0, T41_COLOR_MATRIX_UNITY_Q10));
	system_reg_write(0x0b008, tx_isp_t41_ccm_pack(-green, 0));
	system_reg_write(0x0b00c,
			 tx_isp_t41_ccm_pack(
				 0, T41_COLOR_MATRIX_UNITY_Q10 + green));
	system_reg_write(0x0b010,
			 tx_isp_t41_ccm_pack(blue, -blue));
	system_reg_write(0x0b014, T41_COLOR_MATRIX_UNITY_Q10);
	system_reg_write(0x0b018, 0x00041008);
	system_reg_write(0x0b01c, 0x00000008);
	system_reg_write(0x0b020, 0x0fff00ff);
	system_reg_write(0x0b024, 0x00080000);
	system_reg_write(0x0b028, 0x00010001);
	system_reg_write(0x0b000, 1);

	*top_bypass &= ~T41_TOP_CCM_BYPASS;
	system_reg_write((channel + 16U) << 2, *top_bypass);
	return 0;
}
