#ifndef TX_ISP_SCALER_H
#define TX_ISP_SCALER_H

#ifdef __KERNEL__
#include <linux/types.h>
#else
#include <stdint.h>
typedef int16_t s16;
typedef uint8_t u8;
typedef uint32_t u32;
#endif

#define TX_ISP_SCALER_MAX_PHASES	256U
#define TX_ISP_SCALER_MAX_TAPS		8U

/*
 * Describe one generation's fixed-point polyphase scaler kernel.
 *
 * sinc_lut is sampled in 1/1024 intervals.  phase_count controls the number
 * of stored coefficients, phase_stride is the number of fractional phases
 * normalized independently, and tap_count is the number of coefficients
 * contributing to each phase.  The correction orders distribute integer
 * rounding error until every phase sums to Q11 unity.
 */
struct tx_isp_scaler_kernel {
	const s16 *sinc_lut;
	u32 sinc_lut_count;
	const u8 *add_order;
	const u8 *subtract_order;
	u32 phase_count;
	u32 phase_stride;
	u32 tap_count;
};

/*
 * Generate phase_count + 1 signed Q11 coefficients for ratio_q14.
 *
 * The routine mirrors the vendor scaler's integer interpolation,
 * normalization, and deterministic rounding.  It validates the complete
 * kernel description and publishes no partial result on failure.
 */
int tx_isp_scaler_curve_generate(const struct tx_isp_scaler_kernel *kernel,
				 u32 ratio_q14, s16 *coefficients,
				 u32 coefficient_capacity);

#endif /* TX_ISP_SCALER_H */
