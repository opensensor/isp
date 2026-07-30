#ifdef __KERNEL__
#include <linux/errno.h>
#include <linux/string.h>
#include <linux/types.h>
#else
#include <errno.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
typedef int32_t s32;
typedef uint64_t u64;
#endif

#include "../include/tx_isp/tx_isp_scaler.h"

#define TX_ISP_SCALER_GAIN_UNITY_Q11	2048
#define TX_ISP_SCALER_RATIO_MAX_Q14	27306U

static s32 tx_isp_scaler_abs(s32 value)
{
	return value < 0 ? -value : value;
}

static s32 tx_isp_scaler_shift_right_floor(s32 value, u32 shift)
{
	s32 divisor = (s32)(1U << shift);

	if (value >= 0)
		return value / divisor;
	return -((-value + divisor - 1) / divisor);
}

int tx_isp_scaler_curve_generate(const struct tx_isp_scaler_kernel *kernel,
				 u32 ratio_q14, s16 *coefficients,
				 u32 coefficient_capacity)
{
	s16 samples[TX_ISP_SCALER_MAX_PHASES + 1];
	s16 result[TX_ISP_SCALER_MAX_PHASES + 1];
	s32 weights[TX_ISP_SCALER_MAX_PHASES + 1];
	s32 positions[TX_ISP_SCALER_MAX_TAPS];
	s32 phase_coefficients[TX_ISP_SCALER_MAX_TAPS];
	u32 step;
	u32 phase;
	u32 tap;

	if (!kernel || !coefficients || !ratio_q14 ||
	    ratio_q14 > TX_ISP_SCALER_RATIO_MAX_Q14 ||
	    !kernel->sinc_lut || kernel->sinc_lut_count < 2 ||
	    !kernel->add_order || !kernel->subtract_order ||
	    !kernel->phase_count ||
	    kernel->phase_count > TX_ISP_SCALER_MAX_PHASES ||
	    !kernel->phase_stride ||
	    kernel->phase_stride > kernel->phase_count ||
	    kernel->tap_count < 2 ||
	    kernel->tap_count > TX_ISP_SCALER_MAX_TAPS ||
	    (kernel->tap_count & 1U) ||
	    coefficient_capacity < kernel->phase_count + 1)
		return -EINVAL;

	for (tap = 0; tap < kernel->tap_count; ++tap) {
		if (kernel->add_order[tap] >= kernel->tap_count ||
		    kernel->subtract_order[tap] >= kernel->tap_count)
			return -EINVAL;
	}

	step = (ratio_q14 << 3) / kernel->phase_count;
	if (!step)
		return -ERANGE;
	memset(samples, 0, sizeof(samples));
	memset(weights, 0, sizeof(weights));

	for (phase = 0; phase <= kernel->phase_count; ++phase) {
		u64 position64 = (u64)phase * step;
		u32 position;
		u32 lut_index;
		u32 fraction;
		s32 value;

		if (position64 > (u32)~0U)
			return -ERANGE;
		position = (u32)position64;
		lut_index = position >> 10;
		if (lut_index + 1 >= kernel->sinc_lut_count)
			return -ERANGE;
		fraction = position & 0x3ffU;
		value = kernel->sinc_lut[lut_index + 1] * (s32)fraction;
		value += kernel->sinc_lut[lut_index] *
			 (s32)(0x400U - fraction);
		samples[phase] = (s16)(
			tx_isp_scaler_shift_right_floor(value, 10) +
			((u32)tx_isp_scaler_shift_right_floor(value, 9) & 1U));
	}

	for (tap = 0; tap < kernel->tap_count; ++tap) {
		positions[tap] =
			-((s32)(kernel->tap_count / 2U) - 1) *
			(s32)kernel->phase_stride +
			(s32)tap * (s32)kernel->phase_stride;
		if (tx_isp_scaler_abs(positions[tap]) >
		    (s32)kernel->phase_count)
			return -ERANGE;
	}

	for (phase = 0; phase < kernel->phase_stride; ++phase) {
		s32 sum = 0;
		s32 half;
		s32 total = 0;
		s32 delta;
		u32 adjustment;

		for (tap = 0; tap < kernel->tap_count; ++tap) {
			u32 index = (u32)tx_isp_scaler_abs(
				positions[tap] - (s32)phase);

			if (index > kernel->phase_count)
				return -ERANGE;
			sum += samples[index];
		}
		if (!sum)
			return -ERANGE;
		half = sum / 2;

		for (tap = 0; tap < kernel->tap_count; ++tap) {
			u32 index = (u32)tx_isp_scaler_abs(
				positions[tap] - (s32)phase);
			s32 sample = samples[index];
			s32 numerator = sample * TX_ISP_SCALER_GAIN_UNITY_Q11;
			s32 coefficient;

			numerator += sample < 0 ? -half : half;
			coefficient = numerator / sum;
			weights[index] = coefficient;
			phase_coefficients[tap] = coefficient;
			total += coefficient;
		}

		delta = total - TX_ISP_SCALER_GAIN_UNITY_Q11;
		for (adjustment = 0;
		     adjustment < (u32)tx_isp_scaler_abs(delta);
		     ++adjustment) {
			u32 order_index = adjustment % kernel->tap_count;
			u32 coefficient_index =
				delta < 0 ? kernel->add_order[order_index] :
					    kernel->subtract_order[order_index];

			phase_coefficients[coefficient_index] +=
				delta < 0 ? 1 : -1;
		}

		for (tap = 0; tap < kernel->tap_count; ++tap) {
			u32 index = (u32)tx_isp_scaler_abs(
				positions[tap] - (s32)phase);

			weights[index] = phase_coefficients[tap];
		}
	}

	for (phase = 0; phase <= kernel->phase_count; ++phase) {
		if (weights[phase] < -32768 || weights[phase] > 32767)
			return -ERANGE;
		result[phase] = (s16)weights[phase];
	}
	memcpy(coefficients, result,
	       (kernel->phase_count + 1U) * sizeof(*coefficients));
	return 0;
}
