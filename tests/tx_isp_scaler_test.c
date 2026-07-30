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

int main(void)
{
	test_exact_stock_unity_curve();
	test_t41_main_downscale_curve();
	test_exact_t23_curves();
	test_failure_does_not_publish();
	if (failures)
		return 1;
	puts("tx_isp_scaler_test: ok");
	return 0;
}
