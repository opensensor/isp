#ifndef TX_ISP_SINFO_H
#define TX_ISP_SINFO_H

struct i2c_driver;
struct module;

#define TX_ISP_SINFO_STATIC_METADATA	(1U << 0)
#define TX_ISP_SINFO_EXTENDED_ATTRS	(1U << 1)
#define TX_ISP_SINFO_REGINFO_WIRING	(1U << 2)
#define TX_ISP_SINFO_VALID_FLAGS \
	(TX_ISP_SINFO_STATIC_METADATA | TX_ISP_SINFO_EXTENDED_ATTRS | \
	 TX_ISP_SINFO_REGINFO_WIRING)

/*
 * Binary sensor modules pass private objects whose layouts differ by SoC.
 * Each TX-ISP module supplies one configuration before including the common
 * implementation. Zero offsets are valid, so flags select static metadata
 * rather than using offsets as presence indicators.
 */
struct tx_isp_sinfo_config {
	unsigned int flags;

	unsigned int client_offset;
	unsigned int attr_offset;
	unsigned int width_offset;
	unsigned int height_offset;
	unsigned int fps_offset;
	unsigned int min_fps_offset;
	unsigned int max_fps_offset;
	unsigned int adapter_nr_offset;

	unsigned int attr_name_offset;
	unsigned int attr_chip_id_offset;
	unsigned int attr_mclk_offset;
	unsigned int attr_boot_offset;
	unsigned int attr_interface_offset;
	unsigned int attr_rst_gpio_offset;
	unsigned int attr_pwdn_gpio_offset;

	unsigned int info_offset;
	unsigned int info_mclk_offset;
	unsigned int info_boot_offset;
	unsigned int info_interface_offset;
	unsigned int info_rst_gpio_offset;
	unsigned int info_pwdn_gpio_offset;

	unsigned int static_chip_id;
	unsigned int static_i2c_adapter;
	unsigned int static_width;
	unsigned int static_height;
	unsigned int static_fps;

	void (*driver_added)(struct i2c_driver *drv, int default_i2c_addr,
			     struct module *owner);
	void (*driver_removing)(struct i2c_driver *drv);
	void (*sensor_bound)(void *subdev, struct module *owner);
	void (*sensor_unbound)(void *subdev, struct module *owner);
	int (*read_module_param_int)(struct module *owner, const char *name,
				     int *value);
};

enum tx_isp_sinfo_config_status {
	TX_ISP_SINFO_CONFIG_OK = 0,
	TX_ISP_SINFO_CONFIG_BAD_FLAGS,
	TX_ISP_SINFO_CONFIG_BAD_STATIC_METADATA,
	TX_ISP_SINFO_CONFIG_BAD_OBJECT_OFFSETS,
	TX_ISP_SINFO_CONFIG_BAD_ATTRIBUTE_OFFSETS,
	TX_ISP_SINFO_CONFIG_BAD_EXTENDED_OFFSETS,
	TX_ISP_SINFO_CONFIG_BAD_REGINFO_OFFSETS,
};

static inline int tx_isp_sinfo_offsets_are_u32_aligned(
	const unsigned int *offsets, unsigned int count)
{
	unsigned int i;

	for (i = 0; i < count; ++i)
		if (offsets[i] & 3U)
			return 0;
	return 1;
}

/*
 * Check the offset-addressed ABI before common registry state is allocated.
 * Zero is intentionally accepted: several real structures place a field at
 * their base. config_flags is separate because T41 selects static metadata
 * through a layout-preserving runtime expression.
 */
static inline enum tx_isp_sinfo_config_status
tx_isp_sinfo_config_check(const struct tx_isp_sinfo_config *config,
			   unsigned int config_flags)
{
	unsigned int offsets[7];

	if (!config || (config_flags & ~TX_ISP_SINFO_VALID_FLAGS))
		return TX_ISP_SINFO_CONFIG_BAD_FLAGS;

	if (config_flags & TX_ISP_SINFO_STATIC_METADATA) {
		if (!config->static_width || !config->static_height ||
		    !config->static_fps)
			return TX_ISP_SINFO_CONFIG_BAD_STATIC_METADATA;
	} else {
		offsets[0] = config->client_offset;
		offsets[1] = config->attr_offset;
		offsets[2] = config->width_offset;
		offsets[3] = config->height_offset;
		offsets[4] = config->fps_offset;
		offsets[5] = config->adapter_nr_offset;
		if (!tx_isp_sinfo_offsets_are_u32_aligned(offsets, 6))
			return TX_ISP_SINFO_CONFIG_BAD_OBJECT_OFFSETS;

		offsets[0] = config->attr_name_offset;
		offsets[1] = config->attr_chip_id_offset;
		if (!tx_isp_sinfo_offsets_are_u32_aligned(offsets, 2))
			return TX_ISP_SINFO_CONFIG_BAD_ATTRIBUTE_OFFSETS;
	}

	if (config_flags & TX_ISP_SINFO_EXTENDED_ATTRS) {
		offsets[0] = config->min_fps_offset;
		offsets[1] = config->max_fps_offset;
		if (!tx_isp_sinfo_offsets_are_u32_aligned(offsets, 2))
			return TX_ISP_SINFO_CONFIG_BAD_EXTENDED_OFFSETS;

		if (!(config_flags & (TX_ISP_SINFO_STATIC_METADATA |
				      TX_ISP_SINFO_REGINFO_WIRING))) {
			offsets[0] = config->attr_mclk_offset;
			offsets[1] = config->attr_boot_offset;
			offsets[2] = config->attr_interface_offset;
			offsets[3] = config->attr_rst_gpio_offset;
			offsets[4] = config->attr_pwdn_gpio_offset;
			if (!tx_isp_sinfo_offsets_are_u32_aligned(offsets, 5))
				return TX_ISP_SINFO_CONFIG_BAD_EXTENDED_OFFSETS;
		}
	}

	if ((config_flags & TX_ISP_SINFO_REGINFO_WIRING) &&
	    !(config_flags & TX_ISP_SINFO_STATIC_METADATA)) {
		offsets[0] = config->info_offset;
		offsets[1] = config->info_mclk_offset;
		offsets[2] = config->info_boot_offset;
		offsets[3] = config->info_interface_offset;
		offsets[4] = config->info_rst_gpio_offset;
		offsets[5] = config->info_pwdn_gpio_offset;
		if (!tx_isp_sinfo_offsets_are_u32_aligned(offsets, 6))
			return TX_ISP_SINFO_CONFIG_BAD_REGINFO_OFFSETS;
	}

	return TX_ISP_SINFO_CONFIG_OK;
}

int tx_isp_sinfo_driver_add(struct i2c_driver *drv, int default_i2c_addr,
			    struct module *owner);
void tx_isp_sinfo_driver_del(struct i2c_driver *drv);
int tx_isp_sinfo_get_driver(unsigned int index, char *name,
			    unsigned int name_size,
			    unsigned short *default_i2c_addr);
int tx_isp_sinfo_sensor_bind(void *subdev, struct module *owner);
void tx_isp_sinfo_sensor_unbind(void *subdev, struct module *owner);

int tx_isp_sinfo_init(void);
void tx_isp_sinfo_exit(void);

#endif /* TX_ISP_SINFO_H */
