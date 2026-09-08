#include <errno.h>
#include <stdio.h>
#include <string.h>

#include "../driver/t23/tx_isp_t23_scaler.h"
#include "../driver/t41/tx_isp_t41_scaler.h"

static int failures;

#define EXPECT_TRUE(condition)                                                   \
	do {                                                                      \
		if (!(condition)) {                                                \
			fprintf(stderr, "%s:%d: expectation failed: %s\n",         \
				__FILE__, __LINE__, #condition);                    \
			++failures;                                                \
		}                                                                 \
	} while (0)

static void expect_t41_curve(u32 target, u32 source, const s16 *expected)
{
	s16 actual[TX_ISP_T41_SCALER_COEFFICIENTS];
	int ret;

	memset(actual, 0x5a, sizeof(actual));
	ret = tx_isp_t41_scaler_curve_generate(
		target, source, actual,
		TX_ISP_T41_SCALER_COEFFICIENTS);
	EXPECT_TRUE(ret == 0);
	EXPECT_TRUE(memcmp(actual, expected, sizeof(actual)) == 0);
}

static void test_exact_stock_unity_curve(void)
{
	static const s16 expected[TX_ISP_T41_SCALER_COEFFICIENTS] = {
		2048, 1984, 1801, 1518, 1178, 819, 483, 203,
		0, -123, -174, -169, -131, -80, -36, -9,
		0, -7, -23, -37, -47, -47, -36, -19,
		0, 16, 25, 28, 24, 16, 8, 3, 0,
	};

	expect_t41_curve(2560, 2560, expected);
}

static void test_t41_main_downscale_curve(void)
{
	static const s16 expected[TX_ISP_T41_SCALER_COEFFICIENTS] = {
		1551, 1530, 1453, 1327, 1161, 967, 759, 554,
		365, 203, 70, -30, -94, -128, -134, -123,
		-99, -71, -44, -21, -7, 0, -1, -8,
		-17, -26, -34, -37, -36, -30, -21, -11, -1,
	};

	expect_t41_curve(1920, 2560, expected);
}

static void expect_t23_curve(u32 ratio_q14, const s16 *expected)
{
	s16 actual[TX_ISP_T23_SCALER_COEFFICIENTS];
	int ret;

	memset(actual, 0x5a, sizeof(actual));
	ret = tx_isp_t23_scaler_curve_generate(
		ratio_q14, actual, TX_ISP_T23_SCALER_COEFFICIENTS);
	EXPECT_TRUE(ret == 0);
	EXPECT_TRUE(memcmp(actual, expected, sizeof(actual)) == 0);
}

static void test_exact_t23_curves(void)
{
	static const s16 unity[TX_ISP_T23_SCALER_COEFFICIENTS] = {
		2048, 1977, 1779, 1489, 1152, 803, 477, 202,
		0, -122, -172, -165, -128, -79, -36, -9, 0,
	};
	static const s16 third[TX_ISP_T23_SCALER_COEFFICIENTS] = {
		701, 687, 672, 655, 637, 618, 598, 576,
		554, 510, 467, 426, 387, 349, 311, 275, 239,
	};
	u32 entry_count = 0;

	expect_t23_curve(0x4000, unity);
	expect_t23_curve(0x1555, third);
	EXPECT_TRUE(tx_isp_t23_scaler_sinc_lut_get(&entry_count) != NULL);
	EXPECT_TRUE(entry_count == 257);
}

static void test_failure_does_not_publish(void)
{
	s16 actual[TX_ISP_T41_SCALER_COEFFICIENTS];
	s16 before[TX_ISP_T41_SCALER_COEFFICIENTS];
	int ret;

	memset(actual, 0x3c, sizeof(actual));
	memcpy(before, actual, sizeof(before));
	ret = tx_isp_t41_scaler_curve_generate(
		1920, 2560, actual,
		TX_ISP_T41_SCALER_COEFFICIENTS - 1);
	EXPECT_TRUE(ret == -EINVAL);
	EXPECT_TRUE(memcmp(actual, before, sizeof(actual)) == 0);

	ret = tx_isp_t41_scaler_curve_generate(
		0, 2560, actual, TX_ISP_T41_SCALER_COEFFICIENTS);
	EXPECT_TRUE(ret == -EINVAL);
	EXPECT_TRUE(memcmp(actual, before, sizeof(actual)) == 0);
}

static unsigned int writes;
static int port_error;

static int count_write(void *context, u32 reg, u32 value)
{
	(void)context;
	(void)value;
	EXPECT_TRUE(reg == 0xf8004);
	++writes;
	return port_error;
}

static void test_channel_curves_and_writer(void)
{
	s16 curves[258], before[258];
	u8 params[TX_ISP_T41_SCALER_PARAMS_BYTES] = {0};
	u32 channel, phase, tap;

	for (channel = 0; channel < 3; ++channel) {
		u32 stride = channel == 1 ? 64 : 8;
		u32 count = stride * 4 + 1;
		int pairs;

		memset(curves, 0x5a, sizeof(curves));
		EXPECT_TRUE(tx_isp_t41_scaler_channel_curve_generate(
			channel, 640, 2560, curves, count) == 0);
		EXPECT_TRUE(curves[count] == 0x5a5a);
		for (phase = 0; phase < stride; ++phase) {
			int sum = 0;

			for (tap = 0; tap < 8; ++tap) {
				int index = ((int)tap - 3) * (int)stride - (int)phase;

				sum += curves[index < 0 ? -index : index];
			}
			/* Phase zero aliases symmetric taps; the OEM allows +/-1. */
			EXPECT_TRUE(phase ? sum == 2048 : sum >= 2047 && sum <= 2049);
		}
		writes = 0;
		pairs = tx_isp_t41_scaler_curve_write(channel, params, sizeof(params),
			curves, curves, count, count_write, NULL);
		EXPECT_TRUE(pairs == (channel == 1 ? 266 : 34));
		EXPECT_TRUE(writes == (u32)pairs * 2);
	}

	memcpy(before, curves, sizeof(before));
	EXPECT_TRUE(tx_isp_t41_scaler_channel_curve_generate(1, 640, 2560,
		curves, 256) == -EINVAL);
	EXPECT_TRUE(tx_isp_t41_scaler_channel_curve_generate(3, 640, 2560,
		curves, 257) == -EINVAL);
	EXPECT_TRUE(tx_isp_t41_scaler_channel_curve_generate(1, 65536, 2560,
		curves, 257) == -EINVAL);
	EXPECT_TRUE(tx_isp_t41_scaler_channel_curve_generate(1, 640, 0,
		curves, 257) == -EINVAL);
	EXPECT_TRUE(tx_isp_t41_scaler_channel_curve_generate(1, 640, 2560,
		NULL, 257) == -EINVAL);
	EXPECT_TRUE(memcmp(before, curves, sizeof(curves)) == 0);
	writes = 0;
	EXPECT_TRUE(tx_isp_t41_scaler_curve_write(1, params, sizeof(params) - 1,
		curves, curves, 257, count_write, NULL) == -EINVAL);
	EXPECT_TRUE(tx_isp_t41_scaler_curve_write(1, params, sizeof(params),
		curves, curves, 256, count_write, NULL) == -EINVAL);
	EXPECT_TRUE(tx_isp_t41_scaler_curve_write(3, params, sizeof(params),
		curves, curves, 257, count_write, NULL) == -EINVAL);
	EXPECT_TRUE(tx_isp_t41_scaler_curve_write(1, NULL, sizeof(params),
		curves, curves, 257, count_write, NULL) == -EINVAL);
	EXPECT_TRUE(tx_isp_t41_scaler_curve_write(1, params, sizeof(params),
		NULL, curves, 257, count_write, NULL) == -EINVAL);
	EXPECT_TRUE(tx_isp_t41_scaler_curve_write(1, params, sizeof(params),
		curves, curves, 257, NULL, NULL) == -EINVAL);
	EXPECT_TRUE(writes == 0);
	port_error = -EIO;
	EXPECT_TRUE(tx_isp_t41_scaler_curve_write(1, params, sizeof(params),
		curves, curves, 257, count_write, NULL) == -EIO);
	EXPECT_TRUE(writes == 1);
	port_error = 0;
}

int main(void)
{
	test_exact_stock_unity_curve();
	test_t41_main_downscale_curve();
	test_exact_t23_curves();
	test_failure_does_not_publish();
	test_channel_curves_and_writer();
	if (failures)
		return 1;
	puts("tx_isp_scaler_test: ok");
	return 0;
}
