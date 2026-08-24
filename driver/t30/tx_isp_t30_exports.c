/*
 * T30 sensor-module ABI surface.
 *
 * Keep exports in one reviewed adapter instead of scattering module policy
 * through the recovered core.  This list matches the Ingenic T30 SDK and the
 * undefined-symbol set of its sensor modules.
 */

#include <linux/clk.h>
#include <linux/i2c.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/types.h>

struct tx_isp_subdev;
struct tx_isp_subdev_ops;

extern int private_clk_enable(struct clk *clk);
extern void private_clk_disable(struct clk *clk);
extern void private_clk_put(struct clk *clk);
extern int private_clk_set_rate(struct clk *clk, unsigned long rate);
extern int private_i2c_transfer(struct i2c_adapter *adapter,
				struct i2c_msg *messages, int count);
extern void private_i2c_del_driver(struct i2c_driver *driver);
extern void *private_i2c_get_clientdata(const struct i2c_client *client);
extern void private_i2c_set_clientdata(struct i2c_client *client, void *data);
extern int private_i2c_add_driver(struct i2c_driver *driver);
extern int private_gpio_request(unsigned int gpio, const char *label);
extern void private_gpio_free(unsigned int gpio);
extern int private_gpio_direction_output(unsigned int gpio, int value);
extern int private_jzgpio_set_func(int port, int function,
				    unsigned long pins);
extern void private_msleep(unsigned int milliseconds);
extern bool private_capable(int capability);
extern int private_driver_get_interface(void);
extern int tx_isp_subdev_init(struct platform_device *device,
			      struct tx_isp_subdev *subdev,
			      struct tx_isp_subdev_ops *ops);
extern void tx_isp_subdev_deinit(struct tx_isp_subdev *subdev);

EXPORT_SYMBOL(private_clk_enable);
EXPORT_SYMBOL(private_clk_disable);
EXPORT_SYMBOL(private_clk_put);
EXPORT_SYMBOL(private_clk_set_rate);
EXPORT_SYMBOL(private_i2c_transfer);
EXPORT_SYMBOL(private_i2c_del_driver);
EXPORT_SYMBOL(private_i2c_get_clientdata);
EXPORT_SYMBOL(private_i2c_set_clientdata);
EXPORT_SYMBOL(private_i2c_add_driver);
EXPORT_SYMBOL(private_gpio_request);
EXPORT_SYMBOL(private_gpio_free);
EXPORT_SYMBOL(private_gpio_direction_output);
EXPORT_SYMBOL(private_jzgpio_set_func);
EXPORT_SYMBOL(private_msleep);
EXPORT_SYMBOL(private_capable);
EXPORT_SYMBOL(private_driver_get_interface);
EXPORT_SYMBOL(tx_isp_subdev_init);
EXPORT_SYMBOL(tx_isp_subdev_deinit);
