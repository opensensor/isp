/*
 * T21 sensor-module ABI surface.
 *
 * Keep the export policy separate from the recovered implementation.  The
 * list below is the exact 30-symbol __ksymtab surface of the OEM T21 module;
 * in particular, the sinfo helpers and fixed-point entry points are consumed
 * by the matching sensor/user modules even when the ISP core itself does not
 * call them directly.
 */

#include <linux/clk.h>
#include <linux/i2c.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/types.h>

extern int isp_printf(u32 level, const char *format, ...);

extern int private_capable(int capability);
extern int private_clk_enable(struct clk *clk);
extern void private_clk_disable(struct clk *clk);
extern void private_clk_put(struct clk *clk);
extern int private_clk_set_rate(struct clk *clk, unsigned long rate);
extern int private_driver_get_interface(void);
extern int private_gpio_direction_output(unsigned int gpio, int value);
extern void private_gpio_free(unsigned int gpio);
extern int private_gpio_request(unsigned int gpio, const char *label);
extern int private_i2c_add_driver(struct i2c_driver *driver);
extern void private_i2c_del_driver(struct i2c_driver *driver);
extern void *private_i2c_get_clientdata(const struct i2c_client *client);
extern void private_i2c_set_clientdata(struct i2c_client *client, void *data);
extern int private_i2c_transfer(struct i2c_adapter *adapter,
				struct i2c_msg *messages, int count);
extern int private_jzgpio_set_func(int port, int function,
				    unsigned long pins);
extern s32 private_log2_fixed_to_fixed(u32 value, s32 in_precision,
				       char out_precision);
extern s32 private_log2_int_to_fixed(u32 value, char in_precision,
				     char out_precision);
extern u32 private_math_exp2(s32 value, char in_precision,
			     char out_precision);
extern void private_msleep(unsigned int milliseconds);

extern s32 tisp_log2_fixed_to_fixed(u32 value, s32 in_precision,
				    char out_precision);
extern u32 tisp_math_exp2(s32 value, char in_precision, char out_precision);

extern int tx_isp_exit(void);
extern int tx_isp_init(void);
extern int tx_isp_sinfo_driver_add(void *driver, s16 sensor_id, s32 flags);
extern s32 tx_isp_sinfo_driver_del(u32 driver);
extern s32 tx_isp_sinfo_sensor_bind(u32 sensor, u32 sensor_id);
extern void tx_isp_sinfo_sensor_unbind(s32 sensor_id);
extern s32 tx_isp_subdev_deinit(unsigned long subdev);
extern s32 tx_isp_subdev_init(struct platform_device *device, void *subdev,
			      s32 ops);

EXPORT_SYMBOL(isp_printf);
EXPORT_SYMBOL(private_capable);
EXPORT_SYMBOL(private_clk_disable);
EXPORT_SYMBOL(private_clk_enable);
EXPORT_SYMBOL(private_clk_put);
EXPORT_SYMBOL(private_clk_set_rate);
EXPORT_SYMBOL(private_driver_get_interface);
EXPORT_SYMBOL(private_gpio_direction_output);
EXPORT_SYMBOL(private_gpio_free);
EXPORT_SYMBOL(private_gpio_request);
EXPORT_SYMBOL(private_i2c_add_driver);
EXPORT_SYMBOL(private_i2c_del_driver);
EXPORT_SYMBOL(private_i2c_get_clientdata);
EXPORT_SYMBOL(private_i2c_set_clientdata);
EXPORT_SYMBOL(private_i2c_transfer);
EXPORT_SYMBOL(private_jzgpio_set_func);
EXPORT_SYMBOL(private_log2_fixed_to_fixed);
EXPORT_SYMBOL(private_log2_int_to_fixed);
EXPORT_SYMBOL(private_math_exp2);
EXPORT_SYMBOL(private_msleep);
EXPORT_SYMBOL(tisp_log2_fixed_to_fixed);
EXPORT_SYMBOL(tisp_math_exp2);
EXPORT_SYMBOL(tx_isp_exit);
EXPORT_SYMBOL(tx_isp_init);
EXPORT_SYMBOL(tx_isp_sinfo_driver_add);
EXPORT_SYMBOL(tx_isp_sinfo_driver_del);
EXPORT_SYMBOL(tx_isp_sinfo_sensor_bind);
EXPORT_SYMBOL(tx_isp_sinfo_sensor_unbind);
EXPORT_SYMBOL(tx_isp_subdev_deinit);
EXPORT_SYMBOL(tx_isp_subdev_init);
