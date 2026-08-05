#include <stdio.h>
#include <string.h>

#include "tx_isp/tx_isp_sinfo.h"

static int failures;

#define EXPECT_STATUS(label, expected, config, flags) do { \
	enum tx_isp_sinfo_config_status actual = \
		tx_isp_sinfo_config_check((config), (flags)); \
	if (actual != (expected)) { \
		fprintf(stderr, "%s: got %d expected %d\n", \
			(label), (int)actual, (int)(expected)); \
		failures++; \
	} \
} while (0)

static const struct tx_isp_sinfo_config t23_config = {
	.flags = TX_ISP_SINFO_STATIC_METADATA,
	.static_chip_id = 0x2336,
	.static_width = 1920,
	.static_height = 1080,
	.static_fps = 25,
};

static const struct tx_isp_sinfo_config t31_config = {
	.client_offset = 0x0d4,
	.attr_offset = 0x268,
	.width_offset = 0x234,
	.height_offset = 0x238,
	.fps_offset = 0x274,
	.adapter_nr_offset = 0x0e0,
	.attr_name_offset = 0,
	.attr_chip_id_offset = 4,
};

static const struct tx_isp_sinfo_config t40_config = {
	.flags = TX_ISP_SINFO_REGINFO_WIRING,
	.client_offset = 0x10c,
	.attr_offset = 0x304,
	.width_offset = 0x2d0,
	.height_offset = 0x2d4,
	.fps_offset = 0x314,
	.adapter_nr_offset = 0x190,
	.attr_name_offset = 0,
	.attr_chip_id_offset = 4,
	.info_offset = 0x128,
	.info_mclk_offset = 0x5c,
	.info_boot_offset = 0x60,
	.info_interface_offset = 0x58,
	.info_rst_gpio_offset = 0x48,
	.info_pwdn_gpio_offset = 0x4c,
};

static const struct tx_isp_sinfo_config t41_config = {
	.flags = TX_ISP_SINFO_EXTENDED_ATTRS,
	.static_chip_id = 0x530444,
	.static_width = 2560,
	.static_height = 1440,
	.static_fps = 25,
	.client_offset = 0x10c,
	.attr_offset = 0x308,
	.width_offset = 0x2d4,
	.height_offset = 0x2d8,
	.fps_offset = 0x318,
	.min_fps_offset = 0x31c,
	.max_fps_offset = 0x320,
	.adapter_nr_offset = 0x190,
	.attr_name_offset = 0,
	.attr_chip_id_offset = 4,
	.attr_mclk_offset = 0x184,
	.attr_boot_offset = 0x188,
	.attr_interface_offset = 0x180,
	.attr_rst_gpio_offset = 0x170,
	.attr_pwdn_gpio_offset = 0x174,
};

static void test_real_configs(void)
{
	EXPECT_STATUS("t23", TX_ISP_SINFO_CONFIG_OK, &t23_config,
		      t23_config.flags);
	EXPECT_STATUS("t31", TX_ISP_SINFO_CONFIG_OK, &t31_config,
		      t31_config.flags);
	EXPECT_STATUS("t40", TX_ISP_SINFO_CONFIG_OK, &t40_config,
		      t40_config.flags);
	EXPECT_STATUS("t41 dynamic", TX_ISP_SINFO_CONFIG_OK, &t41_config,
		      t41_config.flags);
	EXPECT_STATUS("t41 static", TX_ISP_SINFO_CONFIG_OK, &t41_config,
		      t41_config.flags | TX_ISP_SINFO_STATIC_METADATA);
}

static void test_invalid_configs(void)
{
	struct tx_isp_sinfo_config config;

	EXPECT_STATUS("null", TX_ISP_SINFO_CONFIG_BAD_FLAGS, NULL, 0);
	EXPECT_STATUS("unknown flag", TX_ISP_SINFO_CONFIG_BAD_FLAGS,
		      &t31_config, 1U << 31);

	config = t23_config;
	config.static_width = 0;
	EXPECT_STATUS("static geometry",
		      TX_ISP_SINFO_CONFIG_BAD_STATIC_METADATA,
		      &config, config.flags);

	config = t31_config;
	config.client_offset++;
	EXPECT_STATUS("object pointer", TX_ISP_SINFO_CONFIG_BAD_OBJECT_OFFSETS,
		      &config, config.flags);

	config = t31_config;
	config.attr_chip_id_offset++;
	EXPECT_STATUS("attribute", TX_ISP_SINFO_CONFIG_BAD_ATTRIBUTE_OFFSETS,
		      &config, config.flags);

	config = t41_config;
	config.min_fps_offset++;
	EXPECT_STATUS("extended fps",
		      TX_ISP_SINFO_CONFIG_BAD_EXTENDED_OFFSETS,
		      &config, config.flags);

	config = t41_config;
	config.attr_mclk_offset++;
	EXPECT_STATUS("extended wiring",
		      TX_ISP_SINFO_CONFIG_BAD_EXTENDED_OFFSETS,
		      &config, config.flags);

	config = t40_config;
	config.info_offset++;
	EXPECT_STATUS("register info",
		      TX_ISP_SINFO_CONFIG_BAD_REGINFO_OFFSETS,
		      &config, config.flags);
}

static void test_policy_selects_only_dereferenced_offsets(void)
{
	struct tx_isp_sinfo_config config = t41_config;

	config.client_offset++;
	config.attr_mclk_offset++;
	EXPECT_STATUS("static skips dynamic object and wiring",
		      TX_ISP_SINFO_CONFIG_OK, &config,
		      config.flags | TX_ISP_SINFO_STATIC_METADATA);

	config = t40_config;
	config.attr_mclk_offset++;
	EXPECT_STATUS("reginfo skips attribute wiring",
		      TX_ISP_SINFO_CONFIG_OK, &config, config.flags);

	config = t41_config;
	config.attr_mclk_offset++;
	config.info_offset = 1;
	EXPECT_STATUS("combined policy selects reginfo",
		      TX_ISP_SINFO_CONFIG_BAD_REGINFO_OFFSETS, &config,
		      config.flags | TX_ISP_SINFO_REGINFO_WIRING);
}

int main(void)
{
	test_real_configs();
	test_invalid_configs();
	test_policy_selects_only_dereferenced_offsets();

	if (failures) {
		fprintf(stderr, "tx_isp_sinfo ABI: %d failure(s)\n", failures);
		return 1;
	}

	puts("tx_isp_sinfo ABI: all tests passed");
	return 0;
}
