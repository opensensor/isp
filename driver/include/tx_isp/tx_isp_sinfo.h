#ifndef TX_ISP_SINFO_H
#define TX_ISP_SINFO_H

struct i2c_driver;
struct module;

#define TX_ISP_SINFO_STATIC_METADATA	(1U << 0)
#define TX_ISP_SINFO_EXTENDED_ATTRS	(1U << 1)
#define TX_ISP_SINFO_REGINFO_WIRING	(1U << 2)

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
};

int tx_isp_sinfo_driver_add(struct i2c_driver *drv, int default_i2c_addr,
			    struct module *owner);
void tx_isp_sinfo_driver_del(struct i2c_driver *drv);
int tx_isp_sinfo_sensor_bind(void *subdev, struct module *owner);
void tx_isp_sinfo_sensor_unbind(void *subdev, struct module *owner);

int tx_isp_sinfo_init(void);
void tx_isp_sinfo_exit(void);

#endif /* TX_ISP_SINFO_H */
